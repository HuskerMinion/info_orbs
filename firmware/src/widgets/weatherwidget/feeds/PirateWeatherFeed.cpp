#include "PirateWeatherFeed.h"
#include "../WeatherTranslations.h"
#include "GlobalTime.h"
#include "TaskFactory.h"
#include "config_helper.h"
#include <unordered_map>

PirateWeatherFeed::PirateWeatherFeed(const String &apiKey, int units)
    : apiKey(apiKey), m_weatherUnits(units) {}

void PirateWeatherFeed::setupConfig(ConfigManager &config) {
    // Define the configuration for OpenWeatherMap variables
    config.addConfigString("WeatherWidget", "pirateWName", &m_name, 15, t_pirateWeatherName);
    config.addConfigFloat("WeatherWidget", "pirateWLat", &m_lat_id, t_pirateWeatherLat);
    config.addConfigFloat("WeatherWidget", "pirateWLong", &m_long_id, t_pirateWeatherLong);
}

bool PirateWeatherFeed::getWeatherData(WeatherDataModel &model) {
    String weatherUnits = m_weatherUnits == 0 ? "si" : "us";
    String lang = I18n::getLanguageString();
    if (lang != "en" && lang != "de" && lang != "fr") {
        lang = "en";
    }

    String httpRequestAddress = String(WEATHER_PIRATEWEATHER_API_URL) + apiKey + "/" + String(m_lat_id,4).c_str() + "," + String(m_long_id,4).c_str() + "?units=" + weatherUnits + "&exclude=minutely,hourly,alerts&lang=" + lang;

    auto task = TaskFactory::createHttpGetTask(
        httpRequestAddress, [this, &model](int httpCode, const String &response) { processResponse(httpCode, response, model); }, [this](int httpCode, String &response) { preProcessResponse(httpCode, response); });

    if (!task) {
        Log.errorln("Failed to create weather task");
        return false;
    }

    bool success = TaskManager::getInstance()->addTask(std::move(task));
    if (!success) {
        Log.errorln("Failed to add weather task");
    }

    return success;
}
void PirateWeatherFeed::preProcessResponse(int httpCode, String &response) {
    if (httpCode > 0) {

        JsonDocument filter;
        filter["currently"]["time"] = true;
        filter["currently"]["temperature"] = true;
        filter["currently"]["summary"] = true;
        filter["currently"]["icon"] = true;

        filter["daily"]["data"][0]["time"] = true;
        filter["daily"]["data"][0]["summary"] = true;
        filter["daily"]["data"][0]["icon"] = true;
        filter["daily"]["data"][0]["temperatureHigh"] = true;
        filter["daily"]["data"][0]["temperatureLow"] = true;

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response, DeserializationOption::Filter(filter));

        if (!error) {
            response = doc.as<String>();
        } else {
            // Handle JSON deserialization error
            Log.errorln("Deserialization failed: %s", error.c_str());
        }
    }
}

void PirateWeatherFeed::processResponse(int httpCode, const String &response, WeatherDataModel &model) {
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            // Set city name
            model.setCityName(m_name.c_str());

            model.setCurrentTemperature(doc["currently"]["temperature"].as<float>());
            model.setCurrentText(doc["currently"]["summary"].as<String>());
            model.setCurrentIcon(translateIcon(doc["currently"]["icon"]));

            model.setTodayHigh(doc["daily"]["data"][0]["temperatureHigh"].as<float>());
            model.setTodayLow(doc["daily"]["data"][0]["temperatureLow"].as<float>());

            Log.traceln("Weather data: %s", response.c_str());
            for (int i = 1; i < 4; i++) {
                model.setDayIcon(i - 1, translateIcon(doc["daily"]["data"][i]["icon"]));
                model.setDayHigh(i - 1, doc["daily"]["data"][i]["temperatureHigh"].as<float>());
                model.setDayLow(i - 1, doc["daily"]["data"][i]["temperatureLow"].as<float>());
            }

        } else {
            // Handle JSON deserialization error
            Log.errorln("Deserialization failed: %s", error.c_str());
        }
    } else {
        Log.errorln("HTTP request failed, error code: %d\n", httpCode);
    }
}
String PirateWeatherFeed::translateIcon(const std::string &icon) {
    // Define the mapping of input strings to simplified weather icons
    static const std::unordered_map<std::string, std::string> iconMapping = {
        {"clear-day", "clear-day"},
        {"clear-night", "clear-night"},
        {"partly-cloudy-day", "partly-cloudy-day"},
        {"partly-cloudy-night", "partly-cloudy-night"},
        {"rain", "rain"},
        {"snow", "snow"},
        {"sleet", "rain"},
        {"cloudy", "rain"},
        {"fog", "fog"},
        {"wind", "clear-day"}};

    // Find the icon in the mapping and return the corresponding simplified value
    auto it = iconMapping.find(icon);
    if (it != iconMapping.end()) {
        return String(it->second.c_str());
    }
    return "clear-day";
}
