#include "CFaceRecogTest.h"
#include "TSafeQueue.h"
#include "CFace.h"
#include "TCPClient.h"
#include "common.h"

CFaceRecogTest::CFaceRecogTest(TSafeQueue<pair<CFace, CFace>>& facePairQueue, TSafeQueue<FaceRecognizeResult>& resultSingleQueue)
	: m_facePairQueue(facePairQueue), m_resultSingleQueue(resultSingleQueue)
{
	m_pTcpClient = new TCPClient();
	m_pTcpClient->init();
}

CFaceRecogTest::~CFaceRecogTest()
{
	m_pTcpClient->final();
	delete m_pTcpClient;
}

void CFaceRecogTest::Run()
{
	pair<CFace, CFace> facePair;
	int nRet= m_facePairQueue.pop(facePair);
	if(nRet == 1)
	{
		int nRet = m_pTcpClient->send(facePair.first, facePair.second);
		if(nRet == 0) // ���ͳɹ�
		{
			FaceRecognizeResult result;
			int nRet = m_pTcpClient->recv(result);
			if(nRet >=0) // ���ܳɹ���������ΪRECOGNIZE_RESULT
			{
				// ���߳���resultSingleQueue�����м�������ʶ������ʶ�������ʱ�ŷ��룩�����̴߳Ӷ�����ȡ��ʶ����
				if(result.bIsSameFace)
				{
					m_resultSingleQueue.push(result);
				}
			}
		}
	}

}