#ifndef _CIFACERECOG_H_
#define _CIFACERECOG_H_
#include "CIRunnable.h"
#include "common.h"
#include <QString>

class CFaceParam;

class CIFaceRecog : public CIRunnable
{
public:
	// �̳߳ص�ʹ���߽�ʵ��CIFaceRecog�ӿڵĶ��󣨷�װ�ص���������CFaceParam*���͵Ķ��󣨷�װ�����������̳߳��У�
	// ʵ��CRunnable�ӿڵ����Run������Ҫ��paramָ��������ȡ��������ǿ������ת��ΪCFaceParam*���͡�
	// ʹ����������ͷŲ��������̳߳��ͷŲ���
	void* Run(void* param);
	virtual ~CIFaceRecog();
private:
	QString generateRecogMessage(bool bIsSameFace, int& nConfidence);
	virtual int recognize(CFace& face1, CFace& face2, bool& bIsSameFace, int& nConfidence) = 0;
};

#endif // _CIFACERECOG_H_