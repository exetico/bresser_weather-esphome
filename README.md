# ESPHome Bresser Weather Sensor Component

Minimal ESPHome component for Bresser weather sensors with CC1101 receiver.

This is a side-code project made possible entirely by the excellent [BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver/tree/main) library by Matthias Prinke. All credit for the sensor decoding and radio communication goes to that project.

> **Disclaimer**: This project was **developed with AI assistance**, but has been tested with the hardware configuration listed below.

## Tested Hardware

This component has been tested **only** with:

- **Microcontroller**: Wemos D1 Mini (ESP8266)
- **Radio Module**: CC1101 868MHz
- **Weather Sensor**: Bresser 7-in-1 (Model 7003100)

Other combinations may work but are untested.

## Features

- Direct Home Assistant integration via ESPHome API
- Sensor readings: Temperature, Humidity, Wind (speed/gust/direction), Rain, UV, Light
- Low memory footprint - fits on ESP8266

## Potentially Compatible Hardware

The underlying library supports **various configurations**. Untested combinations include:

### Weather Sensors

- Bresser 5-in-1
- Bresser 6-in-1
- Other Bresser models

### Radio Modules

- SX1276/RFM95W
- SX1262
- LR1121
- CC1101

### Microcontrollers

- ESP32 variants
- Other ESP8266 boards

## Wiring Example

**D1 Mini + CC1101:**

| CC1101 | D1 Mini | GPIO |
| ------ | ------- | ---- |
| VCC    | 3.3V    | -    |
| GND    | GND     | -    |
| SCLK   | D5      | 14   |
| MISO   | D6      | 12   |
| MOSI   | D7      | 13   |
| CS     | D8      | 15   |
| GD0    | D2      | 4    |
| GD2    | D1      | 5    |

⚠️ **Important**: CC1101 requires **3.3V** not 5V!

## Installation

### Option 1: Use from GitHub (Recommended)

```yaml
external_components:
  - source: github://exetico/bresser_weather-esphome@main
    components: [bresser_weather]
```

### Option 2: Local Development

```yaml
external_components:
  - source:
      type: local
      path: components
```

## Configuration

### Minimal Example

```yaml
substitutions:
  device_name: bresser-weather
  friendly_name: Bresser Weather

esphome:
  name: ${device_name}

esp8266:
  board: d1_mini

# Required: SPI configuration for radio module
spi:
  clk_pin: GPIO14 # D5
  miso_pin: GPIO12 # D6
  mosi_pin: GPIO13 # D7

# External component
external_components:
  - source: github://exetico/bresser_weather-esphome@main
    components: [bresser_weather]

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

api:
logger:
ota:

# Bresser Weather Component
bresser_weather:
  filter_sensor_id: 0x00001F11 # Only accept data from this sensor ID
  temperature:
    name: "${friendly_name} Temperature"
  humidity:
    name: "${friendly_name} Humidity"
  wind_gust:
    name: "${friendly_name} Wind Gust"
  wind_speed:
    name: "${friendly_name} Wind Speed"
  wind_direction:
    name: "${friendly_name} Wind Direction"
  rain:
    name: "${friendly_name} Rain"
  uv:
    name: "${friendly_name} UV Index"
  light:
    name: "${friendly_name} Light"
  rssi:
    name: "${friendly_name} RSSI"
  battery_ok:
    name: "${friendly_name} Battery OK"
  sensor_id:
    name: "${friendly_name} Sensor ID"
```

### Complete Examples

- [example.yaml](example.yaml) - Full configuration using GitHub source (after publishing)
- [local_development.yaml](local_development.yaml) - Local development configuration

## Home Assistant Integration

Once configured and uploaded, the device will automatically appear in Home Assistant via the ESPHome integration. All sensors will be created with proper device classes for optimal dashboard display.

### Available Sensors

| Sensor         | Unit | Device Class    |
| -------------- | ---- | --------------- |
| Temperature    | °C   | temperature     |
| Humidity       | %    | humidity        |
| Wind Gust      | m/s  | wind_speed      |
| Wind Speed     | m/s  | wind_speed      |
| Wind Direction | °    | -               |
| Rain           | mm   | precipitation   |
| UV Index       | -    | -               |
| Light          | klx  | illuminance     |
| RSSI           | dBm  | signal_strength |
| Battery OK     | -    | battery         |
| Sensor ID      | -    | diagnostic      |

## Data Update Frequency

The Bresser 7-in-1 sensor transmits approximately every 48 seconds. The component listens continuously and updates Home Assistant immediately when new data is received.

## Troubleshooting

### No sensor data received

1. Check wiring - especially 3.3V power and all SPI connections
2. Verify radio module - CC1101 LED should blink occasionally
3. Check sensor battery - replace if low
4. Enable DEBUG logging:
   ```yaml
   logger:
     level: DEBUG
   ```
5. Check sensor range - typically ~100m outdoors

### Local Development Setup

For local development and testing:

```bash
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

Then use [local_development.yaml](local_development.yaml) as your configuration file.

#### ESPHome Commands for local development

```bash
# Validate config
esphome config bresser_weather.yaml

# Compile (without uploading)
esphome compile bresser_weather.yaml

# Upload to ESP8266 (finds USB port automatically)
esphome upload bresser_weather.yaml

# Or upload + monitor logs
esphome run bresser_weather.yaml
```

### Memory issues

Disable the web server to save memory:

```yaml
# web_server:  # Remove this
#   port: 80
```

## Credits

This component is only possible thanks to the [BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver/tree/main) library by Matthias Prinke. All the heavy lifting of sensor decoding and radio communication is done by that library.

## License

MIT License - see [LICENSE](LICENSE) file for details.
