
#ifndef __LOCK_H
#define __LOCK_H

class CMutexLock 
{
public:

    CMutexLock();

    ~CMutexLock();

    void lock();

    void unlock();

private:

    pthread_mutex_t m_mutex;
};

template <typename LOCK>
class FGuard
{
public:

    FGuard(LOCK& lock)
    :m_lock(lock)
    {
        m_lock.lock();
    }

    ~FGuard()
    {
        m_lock.unlock();
    }

private:

    FGuard(const FGuard&);

    FGuard& operator=(const FGuard&);

private:

    LOCK& m_lock;
};


typedef FGuard<CMutexLock> MutexGuard;

#endif
