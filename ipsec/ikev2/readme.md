1
# ikev2
- 定义在RFC4306 ，更新与 RFC 5996



# IKEv2报文协商详细过程
```s
Initiator                         Responder
   -------------------------------------------------------------------
   HDR, SAi1, KEi, Ni  -->
                                  <--  HDR, SAr1, KEr, Nr, [CERTREQ]
 
   HDR, SK {IDi, [CERT,] [CERTREQ,]
       [IDr,] AUTH, SAi2,
       TSi, TSr}  -->
                                     <--  HDR, SK {IDr, [CERT,] AUTH, SAr2, TSi, TSr}

```


```s
IKE_SA_INIT request
    HDR:    包含安全参数索引（SPIs）、版本号和flags
    SAi1:   有效载荷声明了发起方支持的IKE SA加密算法,  ENCR, PRF, INTEG, DHgroup
    KEi:    发送方的DH值
    Ni:     发起方的随机数

IKE_SA_INIT respones
    SAr1:       接收方选择的密码学算法
    KEr:        接收方选择的密钥交换材料
    Nr:         接收方的随机数
    CERTREQ:    证书请求

从发起方提供的 SAi1 负载中选择一个可接受的加密组套件，封装在 SAr1 中，并发送双方都支持的DH算法的最高级封装在 KEr 负载中，以及发送回应方的 Nr 随机值

第1个和第2个包交换完成后，每一方都能生成 SKEYSEED , 所有的密钥都是从这个密钥推导出来的。

以后的消息除了消息头都会被加密和完整性保护。加密和完整性保护的密钥都是来自 SKEYSEED , SK_e 用于加密, SK_a 用于认证做完整性保护。会为每个方向计算单独的 SK_a 和 SK_e

除了密钥来自 DH算法的 SK_e 和 SK_a 之外，还会导出另一个两 SK_d, 用于生成子SA的密钥材料。 符号 SK{…} 表示这些有效载荷已加密使用该方向的 SK_e 和 SK_a 保护完整性

SKEYSEED = prf(Ni | Nr, g^ir)
{SK_d | SK_ai | SK_ar | SK_ei | SK_er | SK_pi | SK_pr } = prf+ (SKEYSEED, Ni | Nr | SPIi | SPIr )



IKE_AUTH：
    HDR, SK {IDi, [CERT,] [CERTREQ,]
       [IDr,] AUTH, SAi2,
       TSi, TSr}  -->

    HDR：   不加密
    SK ：   该负载被加密并且完整性保护
    IDi：   发起方的有效身份载荷，用来证明自己的身份和完整性保护
    CERT：  发起方的证书，可选
    CERTREQ：   发起方的证书请求，可选
    IDr：   期望的相应方的身份信息，可选
    AUTH:   发起方认证数据，没有EAP就没有这个字段，可选
    SAi2:   发起方Child SA转换集
    TSi:    Child SA流量选择器 
    TSr:    Child SA流量选择器 


发起方：除了 HDR 未加密，其余都被加密。IDi 是发起方的有效身份载荷，用来证明自己的身份和完整性保护。
也可以在 CERT 中发送整数有效载荷记忆在CertReq有效宰割中的信任列表。
如果包含任何 CERT 有效载荷，必须提供第一个证书，包含用于验证AUTH字段的公钥。
可选的有效负载IDr使发起者能够指定哪个响应者。当在有同一个IP地址的机器上可能有多个身份ID。如果响应者不接受发起方提出的IDr，响应者可能会使用其他一些IDr完成交换。如果发起方不接收相应方提供的IDr，发起方可以关闭这个SA协商。

IKE_AUTH    
    <--  HDR, SK {IDr, [CERT,] AUTH,
                                            SAr2, TSi, TSr}

    SK:     负载被加密和完整性保护
    IDr：   响应方的身份信息
    Cert：  响应方的证书，可选
    AUTH：  响应方的认证数据，没有EAP就没有这个字段，可选
    SAr2：  响应方选择的Child SA转换集
    TSi/TSr: Child SA流量选择器，感兴趣流

响应方：使用 IDr 有效载荷声明其身份，可选发送一个或多个证书（同样这个证书需要有用于验证AUTH的公钥），AUTH：验证第二消息的身份和完整性保护。SAr2 字段用于发送创建CREATE_CHILD_SA的提议材料。



```



```
SPIi
SPIr
SK_ei
SK_er
Enc
SK_ai
SK_ar
Int
```



