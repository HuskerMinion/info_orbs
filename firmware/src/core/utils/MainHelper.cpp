#include "MainHelper.h"
#include "LittleFSHelper.h"
#include "Translations.h"
#include "config_helper.h"
#include "icons.h"
#include <ArduinoLog.h>
#include <GlobalTime.h>
#include <HTTPClient.h>
#include <TimeLib.h>
#include <clockwidget/ClockWidget.h>
#include <esp_task_wdt.h>
#include <nvs_flash.h>

static Button buttonLeft;
static Button buttonMiddle;
static Button buttonRight;
static WiFiManager *s_wifiManager = nullptr;
static ConfigManager *s_configManager = nullptr;
static ScreenManager *s_screenManager = nullptr;
static WidgetSet *s_widgetSet = nullptr;
static int s_widgetCycleDelay = WIDGET_CYCLE_DELAY;
static unsigned long s_widgetCycleDelayPrev = 0;
static int s_orbRotation = ORB_ROTATION;
static std::string s_timezoneLocation = TIMEZONE_API_LOCATION;
static std::string s_ntpServer = NTP_SERVER;
static int s_tftBrightness = TFT_BRIGHTNESS;
static bool s_nightMode = DIM_ENABLED;
static int s_dimStartHour = DIM_START_HOUR;
static int s_dimEndHour = DIM_END_HOUR;
static int s_dimBrightness = DIM_BRIGHTNESS;
static int s_languageId = DEFAULT_LANGUAGE;

void MainHelper::init(WiFiManager *wm, ConfigManager *cm, ScreenManager *sm, WidgetSet *ws) {
    s_wifiManager = wm;
    s_configManager = cm;
    s_screenManager = sm;
    s_widgetSet = ws;
    watchdogInit();
}

/**
 * The ISR handlers must be static
 */
void MainHelper::isrButtonChangeLeft() { buttonLeft.isrButtonChange(); }
void MainHelper::isrButtonChangeMiddle() { buttonMiddle.isrButtonChange(); }
void MainHelper::isrButtonChangeRight() { buttonRight.isrButtonChange(); }

void MainHelper::setupButtons() {
    bool invertButtons = s_orbRotation == 1 || s_orbRotation == 2;
    int leftPin = BUTTON_LEFT_PIN;
    int middlePin = BUTTON_MIDDLE_PIN;
    int rightPin = BUTTON_RIGHT_PIN;

    if (invertButtons) {
        leftPin = BUTTON_RIGHT_PIN;
        rightPin = BUTTON_LEFT_PIN;
    }

    buttonLeft.begin(leftPin);
    buttonMiddle.begin(middlePin);
    buttonRight.begin(rightPin);

    attachInterrupt(digitalPinToInterrupt(leftPin), isrButtonChangeLeft, CHANGE);
    attachInterrupt(digitalPinToInterrupt(middlePin), isrButtonChangeMiddle, CHANGE);
    attachInterrupt(digitalPinToInterrupt(rightPin), isrButtonChangeRight, CHANGE);
}

void MainHelper::setupConfig() {
    // Set language here to get i18n strings for the configuration
    I18n::setLanguageId(s_configManager->getConfigInt("lang", DEFAULT_LANGUAGE));
    s_configManager->addConfigString("General", "timezoneLoc", &s_timezoneLocation, 30, t_timezoneLoc);
    String *optLang = I18n::getAllLanguages();
    s_configManager->addConfigComboBox("General", "lang", &s_languageId, optLang, LANG_NUM, t_language);
    s_configManager->addConfigInt("General", "widgetCycDelay", &s_widgetCycleDelay, t_widgetCycleDelay);
    s_configManager->addConfigString("General", "ntpServer", &s_ntpServer, 30, t_ntpServer, true);
    s_configManager->addConfigComboBox("TFT Settings", "orbRotation", &s_orbRotation, t_orbRot, t_orbRotation);
    s_configManager->addConfigBool("TFT Settings", "nightmode", &s_nightMode, t_nightmode);
    s_configManager->addConfigInt("TFT Settings", "tftBrightness", &s_tftBrightness, t_tftBrightness, true);
    String optHours[] = {"0:00", "1:00", "2:00", "3:00", "4:00", "5:00", "6:00", "7:00", "8:00", "9:00", "10:00", "11:00",
                         "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00", "19:00", "20:00", "21:00", "22:00", "23:00"};
    s_configManager->addConfigComboBox("TFT Settings", "dimStartHour", &s_dimStartHour, optHours, 24, t_dimStartHour, true);
    s_configManager->addConfigComboBox("TFT Settings", "dimEndHour", &s_dimEndHour, optHours, 24, t_dimEndHour, true);
    s_configManager->addConfigInt("TFT Settings", "dimBrightness", &s_dimBrightness, t_dimBrightness, true);
}

