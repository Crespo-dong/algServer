#include "TCPClient.h"
#pragma comment(lib,"ws2_32.lib")
#include "Config.h"
#include "wcdTimer.h"
#include "wcdThreadSync.h"
#include "common.h"
#include "CFace.h"
#include <string>
#include <iostream>
using namespace std;

TCPClient::TCPClient()
{
	m_timer = NULL;
	m_lockSocket = new wcdCThreadSync();
};

TCPClient::~TCPClient()
{
	if(m_timer != NULL)
	{
		delete m_timer;
	}
	delete m_lockSocket;
};

//��ʼ������������������ļ����롢�׽��ֿ��ʼ�������ӵ�
int TCPClient::init()
{
	int err;
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired=MAKEWORD(1,1);
	err=WSAStartup(versionRequired,&wsaData);//Э���İ汾��Ϣ
	if (!err)
	{
		cout<<"�ͻ���Ƕ�����Ѿ���!"<<endl;
	}
	else
	{
		cout<<"�ͻ��˵�Ƕ���ִ�ʧ��!"<<endl;
		return  -1;
	}

	//��ȡ�����ļ�����ȡIP��ַ���˿ں����Ƿ��������ź�
	string stIP;
	int nPort;
	getConfig(stIP, nPort, m_bSendHeartbeat);
	cout<<"IP: "<<stIP<<endl;
	cout<<"port: "<<nPort<<endl;
	cout<<"m_bSendHeartbeat: "<<m_bSendHeartbeat<<endl;
	if(m_bSendHeartbeat == true)
	{
		m_timer = new wcdNewTimer(sm_nInterval,this);
		m_timer->StartTimer();
	}

	m_clientSocket=socket(AF_INET,SOCK_STREAM,0);
	/*
	// ��ֹ�������ݹ��̶�������������������
	//��sock_cli����KEEPALIVE��NODELAY
	int nFlag = 1;
	int nErr = setsockopt(m_clientSocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&nFlag, sizeof(int));//ʹ��KEEPALIVE
	if(nErr == -1)
	{
		cout<<"setsockopt() error : "<<WSAGetLastError()<<endl;
		return -2;
	}
	nErr = setsockopt(m_clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&nFlag, sizeof(int));//����NAGLE�㷨
	if(nErr == -1)
	{
		cout<<"setsockopt() error : "<<WSAGetLastError()<<endl;
		return -2;
	}
	*/
	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr.S_un.S_addr=inet_addr(stIP.c_str());
	clientsock_in.sin_family=AF_INET;
	clientsock_in.sin_port=htons(nPort);
	if(SOCKET_ERROR == connect(m_clientSocket,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR)))//��ʼ����
	{
		cout<<"����ʧ��"<<endl;
		return -3;
	}

	return 0;
}

int TCPClient::getConfig(string& stIP, int& nPort, bool& bSendHeartbeat)
{
	
	Config configSettings("algorithmServerConfig.ini");
	stIP=configSettings.Read<string>("ServerIP");
	nPort=configSettings.Read<int>("Port");
	string stSendHeartbeat=configSettings.Read<string>("SendHeartbeat");
	if(stSendHeartbeat == "true")
	{
		bSendHeartbeat = true;
	}
	else
	{
		bSendHeartbeat = false;
	}
	return 0;
}

//����n���ֽڵ�����
int TCPClient::recvn(SOCKET s, char* buf, int len, int flags)
{
	int nLeft = len;
	int nRead;
	char* bufp = buf;

	while(nLeft > 0)
	{
		if(nRead = ::recv(s, bufp, nLeft, 0) < 0)
		{
			cout<<"��������ʧ��"<<endl;
			return -1;
		}
		else if(nRead == 0)
		{
			//cout<<"nRead == 0"<<endl;
			return len - nRead;
		}
		bufp += nRead;
		nLeft -= nRead;
	}

	return len;
}

//����n���ֽڵ�����
int TCPClient::sendn(SOCKET s, const char * buf, int len, int flags)
{
	int nLeft = len;
	int nWritten;
	char* bufp = (char*)buf;

	while(nLeft > 0)
	{
		if(nWritten = ::send(s, bufp, nLeft, 0) < 0)
		{
			cout<<"��������ʧ��"<<endl;
			return -1;
		}
		else if(nWritten == 0)
		{
			//cout<<"nWritten == 0"<<endl;
			return len - nWritten;
		}
		bufp += nWritten;
		nLeft -= nWritten;
	}

	return len;
}

