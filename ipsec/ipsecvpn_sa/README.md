# 编译

```s
export RTE_SDK=/root/dpdk/dpdk-stable-19.08.2
export RTE_TARGET=x86_64-native-linuxapp-gcc

make clean
make

./build/ipsecvpn_sa -l 1-4 -n 4

```
