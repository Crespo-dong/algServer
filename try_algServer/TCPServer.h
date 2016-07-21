#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <string>
using std::string;

class Widget;
class threadpool;

class TCPServer : public QTcpServer
{
	Q_OBJECT

public:
	TCPServer(QObject *parent = 0);
	~TCPServer();
	int listen(); // �����˿�

private:
	Widget *m_widget; // ����ʱ���洰����ָ�룬ͬ�ڷ��ʴ������writeToEditor����
	threadpool* m_pool; // ִ������ʶ��������̳߳�
	void incomingConnection(int socketId);	
	int getIPAndPort(string& stIP, int& nPort);
	
};

#endif
