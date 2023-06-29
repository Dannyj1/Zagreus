#####################################################################
# Toggleable Flags
#####################################################################

# Toggleable flags allow you to enable or disable specific compiler
# flags based on your requirements. These flags can be individually
# enabled or disabled by setting the corresponding options.

# Option: ENABLE_OPTIMIZATION
# Description: Enables optimization flags (-O3).
# Default: OFF (Debug), ON (Release)

# Option: ENABLE_OPTIMIZATION_FAST_MATH
# Description: Enables fast math optimization flags (-Ofast).
# This option overrides ENABLE_OPTIMIZATION if enabled.
# Default: OFF

# Option: ENABLE_LTO
# Description: Enables Link Time Optimization (LTO).
# Default: OFF (Debug), ON (Release)

# Option: ENABLE_WARNINGS
# Description: Enables all compiler warnings.
# Default: ON (Debug), OFF (Release)

# Option: ENABLE_MARCH
# Description: Enables the -march flag.
# Default: OFF (Debug), ON (Release)

# Option: MARCH_VALUE
# Description: Sets the value for the -march flag if ENABLE_MARCH is enabled.
# Default: native

# Option: ENABLE_MTUNE
# Description: Enables the -mtune flag.
# Default: OFF (Debug), ON (Release)

# Option: MTUNE_VALUE
# Description: Sets the value for the -mtune flag if ENABLE_MTUNE is enabled.
# Default: native

# Option: ENABLE_SANITIZER
# Description: Enables undefined behavior sanitizer flags (-fsanitize=undefined -fsanitize-trap=all).
# Default: OFF

cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
project(Zagreus)

# Default values
set(BUILD_FLAGS "")

# Define the different profiles and their corresponding flags
set(DEBUG_FLAGS "-fno-omit-frame-pointer -pthread -gdwarf-4 -g")
set(PROFILING_FLAGS "-pthread -g -fno-inline")
set(RELEASE_FLAGS "-pthread")

# Define toggleable flags
option(ENABLE_OPTIMIZATION "Enable optimization flags (-O3)" ON)
option(ENABLE_OPTIMIZATION_FAST_MATH "Enable fast math optimization flags (-Ofast)" OFF)
option(ENABLE_LTO "Enable Link Time Optimization (LTO)" ON)
option(ENABLE_WARNINGS "Enable all compiler warnings" ON)
option(ENABLE_MARCH "Enable -march flag" ON)
option(ENABLE_SANITIZER "Enable undefined behavior sanitizer flags" OFF)
set(MARCH_VALUE "x86-64" CACHE STRING "Value for -march flag")
option(ENABLE_MTUNE "Enable -mtune flag" ON)
set(MTUNE_VALUE "native" CACHE STRING "Value for -mtune flag")

# If debug build type, disable optimization, lto, enable warnings, set march to native
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ENABLE_OPTIMIZATION OFF CACHE BOOL "Enable optimization flags (-O3)" FORCE)
    set(ENABLE_LTO OFF CACHE BOOL "Enable Link Time Optimization (LTO)" FORCE)
    set(ENABLE_WARNINGS ON CACHE BOOL "Enable all compiler warnings" FORCE)
    set(ENABLE_MARCH OFF CACHE BOOL "Enable -march flag" FORCE)
    set(MARCH_VALUE "" CACHE STRING "Value for -march flag" FORCE)
    set(ENABLE_MTUNE OFF CACHE BOOL "Enable -mtune flag" FORCE)
    set(MTUNE_VALUE "" CACHE STRING "Value for -mtune flag" FORCE)
endif()

# If release build type, enable optimization, lto, disable warnings
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(ENABLE_OPTIMIZATION ON CACHE BOOL "Enable optimization flags (-O3)" FORCE)
    set(ENABLE_LTO ON CACHE BOOL "Enable Link Time Optimization (LTO)" FORCE)
    set(ENABLE_WARNINGS OFF CACHE BOOL "Enable all compiler warnings" FORCE)
    set(ENABLE_MARCH ON CACHE BOOL "Enable -march flag" FORCE)
    set(MARCH_VALUE "x86-64" CACHE STRING "Value for -march flag" FORCE)
    set(ENABLE_MTUNE ON CACHE BOOL "Enable -mtune flag" FORCE)
    set(MTUNE_VALUE "native" CACHE STRING "Value for -mtune flag" FORCE)
endif()

# Apply toggleable flags to profiles
if (ENABLE_OPTIMIZATION)
    if (ENABLE_OPTIMIZATION_FAST_MATH)
        set(RELEASE_FLAGS "${RELEASE_FLAGS} -Ofast")
        set(DEBUG_FLAGS "${DEBUG_FLAGS} -Ofast")
        set(PROFILING_FLAGS "${PROFILING_FLAGS} -Ofast")
    else()
        set(RELEASE_FLAGS "${RELEASE_FLAGS} -O3")
        set(DEBUG_FLAGS "${DEBUG_FLAGS} -O3")
        set(PROFILING_FLAGS "${PROFILING_FLAGS} -O3")
    endif()
else()
    set(RELEASE_FLAGS "${RELEASE_FLAGS} -O0")
    set(DEBUG_FLAGS "${DEBUG_FLAGS} -O0")
    set(PROFILING_FLAGS "${PROFILING_FLAGS} -O0")
endif()

if (ENABLE_LTO)
    set(RELEASE_FLAGS "${RELEASE_FLAGS} -flto")
endif()

if (ENABLE_WARNINGS)
    set(DEBUG_FLAGS "${DEBUG_FLAGS} -Wall -Wextra")
    set(PROFILING_FLAGS "${PROFILING_FLAGS} -Wall -Wextra")
    set(RELEASE_FLAGS "${RELEASE_FLAGS} -Wall -Wextra")
endif()

if (ENABLE_MARCH)
    set(MARCH_FLAG "-march=${MARCH_VALUE}")
endif()

if (ENABLE_MTUNE)
    set(MTUNE_FLAG "-mtune=${MTUNE_VALUE}")
endif()

if (ENABLE_SANITIZER)
    set(DEBUG_FLAGS "${DEBUG_FLAGS} -fsanitize=undefined -fsanitize-trap=all")
    set(PROFILING_FLAGS "${PROFILING_FLAGS} -fsanitize=undefined -fsanitize-trap=all")
    set(RELEASE_FLAGS "${RELEASE_FLAGS} -fsanitize=undefined -fsanitize-trap=all")
endif()

# Construct the final flags based on the selected profile and toggleable flags
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_FLAGS "${DEBUG_FLAGS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(BUILD_FLAGS "${PROFILING_FLAGS}")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_FLAGS "${RELEASE_FLAGS}")
endif()

# Add the march and mtune flags
if (ENABLE_MARCH)
    set(BUILD_FLAGS "${BUILD_FLAGS} ${MARCH_FLAG}")
    if (ENABLE_MTUNE)
        set(BUILD_FLAGS "${BUILD_FLAGS} ${MTUNE_FLAG}")
    endif()
endif()

# Set the flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BUILD_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BUILD_FLAGS}")

file(GLOB inc_zagreus "src/*.h" "src/*.cpp")
file(GLOB inc_senjo "senjo/*.h" "senjo/*.cpp")

add_executable(Zagreus src/main.cpp ${inc_senjo} ${inc_zagreus})
