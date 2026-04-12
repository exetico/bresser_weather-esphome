#include "bresser_weather.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace bresser_weather
    {

        static const char *const TAG = "bresser_weather";

        void BresserWeatherComponent::setup()
        {
            ESP_LOGI(TAG, "Setting up Bresser Weather Sensor Receiver");
            if (this->spi_clk_ >= 0 && this->spi_miso_ >= 0 && this->spi_mosi_ >= 0) {
                ESP_LOGI(TAG, "Initializing SPI: CLK=%d, MISO=%d, MOSI=%d",
                         this->spi_clk_, this->spi_miso_, this->spi_mosi_);
                SPI.begin(this->spi_clk_, this->spi_miso_, this->spi_mosi_, -1);
            }
            this->ws_.begin();
            ESP_LOGI(TAG, "Receiver initialized successfully");

            xTaskCreatePinnedToCore(
                radio_task,
                "bresser_radio",
                8192,
                this,
                1,
                &this->radio_task_handle_,
                1  // core 1 — ESPHome/WiFi runs on core 0
            );
            ESP_LOGI(TAG, "Radio task started on core 1");
        }

        // Runs on core 1: receives radio data and buffers it for the main loop.
        void BresserWeatherComponent::radio_task(void *parameter)
        {
            BresserWeatherComponent *self = static_cast<BresserWeatherComponent *>(parameter);

            ESP_LOGI(TAG, "Radio task running on core %d", xPortGetCoreID());

            while (true) {
                self->ws_.clearSlots();
                int decode_status = self->ws_.getMessage();

                if (decode_status == DECODE_OK &&
                    self->ws_.sensor.size() > 0 &&
                    self->ws_.sensor[0].valid)
                {
                    const auto &s = self->ws_.sensor[0];

                    // Only accept weather sensor types
                    if (s.s_type == SENSOR_TYPE_WEATHER0 ||
                        s.s_type == SENSOR_TYPE_WEATHER1 ||
                        s.s_type == SENSOR_TYPE_WEATHER3 ||
                        s.s_type == SENSOR_TYPE_WEATHER8)
                    {
                        // Check sensor ID filter
                        if (self->filter_enabled_ && s.sensor_id != self->filter_sensor_id_) {
                            ESP_LOGD(TAG, "Ignoring sensor ID %08X (filter: %08X)",
                                     (unsigned int)s.sensor_id,
                                     (unsigned int)self->filter_sensor_id_);
                        } else {
                            // Copy decoded data into shared buffer under spinlock
                            WeatherData d;
                            d.temp_c = s.w.temp_c;
                            d.humidity = s.w.humidity;
                            d.wind_gust = s.w.wind_gust_meter_sec;
                            d.wind_speed = s.w.wind_avg_meter_sec;
                            d.wind_direction = s.w.wind_direction_deg;
                            d.rain_mm = s.w.rain_mm;
                            d.uv = s.w.uv;
                            d.light_klx = s.w.light_klx;
                            d.rssi = s.rssi;
                            d.battery_ok = s.battery_ok;
                            d.sensor_id = s.sensor_id;
                            d.s_type = s.s_type;
                            d.temp_ok = s.w.temp_ok;
                            d.humidity_ok = s.w.humidity_ok;
                            d.wind_ok = s.w.wind_ok;
                            d.rain_ok = s.w.rain_ok;
                            d.uv_ok = s.w.uv_ok;
                            d.light_ok = s.w.light_ok;
                            d.valid = true;

                            portENTER_CRITICAL(&self->data_mux_);
                            self->latest_data_ = d;
                            self->data_ready_ = true;
                            portEXIT_CRITICAL(&self->data_mux_);
                        }
                    }
                }

                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        // Runs on core 0 (main ESPHome loop): publishes buffered data to HA.
        void BresserWeatherComponent::loop()
        {
            if (!this->data_ready_)
                return;

            // Grab the buffered data under spinlock
            WeatherData d;
            portENTER_CRITICAL(&this->data_mux_);
            d = this->latest_data_;
            this->data_ready_ = false;
            portEXIT_CRITICAL(&this->data_mux_);

            if (!d.valid)
                return;

            // All publish_state() calls happen on core 0 — thread-safe
            if (this->sensor_id_sensor_ != nullptr) {
                char id_str[16];
                snprintf(id_str, sizeof(id_str), "%08X", (unsigned int)d.sensor_id);
                this->sensor_id_sensor_->publish_state(id_str);
            }

            if (this->rssi_sensor_ != nullptr)
                this->rssi_sensor_->publish_state(d.rssi);

            // HA device_class BATTERY: ON = Low, OFF = OK
            if (this->battery_sensor_ != nullptr)
                this->battery_sensor_->publish_state(!d.battery_ok);

            if (d.temp_ok && this->temperature_sensor_ != nullptr)
                this->temperature_sensor_->publish_state(d.temp_c);

            if (d.humidity_ok && this->humidity_sensor_ != nullptr)
                this->humidity_sensor_->publish_state(d.humidity);

            if (d.wind_ok) {
                if (this->wind_gust_sensor_ != nullptr)
                    this->wind_gust_sensor_->publish_state(d.wind_gust);
                if (this->wind_speed_sensor_ != nullptr)
                    this->wind_speed_sensor_->publish_state(d.wind_speed);
                if (this->wind_direction_sensor_ != nullptr)
                    this->wind_direction_sensor_->publish_state(d.wind_direction);
            }

            if (d.rain_ok && this->rain_sensor_ != nullptr)
                this->rain_sensor_->publish_state(d.rain_mm);

            if (d.uv_ok && this->uv_sensor_ != nullptr)
                this->uv_sensor_->publish_state(d.uv);

            if (d.light_ok && this->light_sensor_ != nullptr)
                this->light_sensor_->publish_state(d.light_klx);

            ESP_LOGD(TAG, "Data published: Temp=%.1f°C, Hum=%d%%, Wind=%.1f/%.1f m/s @ %.0f°, Rain=%.1fmm, UV=%.1f, Light=%.1fklx, RSSI=%.1fdBm, Battery=%s",
                     d.temp_c, d.humidity, d.wind_speed, d.wind_gust,
                     d.wind_direction, d.rain_mm, d.uv, d.light_klx,
                     d.rssi, d.battery_ok ? "OK" : "Low");
        }

    } // namespace bresser_weather
} // namespace esphome