void MainHelper::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (state == BTN_NOTHING) {
        // nothing to do
        return;
    }
    // Reset cycle timer whenever a button is pressed
    if (buttonId == BUTTON_LEFT && state == BTN_SHORT) {
        // Left short press cycles widgets backward
        Log.noticeln("Left button short pressed -> switch to prev Widget");
        s_widgetCycleDelayPrev = millis();
        s_widgetSet->prev();
    } else if (buttonId == BUTTON_RIGHT && state == BTN_SHORT) {
        // Right short press cycles widgets forward
        Log.noticeln("Right button short pressed -> switch to next Widget");
        s_widgetCycleDelayPrev = millis();
        s_widgetSet->next();
    } else {
        // Everything else that is not BTN_NOTHING will be forwarded to the current widget
        if (buttonId == BUTTON_LEFT) {
            Log.noticeln("Left button pressed, state=%d", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_LEFT, state);
        } else if (buttonId == BUTTON_MIDDLE) {
            Log.noticeln("Middle button pressed, state=%d", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_MIDDLE, state);
        } else if (buttonId == BUTTON_RIGHT) {
            Log.noticeln("Right button pressed, state=%d", state);
            s_widgetCycleDelayPrev = millis();
            s_widgetSet->buttonPressed(BUTTON_RIGHT, state);
        }
    }
}

void MainHelper::checkButtons() {
    ButtonState leftState = buttonLeft.getState();
    if (leftState == BTN_VERY_LONG) {
        Log.noticeln("Left button very long press detected -> Erasing NVS");
        eraseNVSAndRestart();
        return;
    }
    if (leftState != BTN_NOTHING) {
        buttonPressed(BUTTON_LEFT, leftState);
    }
    ButtonState middleState = buttonMiddle.getState();
    if (middleState != BTN_NOTHING) {
        buttonPressed(BUTTON_MIDDLE, middleState);
    }
    ButtonState rightState = buttonRight.getState();
    if (rightState != BTN_NOTHING) {
        buttonPressed(BUTTON_RIGHT, rightState);
    }
}

void MainHelper::checkCycleWidgets() {
    if (s_widgetSet && s_widgetCycleDelay > 0 && (s_widgetCycleDelayPrev == 0 || (millis() - s_widgetCycleDelayPrev) >= s_widgetCycleDelay * 1000)) {
        s_widgetSet->next();
        s_widgetCycleDelayPrev = millis();
    }
}

// Handle simulated button state
void MainHelper::handleEndpointButton() {
    if (s_wifiManager->server->hasArg("name") && s_wifiManager->server->hasArg("state")) {
        String inButton = s_wifiManager->server->arg("name");
        String inState = s_wifiManager->server->arg("state");
        uint8_t buttonId = Utils::stringToButtonId(inButton);
        ButtonState state = Utils::stringToButtonState(inState);

        if (buttonId != BUTTON_INVALID && state != BTN_NOTHING) {
            buttonPressed(buttonId, state);
            s_wifiManager->server->send(200, "text/plain", "OK " + inButton + "/" + inState + " -> " + String(buttonId) + "/" + String(state));
            return;
        }
    }
    s_wifiManager->server->send(500, "text/plain", "ERR");
}

