
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
```s
vi /etc/default/grub

# 找到 GRUB_CMDLINE_LINUX ， 修改如下，  net.ifnames=0 biosdevname=0  表示将 ensxx 修改为  ethxx， 后面是大页参数
# GRUB_CMDLINE_LINUX="net.ifnames=0 biosdevname=0 default_hugepages=1G hugepagesz=2M hugepages=1024 isolcpus=0-2" 


sudo update-grub
reboot
```

# 4 查看系统是否支持多队列网卡
cat /proc/interrupts

```s

  56:          0         71          0        217   PCI-MSI 1572864-edge      eth0-rxtx-0
  57:          0          0         23         87   PCI-MSI 1572865-edge      eth0-rxtx-1
  58:          0          0          0         32   PCI-MSI 1572866-edge      eth0-rxtx-2
  59:          8          0          0         17   PCI-MSI 1572867-edge      eth0-rxtx-3

```

## 5 安装一些软件

```s
apt-get install libnuma-dev 
# CentOS: yum install libnuma-devel


```


# 6 编译 DPDK

- 下载 dpdk : https://core.dpdk.org/download/
- 19.08.2

- xz -dk 
```s
xz -dk dpdk-19.08.2.tar.xz 
tar xvf dpdk-19.08.2.tar
# tar xJf dpdk-19.08.2.tar.xz
cd dpdk-stable-19.08.2/
./usertools/dpdk-setup.sh

# 64 位系统 选择 39
```

## 6 设置 DPDK 的环境变量
```s
export RTE_SDK=/home/kxstar/dpdk/dpdk-stable-19.08.2
export RTE_TARGET=x86_64-native-linux-gcc
```

## testcmd

```s
./usertools/dpdk-setup.sh

# [43] Insert IGB UIO module
# [44] Insert VFIO module 
# [49] Bind Ethernet/Baseband/Crypto device to IGB UIO module
# 选择需要绑定的网口 eth0 
# [53] Run testpmd application in interactive mode ($RTE_TARGET/app/testpmd)

bitmask 7
show port info 0

```

## 编译例子

```s
cd  examples/helloworld
make
./build/helloworld -l 0-3 -n 4
```

