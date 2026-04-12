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

# With lib_ldf_mode=off, PlatformIO doesn't auto-detect inter-library
# dependencies. Add include paths so libraries can find each other's headers.
extra_paths = []

# Arduino framework libraries (SPI, Preferences)
try:
    framework_dir = env.PioPlatform().get_package_dir("framework-arduinoespressif32")
    for lib_name in ["SPI", "Preferences"]:
        lib_src = os.path.join(framework_dir, "libraries", lib_name, "src")
        if os.path.isdir(lib_src):
            extra_paths.append(lib_src)
except Exception:
    pass

# PlatformIO-managed library include paths (for cross-library includes)
# Only add versioned libraries (with @) to avoid duplicate/conflicting headers
# from unversioned copies that BresserWeatherSensorReceiver may pull in
libdeps_dir = os.path.join(env.get("PROJECT_LIBDEPS_DIR"), env.get("PIOENV"))
if os.path.isdir(libdeps_dir):
    for lib_dir in os.listdir(libdeps_dir):
        # Skip unversioned copies if a versioned one exists (avoid header conflicts)
        if "@" not in lib_dir:
            versioned = [d for d in os.listdir(libdeps_dir)
                         if d.startswith(lib_dir + "@")]
            if versioned:
                continue
        lib_src = os.path.join(libdeps_dir, lib_dir, "src")
        if os.path.isdir(lib_src):
            extra_paths.append(lib_src)

if extra_paths:
    print(f"BRESSER WEATHER: Adding {len(extra_paths)} include paths for library cross-references")
    env.Append(CPPPATH=extra_paths)
    for lb in env.GetLibBuilders():
        lb.env.Append(CPPPATH=extra_paths)


def middleware(node):
    path = node.get_path()

    if "BresserWeatherSensorReceiver" in path:
        env.AppendUnique(CPPDEFINES=[("FORCE_REBUILD", 1)])
    return node

env.AddBuildMiddleware(middleware)
