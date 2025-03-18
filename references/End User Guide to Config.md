# How-To Guide for End Users: Customizing Configuration in InfoOrb

This guide explains how to customize configuration variables in the **InfoOrb** application. Most users will not need to modify configuration files, as they can manage settings through the app’s web interface. However, in some cases, advanced users may want to override settings using a `config.h` file. This guide will walk you through the process.

---

## When to Use `config.h`

Creating a `config.h` file is **optional** and only needed in specific cases:

1. **Building from Source**: If you’re building the app from source code (e.g., using VS Code) instead of using the prebuilt binary from the web flasher.
2. **Non-Standard Widgets**: If you want to enable or use widgets that are disabled by default.
3. **Advanced Customization**: If you need to change settings that cannot be modified through the web UI.

**Note**: Most users will flash the device using the **web flasher**, which uses a prebuilt binary. In these cases, the `config.h` file does not come into play.

---

## Configuration File Hierarchy

InfoOrb looks for configuration variables in the following order:

1. **`/config/config.h`**:

   - This file is optional and is used to override any default or system-defined variables.
   - If a variable is defined here, it takes precedence over all other definitions.

2. **`config.system.h`**:

   - This file contains system-wide default variables.
   - If a variable is not found in `config.h`, InfoOrb will use the value defined here.

3. **Widget Header Files (`.h` files)**:
   - Variables defined in widget header files are used as fallbacks if they are not found in `config.h` or `config.system.h`.

---

## Web Interface vs. `config.h`

### Web Interface

- Many variables can be maintained using the app’s **web interface**.
- When you save settings in the web interface, they are stored in memory on the device.
- **Important**: If you have saved settings in the web interface, any changes you make in `config.h` will **NOT take effect** until you clear the settings in the web interface.

### `config.h`

- If you are **not using the web-based configuration**, you can use `config.h` to configure your settings.
- **Note**: Not all variables are maintainable in the web UI. Some settings can only be changed via `config.h`.

---

## How to Customize Configuration

### Option 1: Use `config.h.template` (Recommended)

The `config.h.template` file contains suggested overrides for common variables. To customize your configuration:

1. **Copy `config.h.template` to `config.h`**:

   - Locate the `config.h.template` file in the `/config` directory.
   - Make a copy of this file and rename it to `config.h`.

2. **Modify `config.h`**:
   - Open the `config.h` file in a text editor.
   - Uncomment and modify the variables you want to override.  
     Example:
   ```c++
   // Uncomment the following line to override MYWIDGET_VARIABLE_1
   // #define MYWIDGET_VARIABLE_1 20
   ```

### Option 2: Create `config.h` from Scratch

If you prefer not to use the template, you can create a `config.h` file manually:

1. **Create a New File**:

   - In the `/config` directory, create a new file named `config.h`.

2. **Add Your Overrides**:
   - Define the variables you want to override in the file.  
     Example:
   ```c++
   #define MYWIDGET_VARIABLE_1 20
   ```

---

## Clearing Web-Based Settings

If you have saved settings in the web interface and want changes in `config.h` to take effect, you must clear the web-based settings:

1. **Press and Hold the Left Button for 15 Seconds**:

   - Press and hold the left button for **15 seconds**.
   - This will clear all web-based settings, including Wi-Fi credentials.
   - Once you release the button, the device will **reboot automatically**.

2. **Reboot the Device**:
   - After clearing the settings, the device will reboot, and changes from `config.h` will take effect.

---

## Important Notes

- **`config.h` is Optional**:
  - Your app will run without a `config.h` file. Only create one if you need to override specific variables.
- **First Definition Wins**:
  - If a variable is defined in `config.h`, it will override any definition in `config.system.h` or widget header files.
- **Reverting to Defaults**:
  - To revert to default settings, simply delete or rename the `config.h` file.
  - To revert web-based settings, **press and hold the left button for 15 seconds** to clear the web UI configuration.

---

## Summary

- Most users will use the **web flasher** and do not need to worry about `config.h`.
- Use the **web interface** to manage settings whenever possible.
- Create a `config.h` file only if you’re building from source or need to override settings not available in the web UI.
- Clear web-based settings by **pressing and holding the left button for 15 seconds** if you want `config.h` changes to take effect. The device will reboot automatically once released.

By following this guide, you can customize InfoOrb’s configuration to suit your needs while ensuring the app remains stable and functional.

---
