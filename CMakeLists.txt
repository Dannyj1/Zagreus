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
# Default: OFF (Debug), ON (Release)

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

# Option: APPEND_VERSION
# Description: Appends the version number to the executable filename.
# Default: OFF

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(Zagreus)
set(ZAGREUS_VERSION "v2.4.0")

# Default values
set(BUILD_FLAGS "")

# Define the different profiles and their corresponding flags
set(DEBUG_FLAGS "-static-libgcc -static-libstdc++ -fno-omit-frame-pointer -pthread -gdwarf-4 -g -Wno-unused-command-line-argument")
set(PROFILING_FLAGS "-static-libgcc -static-libstdc++ -pthread -g -Wno-unused-command-line-argument")
set(RELEASE_FLAGS "-static-libgcc -static-libstdc++ -pthread -Wno-unused-command-line-argument")

if(WIN32)
    # Add the --pdb flag for Windows
    set(DEBUG_FLAGS "${DEBUG_FLAGS} -gcodeview -Wl,--pdb=Zagreus.pdb")
    set(PROFILING_FLAGS "${PROFILING_FLAGS} -gcodeview -Wl,--pdb=Zagreus.pdb")
endif()

# Define toggleable flags
option(ENABLE_OPTIMIZATION "Enable optimization flags (-O3)" ON)
option(ENABLE_OPTIMIZATION_FAST_MATH "Enable fast math optimization flags (-Ofast)" ON)
option(ENABLE_LTO "Enable Link Time Optimization (LTO)" ON)
option(ENABLE_WARNINGS "Enable all compiler warnings" ON)
option(ENABLE_MARCH "Enable -march flag" ON)
option(ENABLE_SANITIZER "Enable undefined behavior sanitizer flags" OFF)
set(MARCH_VALUE "x86-64" CACHE STRING "Value for -march flag")
option(ENABLE_MTUNE "Enable -mtune flag" ON)
set(MTUNE_VALUE "native" CACHE STRING "Value for -mtune flag")
option(APPEND_VERSION "Append version to filename" OFF)

# Profile defaults
# If debug build type and ENABLE_OPTIMIZATION is not defined, set optimization to OFF
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DEFINED ENABLE_OPTIMIZATION)
    set(ENABLE_OPTIMIZATION OFF CACHE BOOL "Enable optimization flags (-O3)" FORCE)
    set(ENABLE_OPTIMIZATION_FAST_MATH OFF CACHE BOOL "Enable fast math optimization flags (-Ofast)" FORCE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DEFINED ENABLE_OPTIMIZATION_FAST_MATH)
    set(ENABLE_OPTIMIZATION_FAST_MATH OFF CACHE BOOL "Enable fast math optimization flags (-Ofast)" FORCE)
endif()

# If debug build type and ENABLE_LTO is not defined, set LTO to OFF
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DEFINED ENABLE_LTO)
    set(ENABLE_LTO OFF CACHE BOOL "Enable Link Time Optimization (LTO)" FORCE)
endif()

# If debug build type and ENABLE_WARNINGS is not defined, set warnings to ON
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DEFINED ENABLE_WARNINGS)
    set(ENABLE_WARNINGS ON CACHE BOOL "Enable all compiler warnings" FORCE)
endif()

# If debug build type and ENABLE_MARCH is not defined, set march to OFF
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DEFINED ENABLE_MARCH)
    set(ENABLE_MARCH OFF CACHE BOOL "Enable -march flag" FORCE)
endif()

# If debug build type and MARCH_VALUE is not defined, set march value to an empty string
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DEFINED MARCH_VALUE)
    set(MARCH_VALUE "" CACHE STRING "Value for -march flag" FORCE)
endif()

# If debug build type and ENABLE_MTUNE is not defined, set mtune to OFF
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DEFINED ENABLE_MTUNE)
    set(ENABLE_MTUNE OFF CACHE BOOL "Enable -mtune flag" FORCE)
endif()

# If debug build type and MTUNE_VALUE is not defined, set mtune value to an empty string
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT DEFINED MTUNE_VALUE)
    set(MTUNE_VALUE "" CACHE STRING "Value for -mtune flag" FORCE)
endif()

# If release build type and ENABLE_OPTIMIZATION is not defined, set optimization to ON
if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND NOT DEFINED ENABLE_OPTIMIZATION)
    set(ENABLE_OPTIMIZATION ON CACHE BOOL "Enable optimization flags (-O3)" FORCE)
endif()

# If release build type and ENABLE_OPTIMIZATION_FAST_MATH is not defined, set optimization to ON
if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND NOT DEFINED ENABLE_OPTIMIZATION_FAST_MATH)
    set(ENABLE_OPTIMIZATION_FAST_MATH ON CACHE BOOL "Enable fast math optimization flags (-Ofast)" FORCE)
endif()

# If release build type and ENABLE_LTO is not defined, set LTO to ON
if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND NOT DEFINED ENABLE_LTO)
    set(ENABLE_LTO ON CACHE BOOL "Enable Link Time Optimization (LTO)" FORCE)
endif()

# If release build type and ENABLE_WARNINGS is not defined, set warnings to OFF
if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND NOT DEFINED ENABLE_WARNINGS)
    set(ENABLE_WARNINGS OFF CACHE BOOL "Enable all compiler warnings" FORCE)
endif()

# If release build type and ENABLE_MARCH is not defined, set march to ON
if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND NOT DEFINED ENABLE_MARCH)
    set(ENABLE_MARCH ON CACHE BOOL "Enable -march flag" FORCE)
endif()

# If release build type and MARCH_VALUE is not defined, set march value to "x86-64"
if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND NOT DEFINED MARCH_VALUE)
    set(MARCH_VALUE "x86-64" CACHE STRING "Value for -march flag" FORCE)
endif()

# If release build type and ENABLE_MTUNE is not defined, set mtune to ON
if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND NOT DEFINED ENABLE_MTUNE)
    set(ENABLE_MTUNE ON CACHE BOOL "Enable -mtune flag" FORCE)
endif()

# If release build type and MTUNE_VALUE is not defined, set mtune value to "native"
if ((CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") AND NOT DEFINED MTUNE_VALUE)
    set(MTUNE_VALUE "native" CACHE STRING "Value for -mtune flag" FORCE)
endif()

message("ENABLE_OPTIMIZATION: ${ENABLE_OPTIMIZATION}")
message("ENABLE_OPTIMIZATION_FAST_MATH: ${ENABLE_OPTIMIZATION_FAST_MATH}")
message("ENABLE_LTO: ${ENABLE_LTO}")
message("ENABLE_WARNINGS: ${ENABLE_WARNINGS}")
message("ENABLE_MARCH: ${ENABLE_MARCH}")
message("ENABLE_SANITIZER: ${ENABLE_SANITIZER}")
message("MARCH_VALUE: ${MARCH_VALUE}")
message("ENABLE_MTUNE: ${ENABLE_MTUNE}")
message("MTUNE_VALUE: ${MTUNE_VALUE}")

if (APPEND_VERSION)
    if (ENABLE_MARCH)
        set(CMAKE_EXECUTABLE_SUFFIX "-${MARCH_VALUE}${CMAKE_EXECUTABLE_SUFFIX}")
    endif()

    set(CMAKE_EXECUTABLE_SUFFIX "-${CMAKE_SYSTEM_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
    set(CMAKE_EXECUTABLE_SUFFIX "-${ZAGREUS_VERSION}${CMAKE_EXECUTABLE_SUFFIX}")
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

target_compile_definitions(Zagreus PRIVATE ZAGREUS_VERSION="${ZAGREUS_VERSION}")