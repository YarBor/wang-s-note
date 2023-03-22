以下为常用的tmux快捷键：

注意：`prefix key`(默认是`Ctrl+b`)表示将键盘焦点从终端会话中移开，以便于后面输入操作指令。

1. 窗口相关

- 创建新窗口: `prefix + c`
- 关闭当前窗口: `prefix + &`
- 跳转到下一个窗口:`prefix + n`
- 跳转到上一个窗口:`prefix + p`
- 选择要切换的窗口:`prefix + w`
- 列出所有窗口:`prefix + s`

2. 终端分割

- 水平分割:`prefix + "`
- 垂直分割:`prefix + %`
- 切换到上一个终端:`prefix + ↑` 或者 `prefix + k`
- 切换到下一个终端:`prefix + ↓` 或者 `prefix + j`
- 切换到左边的终端:`prefix + ←` 或者 `prefix + h`
- 切换到右边的终端:`prefix + →` 或者 `prefix + l`
- 切换当前窗格为主窗格:`prefix + z`

3. 其他

- 显示时间:`prefix + t`
- 显示帮助:`prefix + ?`
- 进入复制模式:`prefix + [`
- 粘贴复制的内容:`prefix + ]`

更多详细的命令可以在终端中输入命令`man tmux`查看。