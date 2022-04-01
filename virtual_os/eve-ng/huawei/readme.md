


# 配置文档
https://support.huawei.com/enterprise/zh/index.html


# usg6000v 配置
https://support.huawei.com/enterprise/zh/security/usg6000v-pid-21431620


system-view
web-manager enable


<Huawei>system-view     //进入管理视图
[Huawei]sysname AR1     //修改路由器的设备名称
[AR1]interface GigabitEthernet 0/0/0    //进入接口视图
[AR1-GigabitEthernet0/0/0]ip address 10.1.1.2 24        //配置接口IP
[AR1-GigabitEthernet0/0/0]quit      //返回系统视图
[AR1]ip route-static 0.0.0.0 0.0.0.0 10.1.1.1       //配置默认路由
[AR1]quit       //返回用户视图
<AR1>save       //保存配置

interface GigabitEthernet0/0/1
 service-manage http permit   允许http登录
 service-manage https permit 允许https登录
 service-manage ping permit  允许ping
 service-manage ssh permit 允许ssh登录
 service-manage snmp permit 允许snmp
 service-manage telnet permit 允许telnet登录

查看接口状态
display ip interface brief


firewall zone trust
 add interface GigabitEthernet1/0/0

 https://xxx:8443  admin Admin@123

 

# 问题
- https://blog.csdn.net/MAsunshine/article/details/109222368
- https://support.huawei.com/enterprise/zh/knowledge/EKB1100007825

```s
interface g 0/0/1
undo ip binding vpn-instance default



firewall zone trust
add interface GigabitEthernet 0/0/0
display zone
```