// Show button web page
void MainHelper::handleEndpointButtons() {
    String msg = WEBPORTAL_BUTTONS_PAGE_START1;
    msg += WEBPORTAL_BUTTONS_STYLE;
    msg += WEBPORTAL_BUTTONS_PAGE_START2;
    String buttons[] = {"left", "middle", "right"};
    String states[] = {"short", "medium", "long"};
    int numButtons = 3; // number of buttons
    int numStates = 3; // number of states
    for (int s = 0; s < numStates; s++) {
        msg += "<tr>";
        for (int b = 0; b < numButtons; b++) {
            msg += "<td><button class='sim' onclick=\"sendReq('" + buttons[b] + "', '" + states[s] + "')\">" + buttons[b] + "<br>" + states[s] + "</button></td>";
        }
        msg += "</tr>";
    }
    msg += WEBPORTAL_BUTTONS_PAGE_END1;
    msg += WEBPORTAL_BUTTONS_SCRIPT;
    msg += WEBPORTAL_BUTTONS_PAGE_END2;
    s_wifiManager->server->send(200, "text/html", msg);
}

void MainHelper::handleEndpointListFiles() {
    String html = WEBPORTAL_BROWSE_HTML_START;
    html += WEBPORTAL_BROWSE_STYLE;
    html += WEBPORTAL_BROWSE_SCRIPT;
    html += WEBPORTAL_BROWSE_BODY_START;

    // Get the current directory from the query parameter or default to "/"
    String currentDir = s_wifiManager->server->arg("dir");
    if (currentDir == "") {
        currentDir = "/"; // Default to the root directory
    }

    html += "<h3>Current Directory: " + currentDir + "</h3>";

    // Add "Parent Directory" button
    if (currentDir != "/") {
        String parentDir = currentDir.substring(0, currentDir.lastIndexOf('/', currentDir.length() - 2) + 1);
        html += "<a class='button' href='/browse?dir=" + parentDir + "'>Back to Parent Directory</a>";
    }

    // List files and directories
    File dir = LittleFS.open(currentDir);
    if (!dir.isDirectory()) {
        html += "<p>Not a valid directory</p>";
    } else {
        File file = dir.openNextFile();
        bool first = true;
        while (file) {
            if (file.isDirectory()) {
                if (first) {
                    first = false;
                    html += "<h3>Directories:</h3><ul>";
                }
                html += "<li><a href='/browse?dir=" + currentDir + String(file.name()) + "/'>" + String(file.name()) + "</a></li>";
            }
            file = dir.openNextFile();
        }
        if (!first) {
            html += "</ul>";
        }

        // List files in the current directory
        dir = LittleFS.open(currentDir); // Reset to the beginning to list files
        file = dir.openNextFile();
        first = true;
        while (file) {
            if (!file.isDirectory()) {
                if (first) {
                    first = false;
                    html += "<h3>Files:</h3>";
                }
                String fileName = String(file.name());
                String filePath = currentDir + fileName;

                // Check if the file is a JPG for preview
                String previewHtml = "";
                if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
                    previewHtml = "<img class='preview' src='/download?path=" + filePath + "'>";
                }

                html += "<div class='file-item'><a href='/download?path=" + filePath + "' download>" + previewHtml + "</a>" +
                        fileName + " (" + String(file.size()) +
                        " Bytes) <button class='button delete' onclick=\"confirmDelete('" +
                        String(file.name()) + "', '" + currentDir + "')\">Delete</button></div>";
            }
            file = dir.openNextFile();
        }
    }

    html += WEBPORTAL_BROWSE_UPLOAD_FORM1;
    html += currentDir;
    html += WEBPORTAL_BROWSE_UPLOAD_FORM2;
    html += currentDir;
    html += WEBPORTAL_BROWSE_UPLOAD_FORM3;
    html += currentDir;
    html += WEBPORTAL_BROWSE_UPLOAD_FORM4;

    html += WEBPORTAL_BROWSE_FETCHURL_FORM1;
    html += currentDir;
    html += WEBPORTAL_BROWSE_FETCHURL_FORM2;

    html += WEBPORTAL_BROWSE_HTML_END;

    s_wifiManager->server->send(200, "text/html", html);
}

