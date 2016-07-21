#ifndef _CFACERECOGTEST_H_
#define _CFACERECOGTEST_H_

// �˺�������ֹWindows.h��winsock.h�İ������Ա���
// ��TCPClient.h�е�WINSOCK2.H������ͻ
#define WIN32_LEAN_AND_MEAN
#include "wcdThread.h"
#include <xutility>
using std::pair;

template <class T>
class TSafeQueue;
class CFace;
class FaceRecognizeResult;
class TCPClient;

class CFaceRecogTest : public wcdCThread
{
public:
	CFaceRecogTest(TSafeQueue<pair<CFace, CFace>>& facePairQueue, TSafeQueue<FaceRecognizeResult>& resultSingleQueue);
	~CFaceRecogTest();
	void Run();
private:
	TSafeQueue<pair<CFace, CFace>>& m_facePairQueue;
	TSafeQueue<FaceRecognizeResult>& m_resultSingleQueue;
	TCPClient* m_pTcpClient;
};

#endif // _CFACERECOGTEST_H_