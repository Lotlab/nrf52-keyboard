# 键盘钩子说明

## hook_bootmagic()

用于处理bootmagic的hook。

这个hook是tmk自带的，在这个工程中实现了绑定擦除和按键开机的功能。如果要实现自定义的开机流程，或者添加/删除bootmagic命令，可以覆盖这个hook。

## action_function()

自定义动作处理的hook。

这个hook默认实现了切换USB和关机功能，如果需要添加更多的功能或删除某些功能，请覆盖这个钩子。