void MainHelper::handleEndpointDownloadFile() {
    String filePath = s_wifiManager->server->arg("path");

    if (LittleFS.exists(filePath)) {
        File file = LittleFS.open(filePath, "r");
        if (file) {
            s_wifiManager->server->streamFile(file, "application/octet-stream");
            file.close();
        } else {
            s_wifiManager->server->send(500, "text/plain", "Failed to open file.");
        }
    } else {
        s_wifiManager->server->send(404, "text/plain", "File not found.");
    }
}

bool MainHelper::handleEndpointFetchFilesFromURLAction(
    const String &directory, const String &url, const bool showProgress,
    const int customClock, const String &clockName, const String &authorName, const String &secondHandColor, const String &overrideColor) {
    String currentUrl = url;
    Log.noticeln("Fetching files from URL: %s, Directory: %s", url.c_str(), directory.c_str());
    if (!currentUrl.startsWith("http://") && !currentUrl.startsWith("https://")) {
        currentUrl = "https://" + currentUrl;
    }
    if (currentUrl.startsWith("https://github.com")) {
        // Replace GitHub URLs
        currentUrl.replace("github.com", "raw.githubusercontent.com");
        currentUrl.replace("tree", "refs/heads");
    }
    if (showProgress) {
        s_screenManager->setFont(DEFAULT_FONT);
        s_screenManager->clearAllScreens();
        s_screenManager->setFontColor(TFT_WHITE, TFT_BLACK);
        s_screenManager->selectScreen(0);
        s_screenManager->drawCentreString("Downloading", ScreenCenterX, ScreenCenterY, 22);
        if (!clockName.isEmpty() && !authorName.isEmpty() && customClock >= 0) {
            // More info for repo downloads
            s_screenManager->selectScreen(1);
            s_screenManager->drawCentreString("Clockface", ScreenCenterX, ScreenCenterY, 22);
            s_screenManager->selectScreen(2);
            s_screenManager->setFontColor(TFT_DARKGREEN, TFT_BLACK);
            s_screenManager->drawCentreString(clockName, ScreenCenterX, ScreenCenterY - 40, 22);
            s_screenManager->setFontColor(TFT_WHITE, TFT_BLACK);
            s_screenManager->drawCentreString("by", ScreenCenterX, ScreenCenterY, 18);
            s_screenManager->setFontColor(TFT_RED, TFT_BLACK);
            s_screenManager->drawCentreString(authorName, ScreenCenterX, ScreenCenterY + 40, 22);
            s_screenManager->setFontColor(TFT_WHITE, TFT_BLACK);
            s_screenManager->selectScreen(3);
            s_screenManager->drawCentreString("from Repo", ScreenCenterX, ScreenCenterY, 22);
        } else {
            s_screenManager->selectScreen(2);
            s_screenManager->drawCentreString("ClockFace", ScreenCenterX, ScreenCenterY, 22);
        }
    }
    // Download files 0.jpg to 11.jpg
    bool success = true;
    String outputDir = directory;
    if (!outputDir.endsWith("/")) {
        outputDir += "/";
    }
    for (int i = 0; i <= 11; i++) {
        String fileName = String(i) + ".jpg";
        String filePathTemp = outputDir + fileName + ".tmp";
        String fileUrl = currentUrl + "/" + fileName;

        Log.noticeln("Downloading %s to %s", fileUrl.c_str(), filePathTemp.c_str());
        if (showProgress) {
            s_screenManager->clearScreen(4);
            // progress message
            String msg = String(i + 1) + "/12";
            s_screenManager->drawCentreString(msg, ScreenCenterX, ScreenCenterY, 24);
        }

        HTTPClient http;
        // Initialize HTTP connection
        if (http.begin(fileUrl)) {
            int httpCode = http.GET();

            if (httpCode == HTTP_CODE_OK) {
                LittleFS.mkdir(outputDir); // Ensure the directory exists
                File file = LittleFS.open(filePathTemp, "w");
                if (file) {
                    // Read data from the HTTP stream and write it to the file
                    Stream &stream = http.getStream();
                    uint8_t buffer[128]; // Buffer for reading data
                    size_t sizeRead;

                    while ((sizeRead = stream.readBytes(buffer, sizeof(buffer))) > 0) {
                        file.write(buffer, sizeRead);
                    }

                    Log.noticeln("Downloaded: %s (%d)", fileName.c_str(), file.size());
                    file.close();
                    watchdogReset(); // Kick the Watchdog after every download
                } else {
                    Log.errorln("Failed to open file for writing: %s", filePathTemp.c_str());
                    success = false;
                    break;
                }
            } else {
                Log.errorln("Failed to download: %s (HTTP Code: %d)", fileUrl.c_str(), httpCode);
                success = false;
                break;
            }

            http.end();
        } else {
            Log.errorln("Failed to initialize HTTP connection for: %s", fileUrl.c_str());
            success = false;
            break;
        }
    }

    for (int i = 0; i <= 11; i++) {
        String fileName = String(i) + ".jpg";
        String filePath = outputDir + fileName;
        String filePathTemp = filePath + ".tmp";
        if (success) {
            // Rename files
            Log.noticeln("Renaming %s to %s", filePathTemp.c_str(), filePath.c_str());
            LittleFS.rename(filePathTemp, filePath);
        } else {
            // Remove files
            Log.warningln("Removing %s", filePathTemp.c_str());
            LittleFS.remove(filePathTemp);
        }
    }

    if (showProgress) {
        s_screenManager->clearScreen(4);
        if (success) {
            s_screenManager->setFontColor(TFT_DARKGREEN, TFT_BLACK);
            s_screenManager->drawCentreString("Success!", ScreenCenterX, ScreenCenterY, 30);
        } else {
            s_screenManager->setFontColor(TFT_RED, TFT_BLACK);
            s_screenManager->drawCentreString("Failed!", ScreenCenterX, ScreenCenterY, 30);
        }
        delay(2000);
    }
    if (success) {
        // Switch to clock
        if (customClock >= 0) {
            ClockWidget *clockWidget = static_cast<ClockWidget *>(s_widgetSet->getWidgetByName("Clock"));
            if (clockWidget) {
                clockWidget->setCustomClock(customClock, secondHandColor, overrideColor); // Change the clock type to the custom clock
            }
        }
        s_widgetSet->switchToWidgetByName("Clock"); // Switch to Clock and force redraw
        s_widgetCycleDelayPrev = millis(); // Reset the cycle timer
    } else {
        // Force redraw
        s_widgetSet->setClearScreensOnDrawCurrent();
    }
    return success;
}

