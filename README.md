# High-Performance Inference Gateway (AI Infra)

![Language](https://img.shields.io/badge/language-C++-00599C.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)

本项目正从通用 HTTP Server 转型为 **高性能 AI 推理网关**。
目前包含两个主要分支，分别对应 **AI 推理基础设施架构** (`AI-infra-dynamic-batching`) 和 **通用高性能网络基座** (`main`)。

---

## 🔥 AI Infra 分支：高性能推理网关
**Branch:** `AI-infra-dynamic-batching`

这是本项目的核心开发分支，专注于解决大模型推理场景下的高并发与吞吐量瓶颈，实现了类 Triton 的核心调度机制。

### 1. 动态批处理 (Dynamic Batching)
*   **Continuous Batching 机制**：实现了请求队列与调度器，能够将同一时间窗口内的多个独立并发推理请求自动合并为 Batch 发送至后端。
*   **吞吐量极大化**：显著减少了后端推理引擎（Inference Engine）的调用次数，在保证延迟 SLA 的前提下，大幅提升 GPU/NPU 的计算利用率。

### 2. 高性能网络架构 (Reactor Model)
*   **主从 Reactor 模型**：基于 `Epoll` + 非阻塞 I/O 实现 **One Loop Per Thread** 架构。
*   **低上下文切换**：有效支持海量并发连接维持，通过减少线程间的上下文切换开销，实现高吞吐的网络 I/O 处理。

### 3. 并发调度与低延迟
*   **线程池优化**：基于 C++11 实现固定大小线程池，避免线程频繁创建与销毁带来的系统抖动。
*   **优先级队列**：引入优先级调度策略，保障高优先级推理任务（如线上实时请求）能获得极低延迟的响应。

### 4. 极致内存管理
*   **对象池 (Object Pool)**：实现了连接对象与 Buffer 的池化复用，显著减少内存碎片和频繁的 `malloc/free` 开销。
*   **RAII 资源管理**：全线采用 `std::shared_ptr` 与 `std::unique_ptr` 管理生命周期，彻底杜绝内存泄漏。

---

## 🏗️ Main 分支：通用网络底层基座
**Branch:** `main`

该分支提供了稳健的底层 HTTP Server 实现，为上层 AI 网关提供了高效的网络传输支持，亦可作为通用的 C++ Web 服务器使用。

*   **RAII 封装**：对文件描述符（FD）、Socket 资源进行严格的 RAII 封装，确保资源安全释放。
*   **零拷贝 (Zero-Copy)**：核心解析层大量使用 C++17 `std::string_view`，避免不必要的内存拷贝，提升数据包解析效率。
*   **高效缓冲管理**：实现了基于 `epollout` 事件触发的发送缓冲区机制，平滑处理大数据包发送。
*   **静态资源服务**：支持高效的静态文件读取与传输（支持 `/public` 目录），并处理了 HTTP Keep-Alive 连接复用。

---
