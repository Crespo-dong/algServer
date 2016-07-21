#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
/*
2016.04.27
	����C���԰��̳߳�
	ÿ�������Ӧһ��ʵ��CRunnable�ӿڵĶ������ڷ�װ�ص���������һ��arg����
	����ִ����ϻ�����������ɾ������ʱ��Ҫ�ͷ�arg������CRunnable�ӿ�ָ��ָ��Ķ������ͷ�task
	����max_queueLen��Ա(����������Ŷӵ��������)��������г��ȳ�����󳤶�ʱ��ȥ����ͷ������
	�����̳߳�ʱ��ɾ����������е�ʣ������
*/
#include "condition.h"
#include <queue>
using std::queue;

//����ṹ�壬���������������̳߳��е��߳���ִ��
/*
typedef struct task
{
	void *(*run)(void* args);	//����ص�����
	void *arg;	// �ص���������
	struct task *next;
} task_t;
*/
class CIRunnable;

// �̳߳ؽṹ��
class threadpool
{
private:
	struct task
	{
		CIRunnable* m_runnable; // ��װ�ص������Ľӿ�
		void *arg;	// �ص���������
	};

	condition::condition_t ready;	//����׼�����������̳߳�����֪ͨ
	queue<task*> tastQueue; // �������
	/*
	task_t *first;	//�������ͷָ��
	task_t *last;	//�������βָ��
	*/
	int counter;	//�̳߳��е�ǰ�߳���
	int idle;	//�̳߳��е�ǰ���ڵȴ�������߳���
	int max_threads;	//�̳߳������������߳���
	int max_queueLen;	//����������Ŷӵ��������
	int quit;	//�����̳߳ص�ʱ����1
	static const int timewait_sec = -1; // �ȴ�����ĳ�ʱʱ�䣨�룩������Ϊ-1��ʾ���õȴ�

	// ��linux��clock_gettime����ֲ
	static int threadpool::clock_gettime(int, struct timespec *spec);

	static void* thread_routine(void *arg);

public:

	//��ʼ���̳߳�
	void threadpool_init(int threads, int queueLen);
	// ���̳߳����������
	void thread_add_task(CIRunnable* runnable, void *arg);
	//�����̳߳�
	void threadpool_destory();
};



#endif //_THREAD_POOL_H_