
# 可参考
https://support.huawei.com/enterprise/zh/doc/EDOC1000154804/a937c90e

# 基本
```s
# 
en

# 进入配置 
conf t

```

# 配置Cisco防火墙接口的IP地址
```s
ASA5520> en 
ASA5520# configure terminal 
ASA5520(config)# interface GigabitEthernet 0/1 
ASA5520(config-if)# nameif in 
ASA5520(config-if)# security-level 90 
ASA5520(config-if)# ip address 10.1.3.1 255.255.255.0 
ASA5520(config-if)# exit 
ASA5520(config)# interface interface GigabitEthernet 0/2 
ASA5520(config-if)# nameif out 
ASA5520(config-if)# security-level 10 
ASA5520(config-if)# ip address 1.1.5.1 255.255.255.0 
ASA5520(config-if)# exit
```


# 打开Cisco防火墙接口的访问控制
```s
ASA5520(config)# access-list 10 extended permit icmp any any 
ASA5520(config)# access-group 10 in interface in 
ASA5520(config)# access-group 10 out interface in 
ASA5520(config)# access-group 10 in interface out 
ASA5520(config)# access-group 10 out interface out
```

# 配置Cisco防火墙到Internet的缺省路由，假设下一跳地址为1.1.5.2
```s
ASA5520(config)# route out 0.0.0.0 0.0.0.0 1.1.5.2 
```

