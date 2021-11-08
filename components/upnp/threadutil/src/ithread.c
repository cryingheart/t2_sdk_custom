#include "ithread.h"
#include <kernel/os.h>

/****************************************************************************
//thread create funciton
//thread:handle of thread
//attr:attribution of thread, it maybe useless, alway NULL
//entry:function of thread
//arg:arguments of entry function
//return:return 0 if success, otherwise -1
***************************************************************************/
int8_t ithread_create(ithread_t * thread, ithread_attr_t *attr, start_routine entry, void * arg)
{
	//thread = os_create_thread(NULL, (os_entrypoint_t)entry, (os_threadarg_t)arg, OS_THREADPRI_LO|OS_CRTHREAD_DETACHED, 512);
	thread = os_create_thread(NULL, entry, NULL, OS_THREADPRI_LO|OS_CRTHREAD_DETACHED, 512);
	if(thread != NULL)
	{
	    return 0;
	}
		
	else
	{
	    return -1;
	}
		
}
//ithread_detach()//将新创建的线程与创建他的父线程进行分离
/****************************************************************************
//wait thread finish
//thread:handle of thread
//retval:return pointer which points to a pointer
***************************************************************************/
void ithread_join(ithread_t * thread, void **retval)
{
	*retval = os_join_thread(thread);
}
ithread_t * ithread_self(void)
{
	return os_self();
}
/****************************************************************************
//get the id of current thread
//return:id of current therad
***************************************************************************/
uint8_t ithread_get_current_thread_id(void)
{
	static uint8_t id = 1;
	if(id>=255)
	{
		id = 1;
	}
	else
	{
		id++;
	}
	return id;//该返回值后续用来做随机数产生器的seed，但是innophase没有给出产生线程id的函数，这里先随便给个数忽悠一下把
}
/****************************************************************************
//initialize a mutex
//mutex:a pointer which points to a mutex struct
//attr:a pointer which points to a attr struct, but useless
//return:0 if success, 1 if fail
***************************************************************************/
int ithread_mutex_init(ithread_mutex_t *mutex, void *attr)//
//ithread_mutex_t *ithread_mutex_init(void *attr)
{
	//int retCode = 0;
	//ithread_mutex_t *mutex=NULL;
	mutex->resources_counter = 1;//resources_counter表示资源数
	os_sem_init(&(mutex->sem), 1);//there is a resource
	//if((int)mutex->sem == 0)
	//	return 1;
	return 0;

}
/****************************************************************************
//lock a mutex
//mutex:a pointer which points to a mutex struct
//return:0 if success, 1 if fail
***************************************************************************/
int ithread_mutex_lock(ithread_mutex_t *mutex)//
{
	os_sem_wait(&(mutex->sem));
	mutex->resources_counter = 0;
	return 0;//先不管怎样，先返回正确码0
}
/****************************************************************************
//unlock a mutex
//mutex:a pointer which points to a mutex struct
***************************************************************************/
void ithread_mutex_unlock(ithread_mutex_t *mutex)//
{
	if(mutex->resources_counter == 0)
	{
		mutex->resources_counter = 1;
		os_sem_post(&(mutex->sem));
	}
	else//if counter >0
	{
		// do nothing
	}
	 
}
/****************************************************************************
//destroy mutex, release the memeory of mutex
//mutex:a pointer which points to a mutex struct
//return:0 if success, 1 if fail
***************************************************************************/
int ithread_mutex_destroy(ithread_mutex_t *mutex)//销毁该信号量
{
	os_free(mutex);
	return 0;//先不管怎样，先返回正确码0
}
/****************************************************************************
//initialize a condition
//cond:pointer which points to a cond struct
//attr:pointer which points to a attr struct
***************************************************************************/
int ithread_cond_init(ithread_cond_t *cond, void *attr)//条件也用信号量来做，搞一个结构体有sem和wait_threads_num，正在等这个信号量的线程数
{
	//int retCode = 0;
	cond->waiting_threads_counter = 0;
	os_sem_init(&(cond->sem), 0);
	//if(cond->sem == 0)
	//	return 1;
	return 0;
}
/****************************************************************************
//send a cond signal to release a thread which is waiting for this cond
//cond:pointer which points to a cond struct
***************************************************************************/
void ithread_cond_signal(ithread_cond_t *cond)//调用sem_post,并且信号量对应线程计数器加1，释放掉一个正在阻塞的线程
{
	os_sem_post(&(cond->sem));
}
/****************************************************************************
//broadcast signals to release all pending thread which is waiting for this cond
//cond:pointer which points to a cond struct
***************************************************************************/
void ithread_cond_broadcast(ithread_cond_t *cond)//多次调用sem_post，并将信号量的线程计数器减为0
{
	int waiting_threads_counter = cond->waiting_threads_counter;
	for (int i = 0; i < waiting_threads_counter; i++)
	{
		os_sem_post(&(cond->sem));
	}
}
/****************************************************************************
//wait for a cond, and relese mutex
//cond:pointer which points to a cond struct
//mutex:a pointer which points to a mutex struct
//NOTE：调用ithread_cond_wait之前必须已经调用了ithread_mutex_unlock锁住了mutex
***************************************************************************/
void ithread_cond_wait(ithread_cond_t *cond, ithread_mutex_t *mutex)//请求cond，同时释放mutex
{
	ithread_mutex_unlock(mutex);//释放mutex
	
	cond->waiting_threads_counter += 1;
	os_sem_wait(&(cond->sem));
	cond->waiting_threads_counter -= 1;//
	
	ithread_mutex_lock(mutex);//锁住mutex
	
}
/****************************************************************************
//wait for a cond until timeout, and relese mutex
//cond:pointer which points to a cond struct
//mutex:a pointer which points to a mutex struct
//timeout:a pointer which points to a timespec struct
//return:0 if success, -1 if fail
//NOTE：调用ithread_cond_timedwait之前必须已经调用了ithread_mutex_unlock锁住了mutex
***************************************************************************/
int ithread_cond_timedwait(ithread_cond_t *cond, ithread_mutex_t *mutex, struct timespec *timeout)//等待cond，释放mutex
{
	int retCode = -1;
	ithread_mutex_unlock(mutex);//释放mutex
	
	uint32_t tmo = timeout->tv_nsec + 1000 * timeout->tv_sec;
	cond->waiting_threads_counter += 1;
	retCode = os_sem_wait_timeout(&(cond->sem), tmo);
	cond->waiting_threads_counter -= 1;//
	
	ithread_mutex_lock(mutex);//锁住mutex
	return retCode;
}
/****************************************************************************
//destroy a cond
//cond:pointer which points to a cond struct
//return:0 if success
***************************************************************************/
int ithread_cond_destroy(ithread_cond_t *cond)//销毁对应信号量
{
	os_free(cond);
	return 0;//先不管怎样，先返回正确码0
}
/****************************************************************************
//pending current thread for sec
//sec:during time of pending
***************************************************************************/
void isleep(uint16_t sec)
{
	os_msleep(sec*1000);
}
