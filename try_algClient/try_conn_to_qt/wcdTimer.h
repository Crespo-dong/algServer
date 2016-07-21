#ifndef _WCDNEWTIMER_H_
#define _WCDNEWTIMER_H_

#include "wcdITimer.h"
#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
#include <windows.h>
#pragma comment(lib,"Winmm.lib")
using namespace std;


# define ONE_MILLI_SECOND 1 //����1ms��2sʱ�Ӽ������msΪ��λ ��
# define TWO_SECOND 2000 
# define FIVE_SECOND 5000 
# define TIMER_ACCURACY 1 //����ʱ�ӷֱ��ʣ���msΪ��λ 

class wcdNewTimer
{
public:
	// ����ʱ����ʱ�������Ժ���Ϊ��λ��
	wcdNewTimer(int nInterval);
	// ����ʱ����ʱ�������Ժ���Ϊ��λ����ͬʱ����ʵ��wcdITimer�ӿڵĶ���
	// �ص�ʱ������iTimer�����TimerProc0����
	wcdNewTimer(int nInterval, wcdITimer* iTimer);
	~wcdNewTimer();
	void StartTimer();
	void StopTimer();

private:
	UINT wTimerRes; //����ʱ���� 
	UINT wAccuracy; //����ֱ��� 
	UINT TimerID; //���嶨ʱ�����
	wcdITimer* m_wcdITimer;

	// ��ʱ���Ļص��������������жϴ������һ��Ҫ����Ϊȫ��PASCAL������
	// �������������� 
	static void PASCAL TimerProc0(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2);
	virtual void TimerProc();
};

wcdNewTimer::wcdNewTimer(int nInterval)
{
	m_wcdITimer = NULL;
	// ��ʱ����������ֵ 
	//wTimerRes = FIVE_SECOND;
	wTimerRes = nInterval;
	TIMECAPS tc;
	//���ú���timeGetDeVCapsȡ��ϵͳ�ֱ��ʵ�ȡֵ��Χ������޴������
	if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR) 
	{
		//�ֱ��ʵ�ֵ���ܳ���ϵͳ��ȡֵ��Χ
		wAccuracy=min(max(tc.wPeriodMin,
			TIMER_ACCURACY),tc.wPeriodMax); 
		//����timeBeginPeriod�������ö�ʱ���ķֱ��� 
		timeBeginPeriod(wAccuracy); 
	}
}

wcdNewTimer::wcdNewTimer(int nInterval, wcdITimer* iTimer)
{
	//wcdNewTimer(nInterval);
	// C++������������һ�����캯��
	new(this) wcdNewTimer(nInterval);
	m_wcdITimer = iTimer;
}

wcdNewTimer::~wcdNewTimer() 
{
}

// ��װ��ʱ�� 
void wcdNewTimer::StartTimer()
{ 
	if((TimerID = timeSetEvent(wTimerRes, wAccuracy, 
		TimerProc0, // �ص������� 
		(DWORD)this,// �û����͵��ص����������ݣ�
		TIME_PERIODIC)) == 0)//���ڵ��ö�ʱ��������
	{ 
		MessageBox(NULL,TEXT("���ܽ��ж�ʱ��"),TEXT("ERROR"),MB_OK);
	}
	else 
		cout << "��װ��ʱ���ɹ�" << endl; //������0������װ�ɹ������ش˶�ʱ���ľ����
}

void wcdNewTimer::StopTimer()
{
	cout <<"����ʱ��"<< endl; //����ʱ�� 
	timeKillEvent(TimerID); // ɾ��������ʱ��
	timeEndPeriod(wAccuracy);  // ɾ�����õķֱ���
}


// ��ʱ���Ļص��������������жϴ������һ��Ҫ����Ϊȫ��PASCAL������
// �������������� 
void PASCAL wcdNewTimer::TimerProc0(UINT wTimerID, UINT msg,DWORD dwUser,
	DWORD dwl,DWORD dw2)
{
	wcdNewTimer* timer = (wcdNewTimer*)dwUser;
	timer->TimerProc();
}

void wcdNewTimer::TimerProc()
{
	if(m_wcdITimer != NULL)
	{
		m_wcdITimer->TimerProc();
	}
}

#endif // _WCDNEWTIMER_H_