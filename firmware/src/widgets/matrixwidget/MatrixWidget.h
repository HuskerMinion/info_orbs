#ifndef MATRIX_WIDGET_H
#define MATRIX_WIDGET_H

#include "DigitalRainAnimation.hpp"
#include "Widget.h"
#include "config_helper.h"
#include <TFT_eSPI.h>

class MatrixWidget : public Widget {
public:
    MatrixWidget(ScreenManager &manager, ConfigManager &config);
    void setup() override;
    void update(bool force) override;
    void draw(bool force) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;

private:
    DigitalRainAnimation matrix_effect;

    bool m_bigFont = false;
    int m_textColor = 0x001F;
    int m_headTextColor = 0x07FF;
    int m_lineMin = 3;
    int m_lineMax = 15;
    int m_speedMin = 3;
    int m_speedMax = 15;
    int m_updateInterval = 100;
};

#endif
