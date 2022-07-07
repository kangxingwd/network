

# cache的预取

- 作用：提高程序的执行效率；  （通俗的将， 如果顺序访问数组元素， 那么在开始访问的时候，硬件预取会自动的预取接下来需要访问的地址）

- 预取指令列表：   (英特尔的 NetBurst 架构)
    - PREFETCH0   将数据存放在每一级cache
    - PREFETCH1   将数据存放在除L1cache的每一个cache， L2，L3 cache
    - PREFETCH2   将数据存放在除L3cache


# cache 一致性

- cache一致性问题的根源： 
    存在多个处理器独占的cache， 而不是多个处理器共享cache， 如果所有处理器共享cache， 在一个指令周期内， 只有一个处理器核心
    能够通过这个cache做内存读写操作， 不会有一致性问题， 但是这种解决方法带来的问题就是效率降低，处理器时间都花费在排队上；

- dpdk解决cache一致性：
    - 避免多个核同事访问同一个内存地址或者数据结构；
    - 例如对于很多数据结构， 定义时都是数组的方式，而数组的长度都是以最大的core大小； lcore[RTE_MAX_LCORE], 这样避免了多个核访问同一个数据结构；
    - 对于网络端口的访问，避免多个核访问同一个网卡的 接受/发送队列， 利用网卡多队列的特性， 为每个核准备单独的 接受/发送队列；

# numa 架构

    - 是由 SMP（ 对称多处理器 ） 系统演化而来
    - 简单理解就是 每个cpu处理器有自己的本地内存， 处理器和本地内存之间有更小的延迟和更大的带宽， 而整个内存仍然可以作为一个整体，任何处理器都能
        访问到， 只不过跨处理器访问内存时速度慢一点；  （同时每个处理器可以有本地的总线等和内存一样）
    
    - dpdk 中 socket_id 可以标识  node id，    socketid 可以和 lcore_id 绑定
        socketid = rte_lcore_to_socket_id(lcore_id);
        rte_pktmbuf_pool_create(s, nb_mbuf,MEMPOOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, socketid);  

        申请内存时， 在对应的 socketid（numa node节点） 上申请内存， 然后处理器 lcore_id 访问本地的内存速度更快

# 并行计算

- 处理器性能提升的主要途径： 提高IPC 和 提高处理器主频率
- dpdk利用多线程绑定cpu来提高cpu cache的命中率， 从而减少内存损耗，提高程序的访问速度；

# 同步与互斥

## linux 内核原子操作
- 原子证书操作
    atomic_t， 对应函数  atomic_xxx （atomic_read/atomic_set/atomic_add 等）
- 原子位操作
     set_bit/clear_bit/test_and_set_bit  等

## dpdk 原子操作

- 内存屏障
    - rte_mb()
    - rte_wmb()
    - rte_rmb()
    - 

- 锁
    - 自旋锁
    - 读写锁
    - 无锁队列


# 报文转发

## 转发模型分为  run to completion(RTC) 和 pipeline(流水线) 模型



