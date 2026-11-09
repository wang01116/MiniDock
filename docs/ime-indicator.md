# 顶部输入法图标修复（本地 v8）

修复顶部状态栏可以切换中英文、文字却一直显示 EN 的问题。中文模式显示“中”，英文布局或中文输入法的英文模式显示“EN”。同一中文输入法内用 Shift 切换时也重新查询，不靠点击次数猜测状态。

原代码在 Dock UI 线程直接对其他应用的焦点窗口调用 ImmGetContext / ImmGetConversionStatus，失败后 native 默认 FALSE，最终显示 EN。微软的 [IMM 多线程说明](https://learn.microsoft.com/en-us/windows/win32/intl/developing-ime-aware-multiple-thread-applications) 明确了输入法上下文的线程所有权限制。

现在先解析前台实际焦点子窗口及其所属线程，再使用 [ImmGetDefaultIMEWnd](https://learn.microsoft.com/en-us/windows/win32/api/immdev/nf-immdev-immgetdefaultimewnd) 查询目标线程的默认输入法窗口，通过 WM_IME_CONTROL 读取打开状态和转换模式。中文布局不等于正在中文模式；关闭输入法时，即使 native 位残留，也显示 EN。直接 HIMC 读取只作为本 UI 线程的回退，并配对释放上下文。

跨线程查询使用 [SendMessageTimeout](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendmessagetimeoutw)，每条消息超时 20 ms，最多两次查询。超时、权限限制、缺失默认 IME 窗口或不支持查询时，中文布局显示中性的“IME”，不把失败误报为英文。英文布局直接显示 EN，不发送 IME 查询消息。

点击切换逻辑保持原样；实际切换由应用异步处理后，状态栏沿用已有 300 ms 刷新和点击后的原有一次刷新读取结果。未新增常驻线程、键盘钩子、按键注入或周期定时器，也未修改 V7 的全屏隐藏规则与底部逻辑。

验证包括 23 项确定性输入法检查及原有 198 项回归，共 **221 项通过**。另启动了一个隐藏私有 EDIT 窗口的独立子进程，在不激活窗口、不修改用户输入法状态的情况下，观察到跨进程 ImmGetContext 返回 NULL，而默认 IME 窗口及两条状态查询均成功。该样本的 open=0、conversion=1，验证了关闭状态不能只看 native 位。真实微软/第三方输入法与每种应用的交互仍需试用；未声称所有 TSF/受保护应用都支持兼容查询。

同 GCC 16.2.0、原版资源及构建参数，V7 和 V8 的 EXE 均为 **436,736 字节**，未增加文件体积。导入的 20 个系统 DLL 集合不变，PE `.bss` 为 0x1638a0（V7 为 0x1638c0）。两条跨线程查询有实际开销，不宣称 CPU 成本为零，未把旧版整进程测量当作 V8 实测。
