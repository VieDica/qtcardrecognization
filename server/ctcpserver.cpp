#include "ctcpserver.h"
#include "cthread.h"
#include "processor.h"
#include <QDebug>
#include <QMessageBox>
#include <QTcpSocket>

CTcpServer::CTcpServer(QObject *parent)
    : QTcpServer(parent)
{

}

void CTcpServer::incomingConnection(qintptr socketDescriptor) {
    qDebug() << "socketDescriptor: " << socketDescriptor;
    CThread *thread = new CThread(socketDescriptor,this);
    Processor *cpu = new Processor(&thread->tcpSocket);
    connect(&thread->tcpSocket,SIGNAL(readyRead()),cpu,SLOT(work()));
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    cpu->moveToThread(thread);
    thread->start();
}