void MainHelper::handleEndpointFetchFilesFromURL() {
    String url = s_wifiManager->server->arg("url");
    String dir = s_wifiManager->server->arg("dir");
    if (url == "" || dir == "") {
        s_wifiManager->server->send(500, "text/html", "<h2>Invalid URL or directory</h2>");
        return;
    }

    const bool success = handleEndpointFetchFilesFromURLAction(dir, url, true);

    if (success) {
        s_wifiManager->server->send(200, "text/html", "<h2>Files downloaded successfully!</h2><a href='/browse?dir=" + dir + "'>Back to file list</a>");
    } else {
        s_wifiManager->server->send(500, "text/html", "<h2>Failed to download files!</h2><a href='/browse?dir=" + dir + "'>Back to file list</a>");
    }
}

void MainHelper::handleEndpointFetchFilesFromClockRepo() {
    String url = s_wifiManager->server->arg("url");
    int customClock = s_wifiManager->server->arg("customClock").toInt();
    String clockName = s_wifiManager->server->arg("clockName");
    String authorName = s_wifiManager->server->arg("authorName");
    String secondHandColor = s_wifiManager->server->arg("secondHandColor");
    String overrideColor = s_wifiManager->server->arg("overrideColor");
    Log.noticeln("Fetch files from ClockRepo URL: %s, CustomClock: %d, sHC: %s, oC: %s", url.c_str(), customClock, secondHandColor.c_str(), overrideColor.c_str());
    bool success = false;
    if (customClock >= 0 && customClock < USE_CLOCK_CUSTOM) {
        // Valid
        String html = WEBPORTAL_FETCHFROMREPO_HTML_START;
        html += WEBPORTAL_FETCHFROMREPO_STYLE;
        html += WEBPORTAL_FETCHFROMREPO_SCRIPT;
        html += WEBPORTAL_FETCHFROMREPO_BODY_START;
        String dir = "/CustomClock" + String(customClock) + "/";
        html += "<div>Current Directory: " + dir + "/</div>";
        html += WEBPORTAL_FETCHFROMREPO_HTML_END;
        s_wifiManager->server->send(200, "text/html", html);
        success = handleEndpointFetchFilesFromURLAction(dir, url, true, customClock, clockName, authorName, secondHandColor, overrideColor);
    } else {
        Log.errorln("Invalid CustomClock number: %d, max allowed is %d", customClock, USE_CLOCK_CUSTOM - 1);
        String message = "Invalid CustomClock number " + String(customClock) + ", max allowed is " + String(USE_CLOCK_CUSTOM - 1);
        message += "\nIncrease USE_CLOCK_CUSTOM if necessary!";
        s_wifiManager->server->send(500, "text/plain", message);
    }
    Log.noticeln("Fetch files from ClockRepo URL: %s, CustomClock: %d, Success: %d", url.c_str(), customClock, success);
}

