#ifndef VISUALCROSSINGFEED_H
#define VISUALCROSSINGFEED_H

#include "../WeatherDataModel.h"
#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <TaskManager.h>

#ifndef WEATHER_VISUALCROSSING_API_URL
    #define WEATHER_VISUALCROSSING_API_URL "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/"
#endif
#ifndef WEATHER_VISUALCROSSING_API_KEY
    #define WEATHER_VISUALCROSSING_API_KEY "XW2RDGD6XK432AF25BNK2A3C7" // Your Visual Crossing API key (https://www.visualcrossing.com/sign-up/)
#endif
#ifndef WEATHER_VISUALCROSSING_LAT
    #define WEATHER_VISUALCROSSING_LAT 42.0711 // just a default, change to your location Lat
#endif
#ifndef WEATHER_VISUALCROSSING_LON
    #define WEATHER_VISUALCROSSING_LON -87.9652 // just a default, change to your location Lon
#endif
#ifndef WEATHER_VISUALCROSSING_NAME
    #define WEATHER_VISUALCROSSING_NAME "Chicago" // just a default, change to your location name
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
    float m_lat_id = WEATHER_VISUALCROSSING_LAT;
    float m_long_id = WEATHER_VISUALCROSSING_LON;
    std::string m_weatherLocation = WEATHER_VISUALCROSSING_LOCATION;
    int m_units;
};

#endif
