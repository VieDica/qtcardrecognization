#include "cthread.h"
#include <QtNetwork>
#include <QDebug>


CThread::CThread(int socketDescriptor,QObject *parent)
    : QThread(parent),socketDescriptor(socketDescriptor)
{
}

void CThread::run() {
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }
    qDebug() << "run";
    QThread::run();
}
