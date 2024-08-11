#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "ipsecvpn_netlink.h"
#include "log.h"

int open(struct ipsec_netlink_s* ipsec_netlink, unsigned subscriptions, int protocol)
{
    memset(&ipsec_netlink->rth, 0, sizeof(ipsec_netlink->rth));
    ipsec_netlink->rth.fd = socket(AF_NETLINK, SOCK_DGRAM, protocol);
    if (ipsec_netlink->rth.fd < 0) {
        LOG_ERROR("Cannot open netlink socket\n");
        return -1;
    }
    memset(&rth.local, 0, sizeof(rth.local));
    ipsec_netlink->rth.local.nl_family = AF_NETLINK;
    ipsec_netlink->rth.local.nl_groups = subscriptions;

    if (bind(ipsec_netlink->rth.fd, (struct sockaddr*) &ipsec_netlink->rth.local, sizeof(ipsec_netlink->rth.local)) < 0) {
        LOG_ERROR("Cannot bind netlink socket\n");
        return -1;
    }

    socklen_t addr_len = sizeof(rth.local);
    if (getsockname(ipsec_netlink->rth.fd, (struct sockaddr*) &ipsec_netlink->rth.local, &addr_len) < 0) {
        LOG_ERROR("Cannot getsockname\n");
        return -1;
    }

    if (addr_len != sizeof(ipsec_netlink->rth.local)) {
        LOG_ERROR("Wrong address length %d\n", addr_len);
        return -1;
    }

    if (ipsec_netlink->rth.local.nl_family != AF_NETLINK) {
        LOG_ERROR("Wrong address family %d\n", ipsec_netlink->rth.local.nl_family);
        return -1;
    }
    ipsec_netlink->rth.seq = time(NULL);
    //  set_nonblock(rth.fd); // 非阻塞
    return 0;
}

int send_req(struct ipsec_netlink_s* ipsec_netlink, int type)
{
    struct { struct nlmsghdr nlh; } req;
    memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = sizeof(req);
    req.nlh.nlmsg_type = type;
    req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
    req.nlh.nlmsg_pid = getpid();
    req.nlh.nlmsg_seq = ipsec_netlink->rth.dump = ++ipsec_netlink->rth.seq;
    //req.g.rtgen_family = AF_UNSPEC;
    return send(rth.fd, (void*) &req, sizeof(req), 0);
}

int data_is_ready(struct ipsec_netlink_s* ipsec_netlink)
{
    fd_set fdsr;
    struct timeval tv;
    int max_sock = ipsec_netlink->rth.fd;

    FD_ZERO(&fdsr);
    FD_SET(ipsec_netlink->rth.fd, &fdsr);
    tv.tv_sec = 0;
    tv.tv_usec = 1000;

    int ret = select(max_sock + 1, &fdsr, NULL, NULL, &tv); //select等待数据到来
    if (ret <= 0) {
        return 0;
    }

    if (FD_ISSET(ipsec_netlink->rth.fd, &fdsr))
        return 1;

    return 0;
}

int recv_msg(struct ipsec_netlink_s* ipsec_netlink, char *recv_buf,int buf_len)
{
    struct sockaddr_nl nladdr;
    struct iovec iov;
    int status;
    struct msghdr msg= {
            msg_name : &nladdr,
            msg_namelen : sizeof(nladdr),
            msg_iov : &iov,
            msg_iovlen : 1,
    };
    bzero(recv_buf, buf_len);
    iov.iov_base = recv_buf;
    iov.iov_len = buf_len;

    while (true)
    {
        status = recvmsg(ipsec_netlink->rth.fd, &msg, MSG_DONTWAIT);
        if (status < 0)
        {
            if (errno == EINTR)
                continue;
            if(errno != EAGAIN)
                LOG_ERROR("netlink receive error %s (%d)\n",strerror(errno), errno);
            return -1;
        }
        LOG_DEBUG("recv msg from pid:%d \n", nladdr.nl_pid);
        break;
    }
    if (status == 0) {
        LOG_ERROR("EOF on netlink\n");
        return -1;
    }
    return status;
}

int ipsec_netlink_init(struct ipsec_netlink_s* ipsec_netlink)
{
    memset(&ipsec_netlink->rth, 0, sizeof(ipsec_netlink->rth));
    ipsec_netlink->rth.fd = -1;

    return 0;
}

int ipsec_netlink_destroy(struct ipsec_netlink_s* ipsec_netlink)
{    
    if (ipsec_netlink->rth.fd >= 0) {
        close(ipsec_netlink->rth.fd);
        ipsec_netlink->rth.fd = -1;
    }

    return 0;
}
