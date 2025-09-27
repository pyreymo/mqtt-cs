# C++ MQTT 示例项目

这是一个使用 C++ 和 `MQTT-C` 库实现的简单 MQTT 发布/订阅（Pub/Sub）示例项目。

项目旨在演示如何在一个现代 C++ 项目中集成一个 C 语言库，并为其创建一个健壮的平台抽象层，以隔离平台相关的代码（如网络套接字）。

## ✨ 主要特点

- **清晰的示例**：包含一个发布者（`publisher`）和一个订阅者（`subscriber`）的完整示例。
- **现代 CMake**：使用现代 CMake 构建系统，结构清晰，易于理解和扩展。
- **平台抽象层 (PAL)**：项目的核心亮点。通过创建一个静态库 `mqtt_platform`，将所有与平台相关的网络代码（基于 POSIX 套接字）封装起来，使主应用程序代码（`publisher.cpp`, `subscriber.cpp`）保持平台无关性。
- **子模块依赖**：使用 Git Submodule 管理 `MQTT-C` 依赖，无需手动下载和配置。

## 🔧 环境要求

在开始之前，请确保您已安装以下工具：

- **CMake** (>= 3.12)
- **C++17 编译器** (如 GCC g++)
- **Git**
- **一个正在运行的 MQTT Broker**：例如 [Mosquitto](https://mosquitto.org/)。项目中的代码默认连接到 `127.0.0.1:1883`。

## 🚀 构建与运行

### 1. 克隆仓库并初始化子模块

`MQTT-C` 库作为 Git 子模块被包含在 `third_party/` 目录中。

```bash
# 克隆主仓库
git clone https://github.com/pyreymo/mqtt-cs
cd mqtt-cs

# 初始化并拉取子模块
git submodule update --init --recursive
```

### 2. 使用 CMake 构建项目

我们推荐使用 out-of-source 的方式进行构建。

```bash
# 创建一个构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
make
```

编译成功后，`build` 目录下会生成两个可执行文件：`publisher` 和 `subscriber`。

### 3. 运行示例

请确保您的 MQTT Broker 正在运行。

**第一步：** 打开一个终端，运行 **订阅者**。它会连接到 Broker 并等待消息。

```bash
./subscriber
```
您会看到如下输出：
```
Successfully connected to MQTT broker.
Subscribed to topic: /test/topic
Waiting for messages... (Press Ctrl+C to exit)
```

**第二步：** 打开另一个终端，运行 **发布者**。它会连接到 Broker，发布 5 条消息，然后断开连接。

```bash
./publisher
```
您会看到如下输出：
```
Successfully connected to MQTT broker.
Published: "Hello from C++ publisher! Count: 0"
Published: "Hello from C++ publisher! Count: 1"
Published: "Hello from C++ publisher! Count: 2"
Published: "Hello from C++ publisher! Count: 3"
Published: "Hello from C++ publisher! Count: 4"
Disconnecting...
Publisher finished.
```

与此同时，在 **订阅者** 的终端中，您会看到接收到的消息：
```
Received publish:
  Topic: /test/topic
  Payload: Hello from C++ publisher! Count: 0

Received publish:
  Topic: /test/topic
  Payload: Hello from C++ publisher! Count: 1

... (依此类推) ...
```

在订阅者终端按下 `Ctrl+C` 可以优雅地退出程序。

## 📁 项目结构

```
.
├── CMakeLists.txt                # 主 CMake 配置文件
├── src
│   ├── platform
│   │   ├── detail
│   │   │   └── network_posix.cpp   # POSIX 网络底层实现
│   │   ├── mqtt_network.h          # 平台网络层的 C++ 封装接口
│   │   ├── mqtt_network.cpp        # C++ 封装的实现
│   │   └── pal_posix.cpp           # MQTT-C PAL 的 POSIX 实现
│   ├── publisher.cpp             # 发布者主程序
│   └── subscriber.cpp            # 订阅者主程序
└── third_party
    └── MQTT-C                    # MQTT-C 库 (Git Submodule)
```

## 💡 实现细节：平台抽象层

本项目的一个关键设计是 `mqtt_platform` 库，它有两个主要职责：

1.  **为 `MQTT-C` 提供平台抽象层 (PAL)**：
    `MQTT-C` 库本身是平台无关的，但它需要用户提供一组函数来处理网络 I/O、时间戳和互斥锁。这些函数在 `mqtt_pal.h` 中定义。本项目在 `src/platform/pal_posix.cpp` 中为 `MQTT-C` 提供了基于 POSIX (Linux) 的实现。

2.  **为 C++ 应用提供简洁的网络接口**：
    为了让 `publisher.cpp` 和 `subscriber.cpp` 的代码更简洁，我们创建了一个 C++ 类 `MqttPlatform::MqttNetwork` (`mqtt_network.h`/`.cpp`)。这个类封装了创建、连接和断开套接字的底层细节，只向上层应用暴露简单的 `connect()`、`disconnect()` 和 `get_socket_handle()` 接口。

通过这种设计，如果需要将项目移植到其他平台（例如 Windows 或 FreeRTOS），我们只需要：
- 在 `src/platform` 目录下提供一个新的 PAL 和网络实现文件。
- 修改 `CMakeLists.txt` 以包含新文件。

而应用程序逻辑（`publisher.cpp` 和 `subscriber.cpp`）则完全不需要改动。