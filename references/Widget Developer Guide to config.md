# How-To Guide for Widget Developers: Managing Configuration Variables in InfoOrb

This guide explains how to manage configuration variables for your widgets in the **InfoOrb** application. InfoOrb uses a hierarchy of configuration files to determine the values of variables. Understanding this hierarchy and how to define your variables correctly is essential for ensuring your widget functions as expected.

---

## Configuration File Hierarchy

InfoOrb looks for configuration variables in the following order:

1. **`/config/config.h`**:

   - This file is optional and is used to override any default or system-defined variables.
   - If a variable is defined here, it takes precedence over all other definitions.

2. **`/config/config.system.h`**:

   - This file contains system-wide default variables.
   - If a variable is not found in `config.h` or the `config.h` file doesn't exist, InfoOrb will look for it here.

3. **Widget Header Files (`.h` files)**:
   - Variables defined in your widget’s header file are used as fallbacks if they are not found in `config.h` or `config.system.h`.
   - Your widget should define all the variables it needs to function in its `.h` file.

---

## Rules for Defining Variables

1. **Define Variables in Your Widget’s `.h` File**:

   - Always define all the variables your widget needs in its header file. This ensures your widget can function even if no overrides are provided.

2. **Use `#ifndef` for Overridable Variables**:

   - If a variable might need to be overridden, wrap its definition in an `#ifndef` block. This ensures the variable is only defined if it hasn’t already been defined elsewhere.  
     Example:

   ```c
   #ifndef MYWIDGET_VARIABLE_1
       #define MYWIDGET_VARIABLE_1 14
   #endif
   ```

3. **Add Variables to `config.system.h` for Maintainability**:
   - In some cases, it may make sense to add your variable definitions to `config.system.h`. This is especially useful for system-wide builds where an admin might want to define all default variables in one place before a build.

---

## Best Practices for `config.h` and `config.h.template`

1. **`config.h` is Optional**:

   - Your code should be able to run without a `config.h` file. This file is only used for end-user or developer overrides.

2. **`config.h.template` is a Suggestion Template**:

   - The `config.h.template` file is **not used by the app**. It is **only a template** that provides suggested overrides for `config.h`.
   - Developers can optionally add variables to `config.h.template` **only in cases where it’s likely that an end user may want to override one of your variables**.
   - Users can copy `config.h.template` to `config.h` and modify it as needed.
   - Adding variables to `config.h.template` is entirely optional and should only be done when it makes sense for end-user customization.

3. **When to Use `config.h`**:
   - **Developers**: For testing and development purposes.
   - **Custom Builds**: For users who want to customize their build beyond what the web UI allows.
   - **Local Development**: For developers building from VS Code instead of using the web UI.
   - **Non-Standard Widgets**: For enabling or using widgets that are disabled by default.

---

## Example Workflow

1. **Define Variables in Your Widget’s `.h` File**:

   ```c++
   #ifndef MYWIDGET_VARIABLE_1
       #define MYWIDGET_VARIABLE_1 14
   #endif
   ```

2. **Optionally Add Variables to `config.system.h`**:

   ```c++
   #ifndef MYWIDGET_VARIABLE_1
       #define MYWIDGET_VARIABLE_1 14
   #endif
   ```

3. **Optionally Provide Override Suggestions in `config.h.template`**:

   - Only add variables to `config.h.template` if it’s likely that an end user may want to override them.  
     Example:

   ```c++
   // Uncomment the following line to override MYWIDGET_VARIABLE_1
   // #define MYWIDGET_VARIABLE_1 20
   ```

4. **Users Customize `config.h` (if needed)**:
   - If a user needs to make overrides, they can either:
     - Copy config.h.template to config.h and uncomment/modify the desired overrides.
     - Create config.h from scratch and manually to define their overrides if they prefer not to use the template.

---

## Summary

- Define all required variables in your widget’s `.h` file.
- Use `#ifndef` to allow overrides.
- Optionally add variables to `config.system.h` for easy maintence of defaults.
- Use `config.h` for overrides, but ensure your widget can run without it.
- Optionally provide override suggestions in `config.h.template` **only if it’s likely an end user may want to override the variable**.

By following these guidelines, you’ll ensure your widget is flexible, maintainable, and compatible with InfoOrb’s configuration system.

---
