# Glove80 Fin Mouse
The Glove80 Fin Mouse is a custom mouse designed to be used with the [Glove80 keyboard](https://www.moergo.com/collections/glove80-keyboards). It features a unique "fin" design that allows for easy access to a clickable PS4 joystick, making it an ideal companion for the ergonomic keyboard. Using two "fin mice" (one attached to each half of the Glove80) allows for a more complete mouse experience, allowing for both mouse movement and scrolling, as well as separating right and left clicks.

Shoutout to [jhwblender](https://github.com/jhwblender) for helping me get started on this project!

Disclaimer: Neither I nor this project are affiliated with Moergo. I'm just a happy owner of the Glove80 keyboard sharing my [customization](https://www.moergo.com/pages/glove80-ergonomic-keyboard-customization) with the community. The Glove80 Fin Mouse is not an official product of Moergo, and any issues or concerns should be directed to me (using the Issues and PRs of this repo), not Moergo.

## Features
- Ergonomic design
- Clickable PS4 joystick
- Compatible with Glove80 keyboard
- Two "fin mice" can be used for a complete mouse experience
- Easy access to mouse movement and scrolling
- Customizable buttons
- Lightweight and portable
- Durable design
- Compatible with Windows, Linux, Android, MacOS*, and iOS*
- BLE wireless connection
- USB-C charging

*See [BLE library's notes](https://github.com/T-vK/ESP32-BLE-Mouse?tab=readme-ov-file#features) for MacOS and iOS compatibility.

## Known Issues/Limitations
- The Glove80 Fin Mouse is not compatible with the Glove80 keyboard case. The fin rises above the keyboard, making it impossible to close the case.
- This is not inherently a precise mouse. This is due to (but not necessarily limited to) the use of potentiometers in the joystick, elecrical noise when using the ESP32's ADC (analog-to-digital converter), the processing power of the ESP32, and processing required for the BLE stack. It's technically possible adjust the movement settings to make it precise, but it might result in a poor user experience because of the difficulty to make faster movements.
- There might be an issue where the BLE stack disconnects after a long period of time (possibly due to inactivity). Troubleshooting this is still a work in progress.
- The Glove80 Fin Mouse is not a "plug and play" device. You will need to flash the firmware to the ESP32 before using it. This is a one-time process, but it does require some technical knowledge.
- At least for the joystick components I found, the physical pressing takes more force than I appreciate. This makes it harder to press it with just your thumb, but I found that using my index finger to support the back of the fin (essentially squeezing the fin) isn't too bad.

## Building the Physical Mouse
### Parts List
- 2x [PS4 joysticks](https://www.aliexpress.us/item/3256803704783280.html)
- 2x BLE-capable programmable microcontrollers (I currently use the [ESP32-C3](https://www.amazon.com/dp/B0DPSNVWB2). It's cheap, small, and low-power -- longer battery life, but limits processing power)
- 5x small wires (22 - 24 AWG or smaller), each about 15cm long
- 3d printed parts (see [3D printing](#3d-printing) section)
- 9x M2.5x10 screws
- Battery parts (TBD)

### 3D Printing
The files are located on [Thingiverse](https://www.thingiverse.com/thing:7019530). If you have the grey Glove80, I've found that the [Overture "Cement Gray" PLA filament](https://www.amazon.com/dp/B09BQN3N7P) is very close to the same color.

Should you wish to customize the design, you can find the original models on [OnShape](https://cad.onshape.com/documents/4e451ebac8f57a58c4bef2be/w/d6a3f2496c56cfa3aa2b1f4d/e/0608c30a6e92b27f488ea5b7?renderMode=0&uiState=680a9fd49b7459745121b0ab). I'm not a professional designer, and this model is fairly complex, so my appologies for any aspects that are designed abnormally and/or challenging to work with. Hopefully the variables make it easier to customize overall!

### Assembly
1. Print the parts using a 3D printer.
1. Solder the PS4 joystick to the small wires.
1. Assemble the joystick and fin using 3x M2.5x10 screws.
1. Mount the fin to the Glove80 sidecar using 4x M2.5x10 screws.
1. Solder the wires to the ESP32-C3 (see [Wiring](#wiring) section).
1. Connect the battery to the ESP32-C3.
1. Mount the ESP32-C3 and battery to the underside of the sidecar.
1. Mount the sidecar to the Glove80 keyboard using 2x M2.5x10 screws.
1. Upload the firmware to the ESP32-C3 (see [Flashing the Firmware](#flashing-the-firmware) section).
1. Test the mouse to ensure it is working properly.

### Wiring
The wiring is fairly simple. The hard part is the soldering. This list of connections describes which ESP32 GPIO pins connect to which parts of the joystick component.

- `GND` -> one side of the button, and one side of each potentiometer
- `3.3v` -> the other side of each potentiometer
- `0` -> the center of the X-axis potentiometer
- `1` -> the center of the Y-axis potentiometer
- `2` -> the other side of the button
- `3` -> the battery voltage divider (see [Battery](#battery) section for more details)

### Battery
The battery is not yet fully designed. The current plan is to use a 3.7V lithium-ion battery with a USB-C charging circuit. The battery will be connected to the ESP32-C3 using a voltage divider to measure the battery level. The voltage divider will consist of two resistors (R1 and R2) connected in series between the battery and ground. The voltage at the junction of the two resistors will be connected to an analog pin on the ESP32-C3.

## Flashing the Firmware
### Prerequisites
The firmware has been developed with [PlatformIO in VS Code](https://platformio.org/platformio-ide). Just get that setup, and you should be good to go! Compatibility with Arduino IDE is not guaranteed, but you can try it if you want. If you do, please let me know if it works or not.

### Dependencies
The only dependency is the [ESP32-BLE-Mouse library](https://github.com/T-vK/ESP32-BLE-Mouse). This library is included in the project, so you don't need to install it separately. However, it does appear to be abandoned, and so it might be beneficial to review forks and PRs of that repo to see if there are any improvements that can be made.

### Steps
1. Clone the repository to your local machine.
1. Open the project in VS Code.
1. Customize the firmware to your liking (see [Customizing the Firmware](#customizing-the-firmware) section).
1. Connect the ESP32-C3 to your computer using a USB cable.
1. Select the correct board and port in PlatformIO.
1. Build the project by clicking on the checkmark icon in the bottom left corner of the window.
1. Upload the firmware to the ESP32-C3 by clicking on the right arrow icon in the bottom left corner of the window.
1. Wait for the upload to complete.
1. Open the serial monitor to see the output from the ESP32-C3.
1. Test the mouse to ensure it is working properly.

## Customizing the Firmware
The firmware is designed to be easily customizable. You can change the following settings in the `src/main.cpp` file:

### Mouse Settings
- `MOUSE_UPDATE_INTERVAL`: The time between mouse updates in milliseconds. A lower value will make the mouse more responsive, but it will also use more resources. On the ESP32-C3, I found that going lower than 60 - 70 ms caused the BLE stack to get overloaded and the mouse to become incredibly laggy.
- `MOUSE_DEAD_BAND`: The minimum movement required to register a mouse move. You want this to be as low as possible, but high enough to avoid noise from the circuitry.
- `MOUSE_SENSITIVITY`: The sensitivity of the mouse. A higher value will make the mouse move faster.
- `MOUSE_EXPONENT_THRESHOLD`: The threshold above which to apply exponential scaling. A value of 0 will make the mouse movement linear through the full range of motion.
- `MOUSE_EXPONENT`: The scale factor for mouse movement. A value of 1.0 will make the mouse movement linear, while a value greater than 1 will make it exponential once the threshold has been reached.
- `MOUSE_MAX_SPEED`: The maximum speed of the mouse in pixels per `MOUSE_UPDATE_INTERVAL`. A higher value will make the mouse move faster. However, going too high may result in some eradic behavoir. I recommend staying below 2000.

### Battery Settings
- `BATT_CHECK_INTERVAL`: The time between battery voltage checks in milliseconds. The battery level shouldn't change significantly, so sticking with 60000 (60 seconds) is usually fine.
- `R1`: The resistor divider R1 value in ohms. This is used to calculate the battery level.
- `R2`: The resistor divider R2 value in ohms. This is used to calculate the battery level.

### Pin Definitions
- `POT_X_PIN`: The analog pin for the X-axis potentiometer.
- `POT_Y_PIN`: The analog pin for the Y-axis potentiometer.
- `BTN_PIN`: The digital pin for the button.
- `BATTERY_PIN`: The analog pin for the battery voltage measurement.

### BLE Mouse Settings
- `BLE_CHECK_INTERVAL`: The time between BLE connection checks in milliseconds. Checking often helps the mouse become responsive sooner after connecting, but also adds load. 1000 (1 second) is usually fine for most people.
- `BLE_NAME`: The name of the BLE device. This will be the name that appears in the Bluetooth settings on your computer or mobile device. I recommend changing this if you have two fin mice so you will be able to differentiate them.
- `BLE_MANUFACTURER`: The manufacturer name of the BLE device. This will be the name that appears in the Bluetooth settings on your computer or mobile device.

## To-Do's
- Add wiring diagram
- Add graph of mouse movement settings
- Finish designing the battery circuitry
- Test battery level logic
- Add battery mounts in the CAD model
- Fine-tune the joystick cavity dimensions
- Build the right-side version in OnShape
- Add support in the code for scrolling and right-clicking
- Review BLE mouse library for any potential improvements from the [original repo's PRs](https://github.com/T-vK/ESP32-BLE-Mouse/pulls)