# Set the minimum required version of CMake for this project.
cmake_minimum_required(VERSION 3.12)

# Define the project name and specify the languages it uses (C++ and C).
project(MyMqttProject LANGUAGES CXX C)

# Set the C++ standard to C++17 and enforce it.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Configure and include the MQTT-C subproject.
# These options are specific to the MQTT-C library's own CMakeLists.txt.
# We set them as CACHE variables with FORCE to ensure our settings override their defaults.
set(MQTT_C_EXAMPLES OFF CACHE BOOL "Disable MQTT-C examples" FORCE)
set(MQTT_C_TESTS OFF CACHE BOOL "Disable MQTT-C tests" FORCE)

# Include the MQTT-C project from the third_party directory. This makes its
# targets (like the 'mqttc' library) available to our project.
add_subdirectory(third_party/MQTT-C)


# 2. Create our platform abstraction library.
# This library encapsulates all platform-specific networking code and the
# implementation of the MQTT-C Platform Abstraction Layer (PAL).
add_library(mqtt_platform)

# Add the platform-independent source files to the library.
# These are PRIVATE because they are part of the library's implementation.
target_sources(mqtt_platform PRIVATE src/platform/mqtt_network.cpp)

# Expose the 'src' directory as a PUBLIC include path. This allows client code
# (like our executables) to find the library's headers, e.g., #include "platform/mqtt_network.h".
target_include_directories(mqtt_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Because the public header of mqtt_platform (mqtt_network.h) includes a header
# from mqttc (mqtt_pal.h), this link dependency must be PUBLIC.
# This ensures that any target linking against mqtt_platform (like our 'publisher'
# executable) will also get the include directories and link dependencies of 'mqttc'.
target_link_libraries(mqtt_platform PUBLIC mqttc)


# 3. Add platform-specific source files and dependencies.
if(UNIX AND NOT APPLE) # Linux
    message(STATUS "Configuring for Linux")
    # Add the POSIX-specific source files for the PAL implementation.
    target_sources(mqtt_platform PRIVATE
        src/platform/detail/network_posix.cpp
        src/platform/pal_posix.cpp
    )

    # Find the standard threads library.
    find_package(Threads REQUIRED)

    # The threads library is an internal implementation detail of mqtt_platform,
    # so we link it as PRIVATE. Consumers of mqtt_platform don't need to link
    # against pthreads directly.
    target_link_libraries(mqtt_platform PRIVATE Threads::Threads)
else()
    message(FATAL_ERROR "Unsupported platform for mqtt_platform library")
endif()


# 4. Define the executables.
add_executable(publisher src/publisher.cpp)
add_executable(subscriber src/subscriber.cpp)


# 5. Link the executables against our platform library.
# The 'publisher' and 'subscriber' executables themselves don't export the
# mqtt_platform API further, so using PRIVATE is correct here.
# CMake will automatically handle the transitive PUBLIC dependency from
# mqtt_platform (i.e., it will correctly link the executables against 'mqttc' as well).
target_link_libraries(publisher PRIVATE mqtt_platform)
target_link_libraries(subscriber PRIVATE mqtt_platform)