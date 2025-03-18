# Developer Guide for Using ArduinoLog

When using the `ArduinoLog` library, the logging methods correspond to different log levels, which help you control the verbosity of your logging output. The log levels are hierarchical, meaning that enabling a specific log level will also enable all log levels above it.

## Log Levels and Methods

1. **`LOG_LEVEL_SILENT`**:

   - No logging output.
   - No corresponding `Log.` method.

2. **`LOG_LEVEL_FATAL`**:

   - Only the most critical errors.
   - Corresponds to `Log.fatal()`.
   - Example: `Log.fatal("This is a fatal error");`

3. **`LOG_LEVEL_ERROR`**:

   - Critical errors.
   - Corresponds to `Log.error()`.
   - Example: `Log.error("This is an error");`

4. **`LOG_LEVEL_WARNING`**:

   - Warnings and errors.
   - Corresponds to `Log.warning()`.
   - Example: `Log.warning("This is a warning");`

5. **`LOG_LEVEL_INFO` and `LOG_LEVEL_NOTICE`**:

   - Informational messages, warnings, and errors.
   - Corresponds to `Log.info()` and `Log.notice()`.
   - Recommend to use `Log.ino()` as `Log.notice()` only kept for backward compatability
   - Example: `Log.info("This is an info message");`

6. **`LOG_LEVEL_TRACE`**:

   - Detailed tracing information, notices, informational messages, warnings, and errors.
   - Corresponds to `Log.trace()`.
   - Example: `Log.trace("This is a trace message");`

7. **`LOG_LEVEL_VERBOSE`**:
   - Very detailed tracing information, traces, notices, informational messages, warnings, and errors.
   - Corresponds to `Log.verbose()`.
   - Example: `Log.verbose("This is a verbose message");`

## Setting the Log Level

You can set the log level globally in your `config.h` file. For example:

```cpp
#define LOG_LEVEL LOG_LEVEL_TRACE
```

To use logging in your code, include the following:

```cpp
#include <ArduinoLog.h>
```

### Examples

Note: all of these methods have variants without the trailing 'ln' e.g. Log.info(). The difference being the ln variant adds a newline character (\n))

```cpp
Log.infoln("This is an info message.");
Log.warningln("Warning: Device is overheating!");
Log.errorln("Error: Sensor not found.");
Log.fatalln("Fatal: System crash imminent!");
Log.infoln("Sensor value: " + String(sensorValue));

Log.infoln("Count: %d", count);
Log.infoln("Status: %s, Value: %d", status.c_str(), value);
Log.warningln("Temperature: %.2f°C", temperature);
Log.errorln("Error %d: %s", errorCode, errorMessage.c_str());
Log.traceln("Loop iteration: %d", loopCount);

Log.infonln("Device: " + String(deviceName) + ", ID: %d", deviceId);
Log.warninln("Battery level: %d%%, Status: %s", batteryLevel, status.c_str());
```

Do NOT use Arduino Strings directly for %s, but use .c_str(), like in Serial.printf()

## General Guidance

1. Opt for using `Log` instead of `Serial.print` where possible.
2. Keep `INFO`, `NOTICE` log messages limited and concise to avoid cluttering the serial monitor with messages that may not be meaningful (`INFO` and `NOTICE` are synonymous).
3. Use `WARNING` log messages for unexpected occurrences during execution that may not cause immediate issues (e.g., "Only 4 stock symbols defined").
4. Use `TRACE` log messages for debugging purposes, such as tracking execution flow or dumping JSON returned from an API call.
5. Use `WARNING` for less critical errors, `ERROR` for more critical errors, and `FATAL` only for the most critical errors.
