cmake_minimum_required(VERSION 3.12)
project(MyMqttProject LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 配置并引入 MQTT-C 子项目
set(MQTT_C_EXAMPLES OFF CACHE BOOL "Disable MQTT-C examples" FORCE)
set(MQTT_C_TESTS OFF CACHE BOOL "Disable MQTT-C tests" FORCE)
add_subdirectory(third_party/MQTT-C)

# 2. 创建我们的平台抽象库
# 这个库封装了所有平台相关的网络代码和 PAL 实现
add_library(mqtt_platform)

# 添加与平台无关的公共源文件
target_sources(mqtt_platform PRIVATE src/platform/mqtt_network.cpp)

# 公开 'src' 目录作为包含路径，这样客户端代码 (#include "platform/mqtt_network.h") 才能找到头文件
target_include_directories(mqtt_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 因为 mqtt_platform 的公共头文件 (mqtt_network.h) 包含了 mqttc 的头文件 (mqtt_pal.h),
# 所以这个链接依赖必须是 PUBLIC。
# 这将确保任何链接到 mqtt_platform 的目标（如 publisher）也能找到 mqttc 的头文件。
target_link_libraries(mqtt_platform PUBLIC mqttc)

# 3. 添加平台相关的源文件和依赖
if(UNIX AND NOT APPLE) # Linux
    message(STATUS "Configuring for Linux")
    target_sources(mqtt_platform PRIVATE
        src/platform/detail/network_posix.cpp
        src/platform/pal_posix.cpp
    )
    find_package(Threads REQUIRED)
    # 线程库是 mqtt_platform 的内部实现细节，所以是 PRIVATE
    target_link_libraries(mqtt_platform PRIVATE Threads::Threads)
else()
    message(FATAL_ERROR "Unsupported platform for mqtt_platform library")
endif()


# 4. 定义可执行文件
add_executable(publisher src/publisher.cpp)
add_executable(subscriber src/subscriber.cpp)

# 5. 链接可执行文件到我们的平台库
# publisher 和 subscriber 本身不直接暴露 mqtt_platform 的 API，所以这里使用 PRIVATE 是正确的。
# CMake 会自动处理从 mqtt_platform 传递过来的 PUBLIC 依赖 (即 mqttc)。
target_link_libraries(publisher PRIVATE mqtt_platform)
target_link_libraries(subscriber PRIVATE mqtt_platform)