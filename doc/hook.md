# 键盘钩子说明

## hook_bootmagic()

用于处理bootmagic的hook。

这个hook是tmk自带的，在这个工程中实现了绑定擦除和按键开机的功能。如果要实现自定义的开机流程，或者添加/删除bootmagic命令，可以覆盖这个hook。

## action_function()

TMK内建的自定义动作处理的hook。

这个hook默认实现了切换USB和关机功能，以及自定义动作模块的处理。不建议直接覆盖此hook。若仅是要添加新的Fn功能，请参见driver.md内的相关信息添加新的自定义动作模块。