//���ʹ�ʶ���������Ϣ��FaceInfoGroup����������
int TCPClient::send(const FaceInfoGroup& faceInfoGroup)
{
	char* pDataBlock = NULL;
	int nDataBlockLen;
	FaceInfoGroup::ObjectToDataBlock(faceInfoGroup, pDataBlock, nDataBlockLen);
	int nPackDataLen = nDataBlockLen + 4;
	char* pSendBuf = new char[4 + nPackDataLen];
	// ������ת��Ϊ�����ֽ��򣬱��ڷ����ʹ��QDataStreamֱ�Ӷ�ȡ
	// int nDataLenNet = htonl(nDataLen);
	//memcpy(pSendBuf, &nDataLenNet, sizeof(int));
	memcpy(pSendBuf, &nPackDataLen, sizeof(int));
	int nType = FACEGROUP; // FACEGROUP���͵����ݰ�
	memcpy(pSendBuf+4, &nType, sizeof(int));
	memcpy(pSendBuf+8, pDataBlock, nDataBlockLen);
	m_lockSocket->Lock();
	int ret = sendn(m_clientSocket, pSendBuf, 4+nPackDataLen, 0);
	m_lockSocket->Unlock();
	delete[] pSendBuf;
	if(ret == 0)
	{
		cout<<"client close"<<endl;
		return -2;
	}
	else if(ret == -1) {
		cout<<"send error"<<endl;
		return -1;
	}
	cout<<"send data success"<<endl;
	return 0;
}

//���ʹ�ʶ���������Ϣ��CFace����������
int TCPClient::send(CFace& face1, CFace& face2)
{
	FaceInfo faceInfo1 = CFace::CFaceToFaceInfo(face1);
	FaceInfo faceInfo2 = CFace::CFaceToFaceInfo(face2);
	FaceInfoGroup faceInfoGroup;
	faceInfoGroup.faceInfoArr[0] = faceInfo1;
	faceInfoGroup.faceInfoArr[1] = faceInfo2;
	return send(faceInfoGroup);
}

// ��������ʶ������������
// resultΪ����������int���ؽ��յ������ݰ������ͻ򷵻ش���״̬
int TCPClient::recv(FaceRecognizeResult& result)
{
	//int nDatalenNet = 0;
	int nPackDatalen = 0;
	int ret = recvn(m_clientSocket, (char*)&nPackDatalen, sizeof(int), 0);
	if(ret == -1)
	{
		cout<<"recv error"<<endl;
		return -1;
	}
	else if (ret < sizeof(int))
	{
		cout<<"client close"<<endl;
		return -2;
	}
	// ��qt����˽��յ��ĳ��ȣ���Ҫת���ֽ���
	//int nDatalen = ntohl(nDatalenNet);
	char* pDataBlock = new char[nPackDatalen-4];
	ret = recvn(m_clientSocket, pDataBlock, nPackDatalen, 0);
	if(ret == -1)
	{
		cout<<"recv error"<<endl;
		return -1;
	}
	else if(ret < nPackDatalen)
	{
		cout<<"client close"<<endl;
		return -2;
	}
	cout<<"recv data success"<<endl;

	int nType = *((int*)pDataBlock);
	if(nType == RECOGNIZE_RESULT)
	{
		int nBlockDataLen;
		result = FaceRecognizeResult::DataBlockToObject(pDataBlock+4, nBlockDataLen);
	}
	return nType;
}

 // ��ʱ����������������������
void TCPClient::TimerProc()
{
	string stMessage = "running\n";
	int nBlockDataLen = stMessage.length()+1;
	int nPackDataLen = nBlockDataLen + 4;
	char* pSendBuf = new char[4+nPackDataLen];
	// ������ת��Ϊ�����ֽ��򣬱��ڷ����ʹ��QDataStreamֱ�Ӷ�ȡ
	// int nDataLenNet = htonl(nDataLen);
	//memcpy(pSendBuf, &nDataLenNet, sizeof(int));
	memcpy(pSendBuf, &nPackDataLen, sizeof(int));
	int nType = HEARTBEAT; // FACEGROUP���͵����ݰ�
	memcpy(pSendBuf+4, &nType, sizeof(int));
	memcpy(pSendBuf+8, stMessage.c_str(), nBlockDataLen);
	m_lockSocket->Lock();
	int ret = sendn(m_clientSocket, pSendBuf, 4+nPackDataLen, 0);
	m_lockSocket->Unlock();
	delete[] pSendBuf;
}

//�ͷ���Դ
int TCPClient::final()
{
	m_timer->StopTimer();
	closesocket(m_clientSocket);
	WSACleanup();
	return 0;
}