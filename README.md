# TV Extension Device Firmware

## Overview
This repository contains the firmware for the TV Extension Device, a project designed to automate the operation of a mounted TV system. The firmware is written in C and developed using Microchip Studio for the Atmega328 microcontroller. It ensures precise control of the device's motors, sensors, and Bluetooth communication to deliver a seamless user experience.

### Directory Structure
- **`/Atmega328 specific`**: Contains Atmega328-specific header files required for low-level hardware configuration.
- **`/Device Property Control`**: Includes headers that define the properties of the TV Extension Device, enabling compatibility with the remote Bluetooth app.
- **`/HM-19`**: Houses files specific to the HM-19 Bluetooth module, facilitating wireless communication.
- **`/General`**: Contains shared code files used across multiple modules, such as utility functions and macros.
- **`/Movement Control`**: Focused on controlling the movement mechanisms of the device, including the sliding doors, linear drive, and tilt functionality.
- **Main Directory**:
  - **`main.c`**: The primary entry point of the firmware, integrating all modules.
  - **`TEUC_PPDef.h and TEUC.h `**: Configurations for the board's pin and port assignments.

## Building the Project
To build and deploy the firmware, follow these steps:

1. **Set Up Microchip Studio**
   - Ensure Microchip Studio is installed on your development machine. The latest version can be downloaded from the [Microchip Studio website](https://www.microchip.com/).

2. **Clone the Repository**
   ```bash
   git clone https://github.com/LaRoomy/TV_Extend_Unit_Control.git
   ```

3. **Open the Project**
   - Launch Microchip Studio.
   - Open the `tv-extension-device.atsln` solution file in the main directory.

4. **Configure the Project**
   - Verify that the target device is set to Atmega328.
   - Check the pin and port definitions in the main directory to ensure they match your hardware setup.

5. **Build the Firmware**
   - In Microchip Studio, click `Build -> Build Solution` or press `F7` to compile the project.

6. **Flash the Firmware**
   - Connect your Atmega328-based board to your development machine.
   - Use a compatible programmer (e.g., AVRISP mkII) to upload the compiled firmware to the device.

## Contribution
This repository is a showcase project and is not open for contributions. For any questions or feedback, please contact the repository owner.

## License
This project is provided for educational and demonstrational purposes and is licensed under the MIT license.

---

For additional details or to explore the code, refer to the specific files and modules in this repository.
