#ifndef CTCPSERVER_H
#define CTCPSERVER_H

#include <QTcpServer>

class CTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    CTcpServer(QObject *parent = 0);

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

public slots:

};

#endif // CTCPSERVER_H
