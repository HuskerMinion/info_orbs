#ifndef PIRATEWEATHERFEED_H
#define PIRATEWEATHERFEED_H

#include "../WeatherDataModel.h"
#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <TaskManager.h>

#ifndef WEATHER_PIRATEWEATHER_API_URL
    #define WEATHER_PIRATEWEATHER_API_URL "https://api.pirateweather.net/forecast/"
#endif
#ifndef WEATHER_PIRATEWEATHER_API_KEY
    #define WEATHER_PIRATEWEATHER_API_KEY ""
#endif
#ifndef WEATHER_PIRATEWEATHER_LAT
    #define WEATHER_PIRATEWEATHER_LAT 0
#endif
#ifndef WEATHER_PIRATEWEATHER_LON
    #define WEATHER_PIRATEWEATHER_LON 0
#endif
#ifndef WEATHER_PIRATEWEATHER_NAME
    #define WEATHER_PIRATEWEATHER_NAME ""
#endif

class PirateWeatherFeed : public WeatherFeed {
public:
PirateWeatherFeed(const String &apiKey, int units);
    bool getWeatherData(WeatherDataModel &model) override;
    void setupConfig(ConfigManager &config) override;
    void processResponse(int httpCode, const String &response, WeatherDataModel &model);
    void preProcessResponse(int httpCode, String &response);
    String translateIcon(const std::string &icon);

private:
    String apiKey;
    float m_lat_id = WEATHER_PIRATEWEATHER_LAT;
    float m_long_id = WEATHER_PIRATEWEATHER_LON;
    std::string m_name = WEATHER_PIRATEWEATHER_NAME;

#ifdef WEATHER_UNITS_METRIC
    int m_weatherUnits = 0;
#else
    int m_weatherUnits = 1;
#endif
};

#endif
