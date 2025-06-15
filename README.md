# Arduino Nano ESP32 RGB Color Picker

A simple color picker for controlling the RGB LED on ESP32 boards.  
Tested on the **Arduino Nano ESP32**, this project lets you pick a color from a web interface and display it on the onboard or external RGB LED.

## Features

- Web-based color picker
- Live RGB LED updates
- Access Point (AP) mode (no router needed)
- Works with other ESP32 boards too

## Getting Started

### Prerequisites

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- ESP32 board (Arduino Nano ESP32 or others)

### Steps

1. **Install PlatformIO Extension**
   - Open VS Code and install the PlatformIO extension from the Extensions Marketplace.

2. **Clone the Repository**

   ```bash
   git clone https://github.com/kumardeo/arduino-nano-esp32-rgb-color-picker.git
   cd arduino-nano-esp32-rgb-color-picker
   ```

3. **(Optional) Configure LED Pins**

   If you're using a board that doesn't have predefined RGB LED pins (i.e. `LED_RED`, `LED_GREEN` and `LED_BLUE`), define the GPIOs in `main.cpp`:

   ```cpp
   #ifndef ARDUINO_NANO_ESP32
   static constexpr uint8_t LED_BUILTIN = 2;
   static constexpr uint8_t LED_RED = 46;
   static constexpr uint8_t LED_GREEN = 0;
   static constexpr uint8_t LED_BLUE = 45;
   #endif
   ```
   
   For the Arduino Nano ESP32, these are predefined and no changes are needed.

4. **Connect and Flash the Board**
   - Plug your ESP32 board into your computer.
   - Open the project in VS Code.
   - Use the "PlatformIO: Upload" button (or Ctrl+Alt+U) to upload the code.

5. **Start the Board**
   - After uploading, the board will start in Access Point mode.

6. **Connect to the ESP32 Wi-Fi**
   - On your device, find and connect to the Wi-Fi network named something like: `ESP32-RGB`.
   - Use the password provided in the code.

7. **Access the Web Interface**

   Open your browser and go to:

   ```
   http://192.168.4.1
   ```

   You should now see the color picker. Try changing the color — the RGB LED will update instantly!

## License

[MIT License](./LICENSE.md)

---

Made with ❤️ while learning. Contributions welcome!
