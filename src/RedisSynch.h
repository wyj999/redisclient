#ifndef REDIS_SYNCH_H
#define REDIS_SYNCH_H

#include <assert.h>
#include <pthread.h>
#include <errno.h>


class MutexLock
{
    public:
        MutexLock()
        {
            int ret = pthread_mutex_init(&mutex_, NULL);
            assert(ret == 0); (void) ret;
        }

        ~MutexLock()
        {
            int ret = pthread_mutex_destroy(&mutex_);
            assert(ret == 0); (void) ret;
        }

        void lock()
        {
            pthread_mutex_lock(&mutex_);
        }

        void unlock()
        {
            pthread_mutex_unlock(&mutex_);
        }

        pthread_mutex_t* getPthreadMutex() /* non-const */
        {
            return &mutex_;
        }

    private:
        pthread_mutex_t mutex_;
};

class MutexLockGuard
{
    public:
        explicit MutexLockGuard(MutexLock& mutex)
            : mutex_(mutex)
        {
            mutex_.lock();
        }

        ~MutexLockGuard()
        {
            mutex_.unlock();
        }

    private:
        MutexLock& mutex_;
};

class Condition 
{
    public:
        explicit Condition(MutexLock& mutex)
            : mutex_(mutex)
        {
            pthread_cond_init(&pcond_, NULL);
        }

        ~Condition()
        {
            pthread_cond_destroy(&pcond_);
        }

        void wait()
        {
            pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
        }

        // returns true if time out, false otherwise.
        bool waitForSeconds(int seconds)
        {
            struct timespec abstime;
            clock_gettime(CLOCK_REALTIME, &abstime);
            abstime.tv_sec += seconds;
            return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);
        }

        void notify()
        {
            pthread_cond_signal(&pcond_);
        }

        void notifyAll()
        {
            pthread_cond_broadcast(&pcond_);
        }

    private:
        MutexLock& mutex_;
        pthread_cond_t pcond_;
};


#endif // REDIS_SYNCH_H
