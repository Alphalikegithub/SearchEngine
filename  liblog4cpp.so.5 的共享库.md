您在执行 `./server` 时遇到的错误：

```
./server: error while loading shared libraries: liblog4cpp.so.5: cannot open shared object file: No such file or directory
```


这是由于系统在运行时无法找到 `liblog4cpp.so.5` 共享库所致。

### 问题原因

虽然您已经成功编译了 `server` 程序，但在运行时，系统需要找到 `liblog4cpp.so.5` 的共享库文件。如果该库文件不在系统默认的库搜索路径中（如 `/lib`、`/usr/lib` 或 `/usr/local/lib`），就会出现上述错误。

### 解决方案

您可以通过以下几种方式解决此问题：

#### 方法一：设置 `LD_LIBRARY_PATH` 环境变量

如果 `liblog4cpp.so.5` 位于非标准目录（例如 `/usr/local/lib`），可以临时设置环境变量：

```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
./server
```


要永久生效，可以将上述 `export` 命令添加到 `~/.bashrc` 或 `~/.bash_profile` 文件中。

#### 方法二：更新系统的库缓存

如果您已将 `liblog4cpp.so.5` 放置在 `/usr/local/lib` 目录下，可以通过以下命令更新系统的库缓存：

```bash
sudo ldconfig
```


这将使系统识别新的库文件路径。

#### 方法三：将库文件复制到标准目录

您也可以将 `liblog4cpp.so.5` 复制到系统的标准库目录，例如：

```bash
sudo cp /usr/local/lib/liblog4cpp.so.5 /usr/lib/
```


然后运行 `sudo ldconfig` 更新库缓存。

#### 方法四：静态链接 `log4cpp` 库

如果您希望避免运行时依赖共享库，可以选择在编译时静态链接 `log4cpp` 库。这需要您拥有 `liblog4cpp.a` 静态库文件，并在 Makefile 中使用静态链接选项。

### 检查库文件位置

您可以使用以下命令查找 `liblog4cpp.so.5` 的位置：

```bash
find /usr -name "liblog4cpp.so.5"
```


确认该文件存在并位于上述方法中提到的路径之一。

### 总结

此问题并非由 Makefile 引起，而是由于系统在运行时无法找到所需的共享库。通过设置环境变量、更新库缓存或调整库文件位置，您可以解决该问题。 