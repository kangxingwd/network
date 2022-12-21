# tcprelay 数据包回放


如果不对包文件进行改写，则重发包流程为：

1.       先对包进行区分客户端和服务器端:

tcpprep -a client -i http.pcap -o http.cache  

2.       重放包文件，命令为：

tcpreplay –p 1 -c http.cache -i eth0 -I eth1 http.pcap



如果对包文件进行改写，则重发包文件流程为：

1.       先对包进行区分客户端和服务器端：

tcpprep -a client -i http. pcap -o http. Cache

2.       对包文件进行改写：

tcprewrite -e 192.85.1.2:192.85.2.2 --enet-dmac=00:15:17:2b:ca:14,00:15:17:2b:ca:15 --enet-smac=00:10:f3:19:79:86,00:10:f3:19:79:87 -c test.cache -i test.tcpdump -o 1.pcap

3.       重放包文件：

tcpreplay -i eth0 -I eth1 -l 1000 -t -c /dev/shm/test.cache /dev/shm/1.pcap


# tt.sh
```sh
#!/bin/bash

INF_1=ens37
INF_2=ens38
PCAP_FILE=test1.pcap
CACHE_FILE=test1.cache

if [ -z $1 ];then
	echo "xx.sh xx.pcap eth1 eth2"
	exit 0
fi

if [ x"$1" != x"" ];then
    PCAP_FILE=$1
fi

if [ x"$2" != x"" ];then
    INF_1=$2
fi

if [ x"$3" != x"" ];then
    INF_2=$3
fi

rm -f $CACHE_FILE
tcpprep -a client -i $PCAP_FILE -o $CACHE_FILE

while true;do
	tcpreplay -M 1 -c $CACHE_FILE -i $INF_1 -I $INF_2 $PCAP_FILE
	sleep 1
done

```

```s
tcpprep -a client -i test1.pcap -o test1.cache
tcpreplay -p 1 -c test1.cache -i ens37 -I ens38 test1.pcap
tcpreplay -M 1 -c test1.cache -i ens37 -I ens38 test1.pcap

# 一直回放
nohup /root/tt.sh >> out.info 2>&1 &
nohup /root/tt.sh test2.pcap ens37 ens38 >> out.info 2>&1 &
```

