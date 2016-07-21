#include "threadpool.h"
#include "CIRunnable.h"
#include <stdlib.h>
#include <stdio.h>
//#include <strinq.h>
#include <errno.h>
#include <time.h>
#include <Windows.h>

// ��linux��clock_gettime����ֲ
int threadpool::clock_gettime(int, struct timespec *spec)
{  
	__int64 wintime;
	GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime      -=116444736000000000i64;  //1jan1601 to 1jan1970
	spec->tv_sec  =wintime / 10000000i64;           //seconds
	spec->tv_nsec =wintime % 10000000i64 *100;      //nano-seconds
	return 0;
}

void* threadpool::thread_routine(void *arg)
{
	struct timespec abstime;
	int timeout;
	//printf("thread 0x%x is starting\n", (int)pthread_self());
	printf("thread 0x%x is starting\n", ::GetCurrentThreadId());
	threadpool *pool = (threadpool*)arg;
	while(1)
	{
		timeout = 0;
		condition_lock(&pool->ready);
		pool->idle++;
		// �ȴ������������������̳߳�����֪ͨ
		while(pool->tastQueue.empty() && !pool->quit)
		{
			//printf("thread 0x%x is waiting\n", (int)pthread_self());
			printf("thread 0x%x is waiting\n", ::GetCurrentThreadId());
			if(timewait_sec == -1)
			{
				//���õȴ�
				condition_wait(&pool->ready);
			}
			else
			{
				//��ʱ�ȴ�
				//clock_gettime(CLOCK_REALTIME, &abstime);
				clock_gettime(0, &abstime);
				//�ȴ�2��
				//abstime.tv_sec += 2;
				abstime.tv_sec += timewait_sec;
				int status = condition_timewait(&pool->ready, &abstime);
				if(status == ETIMEDOUT)
				{
					//printf("thread 0x%x is wait timed out\n", (int)pthread_self());
					printf("thread 0x%x is wait timed out\n", ::GetCurrentThreadId());
					timeout = 1;
					break;
				}
			}
		}

		// �ȴ������������ڹ���״̬
		pool->idle--;
		
		// ����ȴ����̳߳�����֪ͨ
		if(pool->quit)
		{
			// �����ǰ���������
			while(!pool->tastQueue.empty())
			{
				task* t = pool->tastQueue.front();
				pool->tastQueue.pop();
				//ɾ������ʱ������Ҫ�ͷ�arg������CRunnable�ӿ�ָ��ָ��Ķ������ͷ�task
				delete t->arg;
				delete t->m_runnable;
				delete t;
			}
			pool->counter--;
			if(pool->counter == 0)
				condition_signal(&pool->ready);

			condition_unlock(&pool->ready);
			// ����ѭ��֮ǰҪ�ǵý���
			break;
		}
		
		// �ȴ�������
		if(!pool->tastQueue.empty())
		{
			//�Ӷ�ͷȡ������
			task* t = pool->tastQueue.front();
			pool->tastQueue.pop();
			// ִ��������Ҫһ����ʱ�䣬����Ҫ�Ƚ������Ա��������߳�
			// �ܹ���������������������������߳��ܹ�����ȴ�����
			condition_unlock(&pool->ready);
			t->m_runnable->Run(t->arg);
			//ִ��������Ҫ�ͷ�arg������CRunnable�ӿ�ָ��ָ��Ķ������ͷ�task
			delete t->arg;
			delete t->m_runnable;
			delete t;
			condition_lock(&pool->ready);
		}
		/*
		// ����ȴ����̳߳�����֪ͨ,������ִ�����
		if(pool->quit && pool->tastQueue.empty())
		{
			pool->counter--;
			if(pool->counter == 0)
				condition_signal(&pool->ready);

			condition_unlock(&pool->ready);
			// ����ѭ��֮ǰҪ�ǵý���
			break;
		}
		*/
		
		// ����ȴ���ʱ,������ִ�����
		if(timeout && pool->tastQueue.empty())
		{
			pool->counter--;
			condition_unlock(&pool->ready);
			// ����ѭ��֮ǰҪ�ǵý���
			break;
		}
		condition_unlock(&pool->ready);
	}

	//printf("thread 0x%x is exiting\n", (int)pthread_self());
	printf("thread 0x%x is exiting\n", ::GetCurrentThreadId());
	return NULL;
}

// ��ʼ���̳߳�
void threadpool::threadpool_init(int threads, int queueLen)
{
	//�����ֶγ�ʼ��
	condition_init(&this->ready);
	this->counter = 0;
	this->idle = 0;
	this->max_threads = threads;
	this->max_queueLen = queueLen;
	this->quit = 0;
}

// ���̳߳����������
void threadpool::thread_add_task(CIRunnable* runnable, void *arg)
{
	// ����������
	//task_t *newtask = (task_t *)malloc(sizeof(task_t));
	task* newtask = new task();
	newtask->m_runnable = runnable;
	newtask->arg = arg;

	condition_lock(&this->ready);
	/*
	// ��������ӵ�����
	this->tastQueue.push(newtask);
	*/
	//�ĳɣ�������г��ȳ�����󳤶�ʱ��ȥ����ͷ������
	if((int)tastQueue.size() < max_queueLen)
	{
		tastQueue.push(newtask);
	}
	else
	{
		task* t = tastQueue.front();
		//ɾ������ʱ��Ҫ�ͷ�arg������CRunnable�ӿ�ָ��ָ��Ķ������ͷ�task
		delete t->m_runnable;
		delete t;
		tastQueue.pop();
		tastQueue.push(newtask);
	}

	//����еȴ��̣߳���������һ��
	if(this->idle > 0)
		condition_signal(&this->ready);
	else if (this->counter < this->max_threads)
	{
		// û�еȴ��̣߳����ҵ�ǰ�߳�������������߳������򴴽�һ�����߳�
		pthread_t tid;
		pthread_create(&tid, NULL, thread_routine, this);
		this->counter++;
	}

	condition_unlock(&this->ready);
}

// �����̳߳�
void threadpool::threadpool_destory()
{
	if(this->quit)
	{
		return;
	}
	condition_lock(&this->ready);
	this->quit = 1;
	if(this->counter > 0)
	{
		if(this->idle > 0)
			condition_broadcast(&this->ready);

		// ����ִ������״̬�е��̣߳������յ��㲥
		// �̳߳���Ҫ�ȴ�ִ������״̬�е��߳�ȫ���˳�

		while(this->counter > 0)
			condition_wait(&this->ready);
	}
	condition_unlock(&this->ready);
	condition_destory(&this->ready);
}