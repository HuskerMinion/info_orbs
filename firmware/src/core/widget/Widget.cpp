#include "Widget.h"

Widget::Widget(ScreenManager &manager, ConfigManager &config)
    : m_manager(manager),
      m_config(config),
      m_drawTimer(nullptr),
      m_updateTimer(nullptr) {}

WidgetTimer &Widget::addDrawRefreshFrequency(TimeFrequency frequency) {
    if (m_drawTimer) {
        delete m_drawTimer;
    }
    m_drawTimer = new WidgetTimer(static_cast<uint32_t>(frequency));
    return *m_drawTimer;
}

WidgetTimer &Widget::addUpdateRefreshFrequency(TimeFrequency frequency) {
    if (m_updateTimer) {
        delete m_updateTimer;
    }
    m_updateTimer = new WidgetTimer(static_cast<uint32_t>(frequency));
    return *m_updateTimer;
}

void Widget::resetTimer(WidgetTimer &timer) {
    timer.reset();
}

void Widget::setBusy(bool busy) {
    if (busy) {
        digitalWrite(BUSY_PIN, HIGH);
    } else {
        digitalWrite(BUSY_PIN, LOW);
    }
}

bool Widget::isEnabled() {
    return m_enabled;
}

bool Widget::isItTimeToDraw() {
    if (m_drawTimer) {
        return m_drawTimer->isDue();
    }
    return true;
}

bool Widget::isItTimeToUpdate() {
    if (m_updateTimer) {
        return m_updateTimer->isDue();
    }
    return true;
}

unsigned long Widget::getLastDrawTime() const {
    if (m_drawTimer) {
        return m_drawTimer->getInterval(); // Return the last time isDue() returned true
    }
    return 0; // If no timer, return 0 (no previous draw time)
}

unsigned long Widget::getLastUpdateTime() const {
    if (m_updateTimer) {
        return m_updateTimer->getInterval(); // Return the last time isDue() returned true
    }
    return 0; // If no timer, return 0 (no previous update time)
}