void MainHelper::handleEndpointUploadFile() {
    static File fsUploadFile; // Persistent file object for the upload session
    HTTPUpload &upload = s_wifiManager->server->upload();

    String uploadDir = upload.name; // Use the name field as directory (because we can't read arg['dir'] here)
    if (uploadDir == "") {
        uploadDir = "/"; // Default to root directory
    }

    String filePath = uploadDir + upload.filename;

    if (upload.status == UPLOAD_FILE_START) {
        Log.noticeln("Upload Start: %s", filePath.c_str());

        // Ensure the directory exists
        if (!LittleFS.exists(uploadDir)) {
            LittleFS.mkdir(uploadDir);
        }

        // Open the file for writing
        fsUploadFile = LittleFS.open(filePath, "w");
        if (!fsUploadFile) {
            Log.errorln("Failed to open file for writing: %s", filePath.c_str());
            return;
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (fsUploadFile) {
            // Write the file data
            fsUploadFile.write(upload.buf, upload.currentSize);
        } else {
            Log.errorln("File not open for writing during upload: %s", filePath.c_str());
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile) {
            fsUploadFile.close();
            Log.errorln("Upload End: %s", filePath.c_str());
        } else {
            Log.errorln("Failed to close file: %s", filePath.c_str());
        }
    } else if (upload.status == UPLOAD_FILE_ABORTED) {
        if (fsUploadFile) {
            fsUploadFile.close();
            LittleFS.remove(filePath); // Clean up incomplete file
            Log.errorln("Upload Aborted: %s", filePath.c_str());
        }
    }
    // Update ClockWidget (we might be showing the changed custom clock)
    if (s_widgetSet->getCurrent()->getName() == "Clock") {
        s_widgetSet->setClearScreensOnDrawCurrent();
    }
}

void MainHelper::handleEndpointDeleteFile() {
    String fileName = s_wifiManager->server->arg("file");
    String dir = s_wifiManager->server->arg("dir");
    String filePath = dir + fileName;

    if (LittleFS.exists(filePath)) {
        LittleFS.remove(filePath);
        Log.noticeln("File deleted: %s", filePath.c_str());
        s_wifiManager->server->send(200, "text/html", "<h2>File deleted successfully!</h2><a href='/browse?dir=" + dir + "'>Back to file list</a>");
    } else {
        s_wifiManager->server->send(404, "text/html", "<h2>File not found</h2><a href='/browse?dir=" + dir + "'>Back to file list</a>");
    }
}

void MainHelper::handleEndpointCors() {
    Log.noticeln("CORS request");
    s_wifiManager->server->sendHeader(HTTP_HEAD_CORS, HTTP_HEAD_CORS_ALLOW_ALL);
    s_wifiManager->server->sendHeader("Access-Control-Allow-Headers", "content-type");
    s_wifiManager->server->send(200, "text/plain", "OK");
}

void MainHelper::handleEndpointPing() {
    Log.noticeln("Ping request");
    s_wifiManager->server->sendHeader(HTTP_HEAD_CORS, HTTP_HEAD_CORS_ALLOW_ALL);
    String status = "{";
    status += "\"status\": \"OK\"";
    status += ", \"millis\": " + String(millis());
    status += ", \"customClocks\": " + String(USE_CLOCK_CUSTOM);
    status += "}";
    s_wifiManager->server->send(200, "application/json", status);
}

void MainHelper::setupWebPortalEndpoints() {
    // To simulate button presses call e.g. http://<ip>/button?name=right&state=short
    s_wifiManager->server->on("/button", handleEndpointButton);
    s_wifiManager->server->on("/buttons", handleEndpointButtons);
    s_wifiManager->server->on("/browse", HTTP_GET, handleEndpointListFiles);
    s_wifiManager->server->on("/download", HTTP_GET, handleEndpointDownloadFile);
    s_wifiManager->server->on("/fetchFromUrl", HTTP_POST, handleEndpointFetchFilesFromURL);
    s_wifiManager->server->on(
        "/upload", HTTP_POST, [] { s_wifiManager->server->send(200, "text/html", "<h2>File uploaded successfully!</h2><a href='/browse?dir=" + s_wifiManager->server->arg("dir") + "'>Back to file list</a>"); }, handleEndpointUploadFile);
    s_wifiManager->server->on("/delete", HTTP_GET, handleEndpointDeleteFile);

    // Enable API functions (can be called from anywhere, e.g. the public ClockRepo)
    s_wifiManager->server->on("/fetchFromClockRepo", HTTP_GET, handleEndpointFetchFilesFromClockRepo);
    s_wifiManager->server->on("/ping", HTTP_OPTIONS, handleEndpointCors); // CORS
    s_wifiManager->server->on("/ping", HTTP_GET, handleEndpointPing);
}

void MainHelper::showWelcome() {
    s_screenManager->fillAllScreens(TFT_BLACK);
    s_screenManager->setFontColor(TFT_WHITE);

    s_screenManager->selectScreen(0);
    s_screenManager->drawCentreString(i18n(t_welcome), ScreenCenterX, ScreenCenterY, 29);
    if (GIT_BRANCH != "main" && GIT_BRANCH != "unknown" && GIT_BRANCH != "HEAD") {
        s_screenManager->setFontColor(TFT_RED);
        s_screenManager->drawCentreString(GIT_BRANCH, ScreenCenterX, ScreenCenterY - 40, 15);
        s_screenManager->drawCentreString(GIT_COMMIT_ID, ScreenCenterX, ScreenCenterY + 40, 15);
        s_screenManager->setFontColor(TFT_WHITE);
    }

    s_screenManager->selectScreen(1);
    s_screenManager->drawCentreString(i18n(t_infoOrbs), ScreenCenterX, ScreenCenterY - 50, 22);
    s_screenManager->drawCentreString(i18n(t_by), ScreenCenterX, ScreenCenterY - 5, 22);
    s_screenManager->drawCentreString(i18n(t_brettTech), ScreenCenterX, ScreenCenterY + 30, 22);
    s_screenManager->setFontColor(TFT_RED);
    // VERSION is defined in MainHelper.h
    const auto version = String(i18n(t_version)) + " " + String(VERSION);
    s_screenManager->drawCentreString(version, ScreenCenterX, ScreenCenterY + 65, 15);

    s_screenManager->selectScreen(2);
    s_screenManager->drawJpg(0, 0, logo_start, logo_end - logo_start);
}

void MainHelper::resetCycleTimer() {
    s_widgetCycleDelayPrev = millis();
}

void MainHelper::updateBrightnessByTime(uint8_t hour24) {
    uint8_t newBrightness;
    if (s_nightMode) {
        bool isInDimRange;
        if (s_dimStartHour < s_dimEndHour) {
            // Normal case: the range does not cross midnight
            isInDimRange = (hour24 >= s_dimStartHour && hour24 < s_dimEndHour);
        } else {
            // Case where the range crosses midnight
            isInDimRange = (hour24 >= s_dimStartHour || hour24 < s_dimEndHour);
        }
        newBrightness = isInDimRange ? s_dimBrightness : s_tftBrightness;
    } else {
        newBrightness = s_tftBrightness;
    }

    if (s_screenManager->setBrightness(newBrightness)) {
        // brightness was changed -> update widget
        s_screenManager->clearAllScreens();
        s_widgetSet->drawCurrent(true);
    }
}

void MainHelper::restartIfNecessary() {
    if (s_configManager->requiresRestart()) {
        uint32_t start = millis();
        while (millis() - start < 1000) {
            // Answer webportal requests for a short time before restarting
            // to avoid a browser timeout
            s_wifiManager->process();
        }
        Log.noticeln("Restarting ESP now");
        ESP.restart();
    }
}

void MainHelper::setupLittleFS() {
    LittleFSHelper::begin();
#ifdef LITTLEFS_DEBUG
    LittleFSHelper::listFilesRecursively("/");
#endif
}

void MainHelper::watchdogInit() {
    Log.noticeln("Initializing watchdog timer to %d seconds... ", WDT_TIMEOUT);
    // Initialize the watchdog timer for the main task
    if (esp_task_wdt_init(WDT_TIMEOUT, true) == ESP_OK) {
        Log.noticeln("done!");
        // Add the main task to the watchdog
        if (esp_task_wdt_add(nullptr) == ESP_OK) {
            Log.noticeln("Main task added to watchdog.");
        } else {
            Log.errorln("Failed to add main task to watchdog.");
        }
    } else {
        Log.errorln("failed!");
    }
}

void MainHelper::watchdogReset() {
    esp_task_wdt_reset();
}

void MainHelper::printPrefix(Print *_logOutput, int logLevel) {
    char timestamp[20];
#ifdef LOG_NTP_TIME
    time_t now_s = GlobalTime::getUnixEpochIfAvailable(); // local time or zero
#else
    time_t now_s = 0;
#endif
    if (now_s == 0) {
        unsigned long now_ms = millis(); // Fall back to hardware time if we don't have a valid time yet
        now_s = now_ms / 1000;
        sprintf(timestamp, "%02d:%02d:%02d.%03d ", hour(now_s), minute(now_s), second(now_s), now_ms % 1000);
    } else {
        sprintf(timestamp, "%02d:%02d:%02d ", hour(now_s), minute(now_s), second(now_s));
    }
    _logOutput->print(timestamp);
}

void MainHelper::eraseNVSAndRestart() {
    Log.noticeln("Erasing NVS and restarting ESP...");
    nvs_flash_erase();
    nvs_flash_init();
    Log.noticeln("Restarting ESP after NVS erase");
    ESP.restart();
}
