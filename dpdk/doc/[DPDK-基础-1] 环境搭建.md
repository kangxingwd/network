
# 1 准备 ubuntu16 虚拟机

- 两张网卡
    - 桥接网卡作为 DPDK 运行的网卡
    - NAT 网卡作为 ssh 连接的网卡

# 2 修改虚拟机参数
修改 xxx.vmx 虚拟机配置2文件

将 ethernet0.virtualDev 由 e1000 修改 vmxnet3，因为 vmware 的 vmxnet3 支持多队列网卡

# 3 修改系统启动参数

## 物理机

## 虚拟机

以 ubuntu 16 为例

```
vi /etc/default/grub

# 找到 GRUB_CMDLINE_LINUX ， 修改如下，  net.ifnames=0 biosdevname=0  表示将 ensxx 修改为  ethxx， 后面是大页参数
# GRUB_CMDLINE_LINUX="net.ifnames=0 biosdevname=0 default_hugepages=1G hugepagesz=2M hugepages=1024 isolcpus=0-2" 


sudo update-grub
reboot
```

# 4 查看系统是否支持多队列网卡
cat /proc/interrupts

```

  56:          0         71          0        217   PCI-MSI 1572864-edge      eth0-rxtx-0
  57:          0          0         23         87   PCI-MSI 1572865-edge      eth0-rxtx-1
  58:          0          0          0         32   PCI-MSI 1572866-edge      eth0-rxtx-2
  59:          8          0          0         17   PCI-MSI 1572867-edge      eth0-rxtx-3

```


# 5 编译 DPDK

- 下载 dpdk : https://core.dpdk.org/download/


## 6 



