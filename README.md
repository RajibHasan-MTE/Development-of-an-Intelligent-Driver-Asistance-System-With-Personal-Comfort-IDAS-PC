# Development of an Intelligent Driver Assistance System With Personal Comfort

## Introduction
This project focuses on developing an **Intelligent Driver Assistance System (IDAS)** that enhances driver safety and personal comfort by using smart embedded systems, sensor fusion, and computer vision.  
The system monitors the driver's condition and adjusts the vehicle environment automatically based on real-time data.

## Hardware Components
- **ESP32 DevKit** — Main microcontroller
- **AMG8833 Thermal Camera** — Human body temperature detection and presence monitoring
- **0.96" OLED Display** — Real-time status and warning display
- **1.8" TFT Display (128x160 resolution)** — Sensor data and visual feedback
- **MQ-3 Alcohol Sensor** — Alcohol level detection
- **DHT-11 Temperature and Humidity Sensor** — Cabin environment monitoring
- **Camera + SerialPy (Python)** — Eye-blink detection using computer vision
- **(Optional) AC/Relay Control Module** — Automatic AC control based on detected body temperature

## Key Features
- Eye-blink detection for driver drowsiness monitoring (Python and SerialPy)
- Alcohol detection alert system
- Thermal-based driver presence and health monitoring
- Automatic Air Conditioning (AC) control based on body temperature
- Cabin temperature and humidity monitoring
- Real-time feedback through OLED and TFT displays
- ESP32-based data processing and control

## Technologies Used
- Embedded C/C++ programming for ESP32
- Arduino IDE or PlatformIO for firmware development
- Python programming with SerialPy for real-time eye-blink detection
- Sensor integration and smart decision making

## Project Structure
- `/firmware/` — ESP32 firmware source code
- `/python/` — Python scripts for eye-blink detection and serial communication
- `/hardware/` — Circuit schematics and hardware design files
- `/docs/` — Project documentation and research materials

## System Overview
The IDAS system uses the thermal camera to detect human body temperature and dynamically control AC settings for optimal comfort.  
It also uses an alcohol sensor to detect driver intoxication, an environmental sensor to monitor cabin conditions, and a camera to track the driver's eye blinks to detect drowsiness.  
All critical information and warnings are displayed in real-time through OLED and TFT screens.

## How to Use
1. Connect all sensors and displays to the ESP32 DevKit according to the wiring diagram.
2. Upload the ESP32 firmware found in `/firmware/`.
3. Start the Python script in `/python/` for real-time eye-blink detection.
4. Monitor the driver's status and environmental conditions through the displays.
5. AC is automatically adjusted based on the body temperature detected by the thermal camera.

## About This Research
This project is part of a research study titled:  
**"Development of an Intelligent Driver Assistance System With Personal Comfort."**

## License
This project is licensed under the [MIT License](LICENSE).

## Author
- [Your Name]
- [GitHub Profile or Contact Information] (optional)

## Citation
If you reference or use this project, please cite:
