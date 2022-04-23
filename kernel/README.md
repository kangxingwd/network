# network


## netfilter 编程
- nf_register_hook 在新版内核里面换成了 nf_register_net_hook(struct net *net, const struct nf_hook_ops *ops);
```cpp
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
nf_register_net_hook(&init_net, &reg)  //&init_net 可直接使用
#else
nf_register_hook(&reg)
#endif
```

- 自从kernel4.13开始 hook函数的原型就是
```cpp
int sample_nf_hookfn(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
```

## ip 转换

```cpp
#include <arpe/inet.h>

//将点分十进制的ip地址转化为用于网络传输的数值格式返回值：若成功则为1，若输入不是有效的表达式则为0，若出错则为-1
int inet_pton(int family, const char *strptr, void *addrptr);

//将数值格式转化为点分十进制的ip地址格式返回值：若成功则为指向结构的指针，若出错则为NULL
const char * inet_ntop(int family, const void *addrptr, char *strptr, size_t len);

// family： AF_INET(ipv4), AF_INET6(ipv6)
```

# 内核中 ip字符串转换
```cpp
/**
 * in4_pton - convert an IPv4 address from literal to binary representation
 * @src: the start of the IPv4 address string
 * @srclen: the length of the string, -1 means strlen(src)
 * @dst: the binary (u8[4] array) representation of the IPv4 address
 * @delim: the delimiter of the IPv4 address in @src, -1 means no delimiter
 * @end: A pointer to the end of the parsed string will be placed here
 *
 * Return one on success, return zero when any error occurs
 * and @end will point to the end of the parsed string.
 *
 */
int in4_pton(const char *src, int srclen, u8 *dst, int delim, const char **end);


/**
 * in6_pton - convert an IPv6 address from literal to binary representation
 * @src: the start of the IPv6 address string
 * @srclen: the length of the string, -1 means strlen(src)
 * @dst: the binary (u8[16] array) representation of the IPv6 address
 * @delim: the delimiter of the IPv6 address in @src, -1 means no delimiter
 * @end: A pointer to the end of the parsed string will be placed here
 *
 * Return one on success, return zero when any error occurs
 * and @end will point to the end of the parsed string.
 *
 */
int in6_pton(const char *src, int srclen, u8 *dst, int delim, const char **end);

/*
 * Convert an ASCII string to binary IP.
 * This is outside of net/ipv4/ because various code that uses IP addresses
 * is otherwise not dependent on the TCP/IP stack.
 */
__be32 in_aton(const char *str);
```


