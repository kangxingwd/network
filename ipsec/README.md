# strongSwan

# Openswan
https://github.com/xelerance/Openswan

# 协议网
https://www.iana.org/protocols



# ipsec_crypto
- 使用 gmssl 3.1.1， 测试gmssl

# ipsecvpn_sa
- 在dpdk基础上，使用 libnl库创建 netlink sock， 同步内核的sa

# ipsecvpn_sync_kernel
- 使用 libnl 中的 xfrm 库， 使用缓存接口，同步内核的sa，sp
- 由于没有找到合适方式，在多线程中操作缓存，只作为测试使用

# ipsec_netlink
- 自己创建 netlink sock，同步内核sa， 封装， 没有写完，暂时用不了

