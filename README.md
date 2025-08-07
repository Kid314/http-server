# High-Performance HTTP Server

A project to build a high-performance HTTP server from scratch.

## Features

*   **RAII Encapsulation:** File descriptors are managed using the RAII (Resource Acquisition Is Initialization) principle to ensure proper resource management.
*   **Thread Pool:** CPU-intensive tasks are handled by a thread pool for efficient concurrency.
*   **Buffer Management:** Complete send buffer management using `epollout`.
*   **Zero-Copy & String Views:** Utilizes `string_view` for efficient, zero-copy operations (C++17).

## Performance

Achieved **112,927 QPS** (Queries Per Second) in local testing using `wrk`.

## Goals

*   Achieve high concurrency and low latency.
*   Maintain a clean and readable code style.
*   Continuously learn and improve.

---

# 高性能 HTTP 服务器

一个从零开始编写的高性能 HTTP 服务器项目。

## 特性

*   **RAII 封装:** 使用 RAII (资源获取即初始化) 的思想来封装文件描述符，确保资源被妥善管理。
*   **线程池:** 使用线程池来处理 CPU 密集型任务，实现高效并发。
*   **缓冲区管理:** 拥有完整的发送缓冲区管理机制 (epollout)。
*   **零拷贝与视图:** 使用 `string_view` 来实现高效的零拷贝操作 (C++17)。

## 性能

使用 `wrk` 在本地测试中达到了 **112,927 QPS** (每秒请求数)。

## 目标

*   实现高并发、低延迟。
*   保持清晰易读的代码风格。
*   持续学习和迭代。


gemini写的markdown:)因为我不怎么会写md啦:(