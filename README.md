To some reasons , I want to write a high performance http server
======================================================
## 1.use RAII encapsulated file descriptors
## 2.use Thread pools handle CPU-intensive tasks(thread future jthread)
- c++11 maybe c++20 final
## 3.have Complete send buffer management(epollout)
## 4.use Zero copy and views(string_view) 
- c++17
## 5.Take one step at a time
## final target
* 1.maybe 100,000 qps
* 2.clear code style
* 3.....hope i can finish it :)
