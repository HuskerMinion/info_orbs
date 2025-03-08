#ifndef WIDGET_SET_H
#define WIDGET_SET_H

#include "ScreenManager.h"
#include "Utils.h"
#include "Widget.h"

#ifndef MAX_WIDGETS
    #define MAX_WIDGETS 5
#endif

class WidgetSet {
public:
    WidgetSet(ScreenManager *sm);
    void add(Widget *widget);
    void drawCurrent(bool force = false);
    void updateCurrent();
    Widget *getCurrent();
    void next();
    void prev();
    void buttonPressed(uint8_t buttonId, ButtonState state);
    void showLoading();
    void updateAll();
    bool initialUpdateDone();
    void initializeAllWidgetsData();
    void setClearScreensOnDrawCurrent();
    bool isItTimeToDraw();
    bool isItTimeToUpdate();
    Widget *getWidgetByName(const String &name);
    void switchToWidgetByName(const String &name);

private:
    void showCenteredLine(int screen, const String &text);
    ScreenManager *m_screenManager;
    bool m_clearScreensOnDrawCurrent = true;
    Widget *m_widgets[MAX_WIDGETS];
    uint8_t m_widgetCount = 0;
    uint8_t m_currentWidget = 0;

    bool m_initialized = false;

    void switchWidget();

protected:
    WidgetTimer *m_drawTimer = nullptr;
    WidgetTimer *m_updateTimer = nullptr;
};
#endif // WIDGET_SET_H