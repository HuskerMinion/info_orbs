#ifndef WEATHERFEED_H
#define WEATHERFEED_H

#include "ConfigManager.h" // Include ConfigManager
#include "WeatherDataModel.h"

class WeatherFeed {
public:
    virtual bool getWeatherData(WeatherDataModel &model) = 0;
    virtual void setupConfig(ConfigManager &config) = 0;
    virtual ~WeatherFeed() = default;
};

#endif
