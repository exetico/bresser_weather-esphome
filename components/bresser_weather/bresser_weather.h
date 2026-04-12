#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include <SPI.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "WeatherSensorCfg.h"
#include "WeatherSensor.h"

namespace esphome
{
    namespace bresser_weather
    {

        struct WeatherData
        {
            float temp_c;
            uint8_t humidity;
            float wind_gust;
            float wind_speed;
            float wind_direction;
            float rain_mm;
            float uv;
            float light_klx;
            float rssi;
            bool battery_ok;
            uint32_t sensor_id;
            uint8_t s_type;
            bool temp_ok;
            bool humidity_ok;
            bool wind_ok;
            bool rain_ok;
            bool uv_ok;
            bool light_ok;
            bool valid;
        };

        class BresserWeatherComponent : public Component
        {
        public:
            void setup() override;
            void loop() override;
            float get_setup_priority() const override { return setup_priority::DATA; }

            void set_temperature_sensor(sensor::Sensor *sensor) { temperature_sensor_ = sensor; }
            void set_humidity_sensor(sensor::Sensor *sensor) { humidity_sensor_ = sensor; }
            void set_wind_gust_sensor(sensor::Sensor *sensor) { wind_gust_sensor_ = sensor; }
            void set_wind_speed_sensor(sensor::Sensor *sensor) { wind_speed_sensor_ = sensor; }
            void set_wind_direction_sensor(sensor::Sensor *sensor) { wind_direction_sensor_ = sensor; }
            void set_rain_sensor(sensor::Sensor *sensor) { rain_sensor_ = sensor; }
            void set_uv_sensor(sensor::Sensor *sensor) { uv_sensor_ = sensor; }
            void set_light_sensor(sensor::Sensor *sensor) { light_sensor_ = sensor; }
            void set_rssi_sensor(sensor::Sensor *sensor) { rssi_sensor_ = sensor; }
            void set_battery_sensor(binary_sensor::BinarySensor *sensor) { battery_sensor_ = sensor; }
            void set_sensor_id_text_sensor(text_sensor::TextSensor *sensor) { sensor_id_sensor_ = sensor; }
            void set_filter_sensor_id(uint32_t filter_id)
            {
                filter_sensor_id_ = filter_id;
                filter_enabled_ = true;
            }
            void set_spi_clk(int pin) { spi_clk_ = pin; }
            void set_spi_miso(int pin) { spi_miso_ = pin; }
            void set_spi_mosi(int pin) { spi_mosi_ = pin; }

        protected:
            static void radio_task(void *parameter);

            WeatherSensor ws_;
            TaskHandle_t radio_task_handle_{nullptr};
            portMUX_TYPE data_mux_ = portMUX_INITIALIZER_UNLOCKED;
            WeatherData latest_data_{};
            volatile bool data_ready_{false};

            uint32_t filter_sensor_id_{0};
            bool filter_enabled_{false};
            int spi_clk_{-1};
            int spi_miso_{-1};
            int spi_mosi_{-1};

            sensor::Sensor *temperature_sensor_{nullptr};
            sensor::Sensor *humidity_sensor_{nullptr};
            sensor::Sensor *wind_gust_sensor_{nullptr};
            sensor::Sensor *wind_speed_sensor_{nullptr};
            sensor::Sensor *wind_direction_sensor_{nullptr};
            sensor::Sensor *rain_sensor_{nullptr};
            sensor::Sensor *uv_sensor_{nullptr};
            sensor::Sensor *light_sensor_{nullptr};
            sensor::Sensor *rssi_sensor_{nullptr};
            binary_sensor::BinarySensor *battery_sensor_{nullptr};
            text_sensor::TextSensor *sensor_id_sensor_{nullptr};
        };

    } // namespace bresser_weather
} // namespace esphome
