


1. 开启NAT穿越时，IKEv1协商第一阶段的前两个消息会发送标识NAT穿越（NAT Traversal，简称NAT-T）能力的Vendor ID载荷（主模式和野蛮模式都是）。用于检查通信双方是否支持NAT-T。
当双方都在各自的消息中包含了该载荷时，才会进行相关的NAT-T协商。

2. 主模式消息3和消息4（野蛮模式消息2和消息3）中发送NAT-D（NAT Discovery）载荷。NAT-D载荷用于探测两个要建立IPSec隧道的网关之间是否存在NAT网关以及NAT网关的位置。
通过协商双方向对端发送源和目的的IP地址与端口的Hash值，就可以检测到地址和端口在传输过程中是否发生变化。若果协商双方计算出来的Hash值与它收到的Hash值一样，则表示它们之间没有NAT。否则，则说明传输过程中对IP或端口进行了NAT转换。

第一个NAT-D载荷为对端IP和端口的Hash值，第二个NAT-D载荷为本端IP和端口的Hash值。

3. 发现NAT网关后，后续ISAKMP消息（主模式从消息5、野蛮模式从消息3开始）的端口号转换为4500。ISAKMP报文标识了“Non-ESP Marker”。

4. 在第二阶段会启用NAT穿越协商。在IKE中增加了两种IPSec报文封装模式：UDP封装隧道模式报文（UDP-Encapsulated-Tunnel）和UDP封装传输模式报文（UDP-Encapsulated-Transport）通过为ESP报文封装UDP头，当封装后的报文通过NAT设备时，NAT设备对该报文的外层IP头和增加的UDP头进行地址和端口号转换。UDP报文端口号修改为4500。


