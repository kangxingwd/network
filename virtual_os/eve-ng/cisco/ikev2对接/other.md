# 基本操作：
```s
# 进入命令模式
en 

# 进入配置
conf t
```

# 镜像问题:  “password required, but none set”
```s
configure terminal
line vty 0
password 123456
exit

enable password 123456
```

# 思科中文
https://www.cisco.com/c/zh_cn/index.html?country-redirect=true
支持→产品和下载→所有产品

1000v
show version
Cisco IOS XE 15.6(1)s
https://www.cisco.com/c/zh_cn/support/routers/cloud-services-router-1000v-series/products-installation-and-configuration-guides-list.html
https://www.cisco.com/c/en/us/td/docs/ios-xml/ios/sec_conn_ikevpn/configuration/xe-16/sec-ike-for-ipsec-vpns-xe-16-book/sec-key-exch-ipsec.html



show crypto session
show crypto session detail
clear crypto session            # Deletes crypto sessions (IPSec and IKE SAs).


# 默认配置
```
show crypto ikev2 authorization policy default
show crypto ikev2 proposal
show crypto ikev2 policy default
show crypto ipsec profile default
show crypto ipsec transform-set default
```

# 全局配置
```
enable
configure terminal
crypto ikev2 certificate-cache number-of-certificates
crypto ikev2 cookie-challenge number
crypto ikev2 diagnose error number
crypto ikev2 dpd interval retry-interval {on-demand | periodic} 7. crypto ikev2 http-url cert
crypto ikev2 limit {max-in-negotiation-sa limit | max-sa limit}
crypto ikev2 nat keepalive interval
crypto ikev2 window size
crypto logging ikev2
end
```



# 
```s
show crypto isakmp sa 
show crypto ipsec sa
show crypto ikev2 sa
show crypto engine connections active 
```

# 其他命令
```s
clear crypto sa
config t
logging buffered 409600
logging buffered debugging
logging console debugging
crypto ikev2 diagnose error 1000
exit
debug crypto ipsec
debug crypto isakmp
debug crypto ikev2
debug crypto ikev2 error
debug crypto ikev2 packet
debug crypto isakmp error
debug crypto ipsec message
debug crypto ipsec states
debug crypto isakmp error
debug crypto ikev2 internal

no debug crypto ipsec
no debug crypto isakmp
no debug crypto ikev2
no debug crypto ikev2 error
no debug crypto ikev2 packet
no debug crypto isakmp error
no debug crypto ipsec message
no debug crypto ipsec states
no debug crypto isakmp error
no debug crypto ikev2 internal
```