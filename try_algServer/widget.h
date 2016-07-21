#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMutex>
class TCPServer;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
	void writeToEditor(QString& qstr); // ��textEditд����

private:
    Ui::Widget *ui;
	TCPServer* m_tcpServer;
	QMutex m_mutex; // ������textEditд����ʱ������

};

#endif // WIDGET_H
