#ifndef VISUALCROSSINGFEED_H
#define VISUALCROSSINGFEED_H

#include "../WeatherDataModel.h"
#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <TaskManager.h>

#ifndef WEATHER_VISUALCROSSING_API_URL
    #define WEATHER_VISUALCROSSING_API_URL "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/"
#endif

class VisualCrossingFeed : public WeatherFeed {
public:
    VisualCrossingFeed(const String &apiKey, int units);
    bool getWeatherData(WeatherDataModel &model) override;
    void setupConfig(ConfigManager &config) override;
    void processResponse(int httpCode, const String &response, WeatherDataModel &model);
    void preProcessResponse(int httpCode, String &response);

private:
    String apiKey;

    std::string m_weatherLocation = WEATHER_VISUALCROSSING_LOCATION;
    int m_units;
};

#endif
