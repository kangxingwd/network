

# emulatedlab
```s
# 登录
kxstar
Wd15529143238

# EVE-NG全系列模板，镜像
https://www.emulatedlab.com/forum.php?mod=viewthread&tid=939


# EVE-NG社区懒人版
https://www.emulatedlab.com/forum.php?mod=viewthread&tid=489


# 教程
https://www.emulatedlab.com/forum.php?mod=viewthread&tid=1465&extra=page%3D1
EVE-NG科普视频第一期文档 - 有道云笔记
https://note.youdao.com/s/PAdvwten
EVE-NG科普视频第二期文档 - 有道云笔记
https://note.youdao.com/s/A8iNsiIr
EVE-NG科普视频第三期文档 - 有道云笔记
https://note.youdao.com/s/KSGUcjnL
EVE-NG科普视频第四期文档 - 有道云笔记
https://note.youdao.com/s/WzolkchW
EVE-NG科普视频第五期文档  - 有道云笔记
https://note.youdao.com/s/Isx4WGvr
EVE-NG科普视频第六期文档 - 有道云笔记
https://note.youdao.com/s/YAky7vMK
第六期文档是桥接部分的补充说明。
EVE-NG相关文件分享链接 - 有道云笔记
https://note.youdao.com/s/ZaFZ1h6Q
```

# eve-ng 登录
```s
# 页面
admin/eve

# 后台
22
root/eve
```

# 导入镜像
```s
# 导入 qemu
# 见 emulatedlab 论坛
# 1、 下载镜像 
#   1) 百度网盘： 心奋斗吧  软件->EVE-NG
#   2) https://www.emulatedlab.com/forum.php?mod=viewthread&tid=939
# 2、解压后，上传到eve-ng 后台目录

# IOL镜像位置：
/opt/unetlab/addons/iol/bin/
# qemu镜像位置：
/opt/unetlab/addons/qemu/
# qemu镜像以目录的名称作为镜像名称，根据目录里面的虚拟硬盘文件名确定硬盘的接口。
# 比如virtioa.qcow2表示硬盘接口是virtio，a表示第一个硬盘。硬盘的顺序按照26个英文字母来排序。

# 设备图标位置：
/opt/unetlab/html/images/icons/
# 设备脚本位置：
/opt/unetlab/scripts/
# 设备模板位置：
/opt/unetlab/html/templates/

# 导入后修复权限
/opt/unetlab/wrappers/unl_wrapper -a fixpermissions

unl_wrapper -a fixpermissions
```

# 懒人版镜像默认密码
- 集成镜像默认密码：
- asav 进特权模式必须先设置密码
- veos 账号为admin 直接回车
- 山石 账号密码均为hillstone
- 华为ar1000v 账号密码均为super
- 华为usg6kv  账号为admin 直接回车
- vyos  账号密码均为vyos
- routeros 账号为admin密码为空直接回车

# eve-ng 教程视频
- https://www.emulatedlab.com/forum.php?mod=viewthread&tid=1465&fromuid=1692


