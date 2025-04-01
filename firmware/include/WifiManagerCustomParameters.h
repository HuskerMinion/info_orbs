#ifndef WIFIMGR_CUSTOM_PARAMETERS_H
#define WIFIMGR_CUSTOM_PARAMETERS_H

#include "OrbsWiFiManager.h"
#include "Utils.h"

const char checkboxHtml[] = "type='checkbox'";
const char checkboxHtmlChecked[] = "type='checkbox' checked";
const char colorHtml[] = "type='color'";
const char numberHtml[] = "type='number'";

class StringParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
    using WiFiManagerParameter::setValue; // make parent function private
public:
    StringParameter(const char *id, const char *placeholder, std::string value, const uint8_t length = 30)
        : WiFiManagerParameter(id, placeholder, nullptr, length) {
        setValue(value);
    }

    std::string getValue() {
        return WiFiManagerParameter::getValue();
    }

    void setValue(const std::string &value) {
        WiFiManagerParameter::setValue(value.c_str(), getValueLength());
    }
};

class IPAddressParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
    using WiFiManagerParameter::setValue; // make parent function private
public:
    IPAddressParameter(const char *id, const char *placeholder, IPAddress address) {
        init(id, placeholder, nullptr, 16, "", WFM_LABEL_BEFORE);
        setValue(address);
    }

    bool getValue(IPAddress &ip) {
        return ip.fromString(WiFiManagerParameter::getValue());
    }

    void setValue(const IPAddress &address) {
        WiFiManagerParameter::setValue(address.toString().c_str(), getValueLength());
    }
};

class IntParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
    using WiFiManagerParameter::setValue; // make parent function private
public:
    IntParameter(const char *id, const char *placeholder, long value, const uint8_t length = 10) {
        init(id, placeholder, nullptr, length, numberHtml, WFM_LABEL_BEFORE);
        setValue(value);
    }

    long getValue() {
        return String(WiFiManagerParameter::getValue()).toInt();
    }

    void setValue(const long value) {
        WiFiManagerParameter::setValue(String(value).c_str(), getValueLength());
    }
};

class BoolParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
    using WiFiManagerParameter::setValue; // make parent function private
public:
    BoolParameter(const char *id, const char *placeholder, bool value, const uint8_t length = 2) {
        // We use 1 as value here and check later if the argument was passed back when submitting the form
        // If the checkbox is checked, the argument will be there
        init(id, placeholder, "1", length, value ? checkboxHtmlChecked : checkboxHtml, WFM_LABEL_BEFORE);
    }

    bool getValue(WiFiManager &wm) {
        // If the checkbox is selected, the call to "/paramsave" will have an argument with our id
        return wm.server->hasArg(_id);
    }

    void setValue(const bool value) {
        // TODO
        // we need to update the custom html here, but WifiManager does not allow that
        // i.e. we would need to fork WifiManager for this...
    }
};

class ColorParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
    using WiFiManagerParameter::setValue; // make parent function private
public:
    ColorParameter(const char *id, const char *placeholder, int value, const uint8_t length = 8) {
        init(id, placeholder, nullptr, length, colorHtml, WFM_LABEL_BEFORE);
        setValue(value);
    }

    int getValue() {
        return Utils::rgb888htmlToRgb565(WiFiManagerParameter::getValue());
    }

    void setValue(const int color565) {
        return WiFiManagerParameter::setValue(Utils::rgb565ToRgb888html(color565).c_str(), getValueLength());
    }
};

class ComboBoxParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
    using WiFiManagerParameter::setValue; // make parent function private
public:
    ComboBoxParameter(const char *id, const char *placeholder, String options[], int numOptions, int value, const uint8_t length = 0) {
        String html = "<label for='" + String(id) + "'>" + String(placeholder) + "</label><br/><select id='" + String(id) + "' name='" + String(id) + "'>";
        for (int i = 0; i < numOptions; i++) {
            html += "<option value='" + String(i) + "'" + (value == i ? " selected>" : ">") + options[i] + "</option>";
        }
        html += "</select>";
        htmlBuffer = Utils::createConstCharBuffer(html.c_str());
        // Set id as label here, otherwise WifiMgr will show an input
        init(NULL, id, nullptr, 0, htmlBuffer, WFM_LABEL_AFTER);
    }

    ~ComboBoxParameter() {
        // cleanup memory
        delete[] htmlBuffer;
    }

    int getValue(WiFiManager &wm) {
        // The combobox will return its value via "/paramsave" argument
        if (wm.server->hasArg(_label)) {
            String arg = wm.server->arg(_label);
            return arg.toInt();
        }
        // Fallback to 0
        return 0;
    }

    void setValue() {
        // TODO
        // we need to update the custom html here, but WifiManager does not allow that
        // i.e. we would need to fork WifiManager for this...
    }

private:
    const char *htmlBuffer;
};

class FloatParameter : public WiFiManagerParameter {
    using WiFiManagerParameter::getValue; // make parent function private
    using WiFiManagerParameter::setValue; // make parent function private
public:
    FloatParameter(const char *id, const char *placeholder, float value, const uint8_t length = 10) {
        init(id, placeholder, nullptr, length, "", WFM_LABEL_BEFORE);
        setValue(value);
    }

    float getValue() {
        return String(WiFiManagerParameter::getValue()).toFloat();
    }

    void setValue(const float value) {
        WiFiManagerParameter::setValue(String(value, 4).c_str(), getValueLength());
    }
};

#endif // WIFIMGR_CUSTOM_PARAMETERS_H
