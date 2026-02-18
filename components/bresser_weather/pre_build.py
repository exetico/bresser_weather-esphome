import shutil
import os
from SCons.Script import Import

Import("env")

dest = os.path.join(env.get("PROJECT_LIBDEPS_DIR"), env.get("PIOENV"), "BresserWeatherSensorReceiver", "src", "WeatherSensorCfg.h")
src = os.path.join(env.get("PROJECT_SRC_DIR"),"esphome","components","bresser_weather", "WeatherSensorCfg.h")

if not os.path.exists(src):
    raise FileNotFoundError(f"BRESSER WEATHER: WeatherSensorCfg.h not found at: {src}")

print("BRESSER WEATHER: Overriding WeatherSensorCfg.h in:", dest)
shutil.copy(src, dest)


def middleware(node):
    path = node.get_path()

    if "BresserWeatherSensorReceiver" in path:
        env.AppendUnique(CPPDEFINES=[("FORCE_REBUILD", 1)])
    return node

env.AddBuildMiddleware(middleware)