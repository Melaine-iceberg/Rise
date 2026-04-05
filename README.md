# co_async

`co_async` 是一个基于 C++20 coroutine 的异步 IO 库，当前源码集中在 `co_async/` 目录中。

这个仓库现在按“库项目”组织，而不是应用程序：保留公开头文件与实现源码，清理构建产物，并提供基础的 CMake 安装/导出能力，方便下游通过 `find_package(co_async CONFIG REQUIRED)` 使用。

## 依赖

- `zlib`
- `liburing`
- `bearssl`
- 支持 C++20 的编译器

如果你使用 vcpkg，建议通过 manifest mode 提供依赖；如果你使用系统包管理器，也可以只要让 CMake 能找到这些依赖即可。

## 构建

推荐使用仓库内预设：

- 配置：`debug`
- 构建：`debug`

标准 CMake 工作流同样适用：配置一个独立构建目录，然后编译 `co_async` 目标。

## 作为库使用

安装后，下游项目可以通过 CMake 使用：

1. `find_package(co_async CONFIG REQUIRED)`
2. 链接目标 `co_async::co_async`

公开头文件使用形如 `#include <co_async/co_async.hpp>` 的路径。

## 当前范围

- 保留并整理库源码目录 `co_async/`
- 提供基础安装与导出配置
- 移除构建目录、IDE 缓存和 Windows `Zone.Identifier` 残留文件

## 后续可以继续完善的方向

- 增加 `examples/` 与 `tests/`
- 提供 CI 验证
- 进一步拆分可选模块（例如 `ssl` / `zlib` / `net`）
- 如有需要，再演进到 `include/` 与 `src/` 分层布局
