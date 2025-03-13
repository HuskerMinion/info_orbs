#include "LittleFSHelper.h"
#include <ArduinoLog.h>

bool LittleFSHelper::begin() {
    if (!LittleFS.begin()) {
        Log.warningln("LittleFS Mount Failed. Formatting...");
        LittleFS.format();
        if (!LittleFS.begin()) {
            Log.warningln("Failed to mount LittleFS after formatting.");
            return false;
        }
    }
    Log.infoln("LittleFS mounted successfully.");
    return true;
}

void LittleFSHelper::writeFile(const char *path, const char *message) {
    File file = LittleFS.open(path, "w");
    if (!file) {
        Log.warningln("Failed to open file for writing");
        return;
    }
    file.print(message);
    file.close();
    Log.infoln("File written successfully.");
}

void LittleFSHelper::readFile(const char *path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Log.warningln("Failed to open file for reading");
        return;
    }
    Log.infoln("Reading file: %s", path);
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

void LittleFSHelper::deleteFile(const char *path) {
    if (LittleFS.remove(path)) {
        Log.infoln("File deleted successfully.");
    } else {
        Log.warningln("Failed to delete file.");
    }
}

void LittleFSHelper::listFilesRecursively(const char *dirname) {
    // Ensure the path starts with a `/`
    if (dirname[0] != '/') {
        Log.warningln("Path must start with '/'");
        return;
    }

    File root = LittleFS.open(dirname);
    if (!root || !root.isDirectory()) {
        Log.warningln("Failed to open directory: %s\n", dirname);
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            // Recursive call for subdirectories
            char fullPath[128];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirname, file.name());
            Log.infoln("Directory: %s", fullPath);
            listFilesRecursively(fullPath);
        } else {
            Log.infoln("File: %s/%s, Size: %d", dirname, file.name(), file.size());
        }
        file = root.openNextFile();
    }
}
