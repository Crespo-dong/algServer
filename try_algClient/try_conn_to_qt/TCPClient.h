#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_
//����ͨ�ŵ�TCP�ͻ���
#include "wcdITimer.h"
#include <string>
using std::string;
#include <WINSOCK2.H>
#include <Windows.h>

class wcdNewTimer;
class wcdCThreadSync;
class FaceInfoGroup;
class FaceRecognizeResult;
class CFace;

class TCPClient : wcdITimer
{
public:
	//��ʼ������������������ļ����롢�׽��ֿ��ʼ�������ӵ�
	int init();

	//���ʹ�ʶ���������Ϣ��������
	int send(const FaceInfoGroup& faceInfoGroup);

	//���ʹ�ʶ���������Ϣ��CFace����������
	int send(CFace& face1, CFace& face2);

	//��������ʶ������������
	// resultΪ����������int���ؽ��յ������ݰ������ͻ򷵻ش���״̬
	int TCPClient::recv(FaceRecognizeResult& result);

	//�ͷ���Դ
	int final();

	TCPClient();
	~TCPClient();

private:
	SOCKET m_clientSocket;
	// ���ڻ������socket���͵Ļ�����
	wcdCThreadSync* m_lockSocket;
	int getConfig(string& stIP, int& nPort, bool& bSendHeartbeat);

	//����n���ֽڵ�����
	int sendn(SOCKET s, const char * buf, int len, int flags);
	//����n���ֽڵ�����
	int recvn(SOCKET s, char* buf, int len, int flags);

	wcdNewTimer* m_timer; // ��ʱ��
	bool m_bSendHeartbeat; //�Ƿ��������ź�
	static const int sm_nInterval = 5000; // �����ź�ʱ������ms��
	void TimerProc(); // ��ʱ����������������������
};
#endif // _TCPCLIENT_H_