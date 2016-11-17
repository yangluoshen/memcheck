
#include <pthread.h>

#include "lock.h"

CMutexLock::CMutexLock()
{
    pthread_mutex_init(&this->m_mutex, NULL);
}

CMutexLock::~CMutexLock()
{
    pthread_mutex_destroy(&this->m_mutex);
}

void CMutexLock::lock()
{
    pthread_mutex_lock(&m_mutex);

    return;
}

void CMutexLock::unlock()
{
    pthread_mutex_unlock(&m_mutex);

    return;
}

/*
//template <typename LOCK>
//inline
FGuard::FGuard(CMutexLock& lock)
    :m_lock(lock)
{
    m_lock.lock();
}

//template <typename LOCK>
//inline
FGuard::~FGuard()
{
    m_lock.unlock();
}

*/
