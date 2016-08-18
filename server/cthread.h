#ifndef CTHREAD_H
#define CTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QListWidgetItem>

class CThread : public QThread
{
    Q_OBJECT

public:
    CThread(int socketDescriptor,QObject *parent);

    QTcpSocket tcpSocket;
    int socketDescriptor;

    void run() Q_DECL_OVERRIDE;

signals:
    void error(QTcpSocket::SocketError socketError);


};

#endif // CTHREAD_H
