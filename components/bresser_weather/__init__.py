import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor, text_sensor
from esphome.const import (
    CONF_ID,
    CONF_TEMPERATURE,
    CONF_HUMIDITY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CELSIUS,
    UNIT_PERCENT,
)
from esphome.core import CORE

DEPENDENCIES = ["esp8266"]
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor"]

CONF_WIND_GUST = "wind_gust"
CONF_WIND_SPEED = "wind_speed"
CONF_WIND_DIRECTION = "wind_direction"
CONF_RAIN = "rain"
CONF_UV = "uv"
CONF_LIGHT = "light"
CONF_RSSI = "rssi"
CONF_BATTERY_OK = "battery_ok"
CONF_SENSOR_ID = "sensor_id"
CONF_FILTER_SENSOR_ID = "filter_sensor_id"

# Custom units not in const
UNIT_METER_PER_SECOND = "m/s"
UNIT_MILLIMETER = "mm"
UNIT_DEGREES = "°"
UNIT_KILOLUX = "klx"
UNIT_DBM = "dBm"

bresser_weather_ns = cg.esphome_ns.namespace("bresser_weather")
BresserWeatherComponent = bresser_weather_ns.class_("BresserWeatherComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(BresserWeatherComponent),
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_HUMIDITY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WIND_GUST): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WIND_SPEED): sensor.sensor_schema(
            unit_of_measurement=UNIT_METER_PER_SECOND,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_WIND_DIRECTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_DEGREES,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_RAIN): sensor.sensor_schema(
            unit_of_measurement=UNIT_MILLIMETER,
            accuracy_decimals=1,
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),
        cv.Optional(CONF_UV): sensor.sensor_schema(
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_LIGHT): sensor.sensor_schema(
            unit_of_measurement=UNIT_KILOLUX,
            accuracy_decimals=1,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_RSSI): sensor.sensor_schema(
            unit_of_measurement=UNIT_DBM,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_OK): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_BATTERY,
        ),
        cv.Optional(CONF_SENSOR_ID): text_sensor.text_sensor_schema(),
        cv.Optional(CONF_FILTER_SENSOR_ID): cv.hex_uint32_t,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    if CORE.is_esp8266:
        from esphome.components.esp8266.const import require_waveform

        require_waveform()
    
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature_sensor(sens))

    if CONF_HUMIDITY in config:
        sens = await sensor.new_sensor(config[CONF_HUMIDITY])
        cg.add(var.set_humidity_sensor(sens))

    if CONF_WIND_GUST in config:
        sens = await sensor.new_sensor(config[CONF_WIND_GUST])
        cg.add(var.set_wind_gust_sensor(sens))

    if CONF_WIND_SPEED in config:
        sens = await sensor.new_sensor(config[CONF_WIND_SPEED])
        cg.add(var.set_wind_speed_sensor(sens))

    if CONF_WIND_DIRECTION in config:
        sens = await sensor.new_sensor(config[CONF_WIND_DIRECTION])
        cg.add(var.set_wind_direction_sensor(sens))

    if CONF_RAIN in config:
        sens = await sensor.new_sensor(config[CONF_RAIN])
        cg.add(var.set_rain_sensor(sens))

    if CONF_UV in config:
        sens = await sensor.new_sensor(config[CONF_UV])
        cg.add(var.set_uv_sensor(sens))

    if CONF_LIGHT in config:
        sens = await sensor.new_sensor(config[CONF_LIGHT])
        cg.add(var.set_light_sensor(sens))

    if CONF_RSSI in config:
        sens = await sensor.new_sensor(config[CONF_RSSI])
        cg.add(var.set_rssi_sensor(sens))

    if CONF_BATTERY_OK in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_BATTERY_OK])
        cg.add(var.set_battery_sensor(sens))

    if CONF_SENSOR_ID in config:
        sens = await text_sensor.new_text_sensor(config[CONF_SENSOR_ID])
        cg.add(var.set_sensor_id_text_sensor(sens))

    if CONF_FILTER_SENSOR_ID in config:
        cg.add(var.set_filter_sensor_id(config[CONF_FILTER_SENSOR_ID]))

    # Add library dependencies
    cg.add_platformio_option("lib_deps", ["matthias-bs/BresserWeatherSensorReceiver@0.37.0"])
    cg.add_platformio_option("lib_deps", ["jgromes/RadioLib@7.4.0"])
    cg.add_platformio_option("lib_deps", ["vshymanskyy/Preferences@2.2.2"])
    cg.add_platformio_option("lib_deps", ["bblanchon/ArduinoJson@7.4.2"])
    
    # Add build flag to ensure library dependencies are found
    cg.add_build_flag("-DUSE_CC1101")
    cg.add_platformio_option("lib_ldf_mode", "deep+")
