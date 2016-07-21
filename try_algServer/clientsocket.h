#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>
#include <opencv2/core/core.hpp>
using cv::Mat;
#include <string>
using std::string;

class TCPServer;
class Widget;
class FaceInfo;
class FaceInfoGroup;
class FaceRecognizeResult;
class threadpool;

class QDate;
class QTime;

class ClientSocket : public QTcpSocket
{
    Q_OBJECT

public:
	ClientSocket(QObject *parent = 0, Widget *widget = NULL, threadpool* pool = NULL);
	~ClientSocket();
	void sendResult(FaceRecognizeResult& result);

private slots:
    void readClient();

private:
    void generateRandomTrip(const QString &from, const QString &to,
                            const QDate &date, const QTime &time);
	QString generateRecvMessage(FaceInfoGroup& faceInfoGroup);
	QString generateRecvMessage(string& stHeartbeatMessage);
	// ����FaceInfo�ָ���ͼ��
	Mat ParseImageFromFaceInfo(const FaceInfo& faceInfo);
	void doService(FaceInfoGroup& faceInfoGroup);

	Widget *m_widget; // ����ʱ���洰����ָ�루��TCPServer�ഫ�����������ڷ��ʴ�����writeToEditor����
	threadpool* m_pool; // ����ʱ�����̳߳�ָ�루��threadpool�ഫ�����������ڷ����̳߳����thread_add_task����
    int nPackDataLen;
};

#endif
