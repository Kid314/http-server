# High-Performance Inference Gateway

![Language](https://img.shields.io/badge/language-C++-00599C.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)

> **Note**: This project is evolving from a general-purpose HTTP server into a **High-Performance AI Inference Gateway**. The repository is currently organized into two major branches to separate the new AI infrastructure capabilities from the stable network foundation.

---

## 🔥 Branch: AI-infra-dynamic-batching (Core Focus)
**High-Performance Inference Gateway**

This branch represents the core value of the project, focusing on **AI Infrastructure**. It is designed to solve high concurrency and throughput bottlenecks in Large Language Model (LLM) inference scenarios by acting as a specialized gateway that optimizes resource scheduling before requests reach the backend.

### Key Features

#### 1. Dynamic Batching (Continuous Batching)
*   **Mechanism**: Implements a sophisticated **Request Queue and Scheduler** inspired by Triton Inference Server's Continuous Batching. It automatically merges multiple independent inference requests arriving within a specific time window into a single execution Batch.
*   **Throughput Optimization**: Significantly improves backend inference throughput by minimizing the invocation overhead of the inference engine (GPU/NPU) while strictly adhering to latency SLAs.

#### 2. High-Performance Network Architecture
*   **Reactor Model**: Built on **Epoll + Non-blocking I/O**, utilizing a **Main-Sub Reactor** event-driven architecture (One Loop Per Thread).
*   **Concurrency**: Capable of sustaining massive concurrent connections with minimal context-switching overhead, ensuring high throughput for gateway operations.

#### 3. Concurrency & Intelligent Scheduling
*   **Thread Pool**: Implements a fixed-size thread pool based on C++11 to eliminate system jitter caused by frequent thread creation and destruction.
*   **Priority Queue**: Introduces a priority-based scheduling mechanism to ensure low-latency responses for high-priority tasks (e.g., real-time user inference requests).

#### 4. Extreme Memory Optimization
*   **Object Pool**: Utilizes Object Pools to reuse connection objects and buffers, drastically reducing memory fragmentation and allocation overhead.
*   **RAII Resource Management**: Fully adopts `std::shared_ptr` and `std::unique_ptr` throughout the codebase to ensure zero memory leaks.

---

## 🏗️ Branch: main (Foundation)
**Underlying High-Performance Network Base**

This branch serves as the robust C++ HTTP server foundation that powers the upper-layer AI gateway. It focuses on stability, standard compliance, and efficient raw data transmission.

### Infrastructure Features
*   **RAII Encapsulation**: Strictly manages File Descriptors (FD) and Socket resources using RAII principles.
*   **Zero-Copy Architecture**: Extensively uses C++17 `std::string_view` in the HTTP parsing layer to avoid unnecessary memory copying.
*   **Efficient Buffering**: Implements a complete sending buffer mechanism based on `epollout` events to handle large data packets smoothly.
*   **Static File Service**: Supports high-performance static file serving (e.g., `/public` directory) with HTTP Keep-Alive support.

### 📊 Performance Benchmarks (wrk)
*Metrics tested on the `main` branch with Keep-Alive enabled:*

| Metric | Value |
| :--- | :--- |
| **Requests/sec (QPS)** | **100,000+** |
| **Concurrency** | **High Concurrency Support** |

> *Note: Performance results may vary based on hardware configuration.*

---

<!-- Chinese Version Starts Here -->

# 高性能推理网关 (High-Performance Inference Gateway)

> **注意**: 本项目目前正从通用 HTTP Server 转型为 **高性能 AI 推理网关**。项目包含两个核心分支，分别对应 **AI 推理基础设施架构** (`AI-infra-dynamic-batching`) 和 **通用高性能网络基座** (`main`)。

---

## 🔥 AI Infra 分支：核心卖点
**Branch:** `AI-infra-dynamic-batching`

这是本项目的核心开发分支，专注于解决大模型推理场景下的高并发与吞吐量瓶颈，实现了类 Triton 的核心调度机制。

### 核心特性

#### 1. 动态批处理 (Dynamic Batching)
*   **Continuous Batching 机制**：设计了高效的请求队列与调度器，能够将同一时间窗口内的多个独立并发推理请求自动合并为 Batch 发送至后端。
*   **吞吐量极大化**：通过请求合并，显著减少了后端推理引擎（Inference Engine）的调用次数，在保证延迟 SLA 的前提下，大幅提升 GPU/NPU 的计算利用率。

#### 2. 高性能网络架构 (Reactor Model)
*   **主从 Reactor 模型**：基于 `Epoll` + 非阻塞 I/O 实现 **One Loop Per Thread** 架构。
*   **低上下文切换**：有效支持海量并发连接维持，通过减少线程间的上下文切换开销，实现高吞吐的网络 I/O 处理。

#### 3. 并发调度与低延迟
*   **线程池优化**：基于 C++11 实现固定大小线程池，避免线程频繁创建与销毁带来的系统抖动。
*   **优先级队列**：引入优先级调度策略，保障高优先级推理任务（如线上实时请求）能获得极低延迟的响应。

#### 4. 极致内存管理
*   **对象池 (Object Pool)**：实现了连接对象与 Buffer 的池化复用，显著减少内存碎片和频繁的 `malloc/free` 开销。
*   **RAII 资源管理**：全线采用 `std::shared_ptr` 与 `std::unique_ptr` 管理生命周期，彻底杜绝内存泄漏。

---

## 🏗️ Main 分支：底层基座
**Branch:** `main`

该分支提供了稳健的底层 HTTP Server 实现，为上层 AI 网关提供了高效的网络传输支持，亦可作为通用的 C++ Web 服务器使用。

### 基础设施功能
*   **RAII 封装**：对文件描述符（FD）、Socket 资源进行严格的 RAII 封装，确保资源安全释放。
*   **零拷贝 (Zero-Copy)**：核心解析层大量使用 C++17 `std::string_view`，避免不必要的内存拷贝，提升数据包解析效率。
*   **高效缓冲管理**：实现了基于 `epollout` 事件触发的发送缓冲区机制，平滑处理大数据包发送。
*   **静态资源服务**：支持高效的静态文件读取与传输（支持 `/public` 目录），并处理了 HTTP Keep-Alive 连接复用。

### 📊 性能指标 (wrk)
*在此分支下进行的基准测试（开启 Keep-Alive）：*

| 指标 | 测试结果 |
| :--- | :--- |
| **QPS (Requests/sec)** | **100,000+** |
| **并发能力** | **支持高并发连接** |
