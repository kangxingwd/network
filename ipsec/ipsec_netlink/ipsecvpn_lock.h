#ifndef _IPSECVPN_LOCK_
#define _IPSECVPN_LOCK_

#include <assert.h>
#include <pthread.h>

struct ipsecvpn_lock_s {
    pthread_mutex_t mutex;
};

//初始化锁
inline int ipsec_lock_init(ipsecvpn_lock_s *lock)
{
    int ret = 0;
    pthread_mutexattr_t attr;
    if(0 != (ret = pthread_mutexattr_init(&attr)))
    {
        return ret;
    }
    if(0 != (ret = pthread_mutexattr_setpshared(&attr, 1)))
    {
        pthread_mutexattr_destroy(&attr);
        return ret;
    }
    if(0 != (ret ==  pthread_mutex_init(&lock->mutex, &attr)))
    {
        pthread_mutexattr_destroy(&attr);
        return ret;
    }
    pthread_mutexattr_destroy(&attr);
    return 0;
}

//销毁锁
inline int ipsec_lock_destroy(ipsecvpn_lock_s *lock)
{
    assert(lock);
    int ret = pthread_mutex_destroy(&lock->mutex);
    return ret;
}

//加锁
inline int ipsec_lock_lock(ipsecvpn_lock_s *lock)
{
    assert(lock);
    int ret = pthread_mutex_lock(&lock->mutex);
    return ret;
}

//试图加锁
inline int ipsec_lock_trylock(ipsecvpn_lock_s *lock)
{
    assert(lock);
    int ret = pthread_mutex_trylock(&lock->mutex);
    return ret;
}

//解锁
inline int ipsec_lock_unlock(ipsecvpn_lock_s *lock)
{
    assert(lock);
    int ret = pthread_mutex_unlock(&lock->mutex);
    return ret;
}

#endif //IPSEC_LOCK

