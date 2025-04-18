您在将 Makefile 中的 `STD := c++14` 改为 `STD := c++17` 后遇到的编译错误，是由于 `log4cpp` 使用了 C++17 中已被废弃的动态异常规范（dynamic exception specification）。

### 问题原因

在 C++17 中，动态异常规范（例如 `throw(std::invalid_argument)`）已被完全移除，使用这些规范会导致编译错误。 `log4cpp` 的旧版本（如 1.1.1）在头文件中使用了这些规范，因此在使用 C++17 编译时会出现错误。

### 解决方案

#### 1. 升级 `log4cpp` 至最新版本

`log4cpp` 的开发者已在较新的版本中移除了动态异常规范。您可以从其 [Git 仓库](https://sourceforge.net/p/log4cpp/codegit/ci/master/tree/) 获取最新的源代码，并按照以下步骤进行编译和安装：

```bash
git clone https://git.code.sf.net/p/log4cpp/codegit log4cpp
cd log4cpp
./autogen.sh
./configure
make
sudo make install
```

安装完成后，请确保您的项目使用的是新安装的 `log4cpp` 版本。

#### 2. 修改旧版本的 `log4cpp` 源码

如果您无法升级 `log4cpp`，可以手动修改其源代码，移除所有的动态异常规范。例如，将以下代码：

```cpp
void setPriority(Priority::Value priority) throw(std::invalid_argument);
```

修改为：

```cpp
void setPriority(Priority::Value priority);
```

请注意，这种方法需要您对 `log4cpp` 的源代码进行全面审查和修改，以确保所有的动态异常规范都被移除。

#### 3. 继续使用 C++14

如果上述方法不可行，您可以暂时将项目的编译标准恢复为 C++14，方法是将 Makefile 中的 `STD := c++17` 改回 `STD := c++14`。这将允许您继续使用旧版本的 `log4cpp`，但可能无法使用 C++17 的新特性。

### 建议

为了确保项目的可维护性和兼容性，建议您升级 `log4cpp` 至支持 C++17 的最新版本。 

---


### 卸载旧版本的 log4cpp

如果您是通过源码安装的 log4cpp（例如使用 `./configure && make && make install`），可以按照以下步骤卸载：

1. **查找安装路径**：默认情况下，log4cpp 会安装在 `/usr/local/` 目录下。您可以使用以下命令查找相关文件：
   ```bash
   find /usr/local -name "*log4cpp*"
   ```


2. **删除相关文件**：根据查找到的文件路径，使用 `rm` 命令删除相关文件。例如：
   ```bash
   sudo rm -rf /usr/local/include/log4cpp
   sudo rm -f /usr/local/lib/liblog4cpp.*
   sudo rm -f /usr/local/bin/log4cpp-config
   ```


3. **更新动态链接库缓存**：执行以下命令更新系统的动态链接库缓存：
   ```bash
   sudo ldconfig
   ```


如果您是通过包管理器安装的 log4cpp（例如使用 `apt`），可以使用以下命令卸载：
```bash
sudo apt remove liblog4cpp5-dev liblog4cpp5
```


