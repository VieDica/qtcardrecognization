#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>
#include <QTcpSocket>
#include <QStringList>

class Processor : public QObject
{
    Q_OBJECT
public:
    Processor(QTcpSocket *socket,QObject *parent = 0);
    ~Processor() { delete m_socket; }

    QTcpSocket *m_socket;
    QByteArray imageData;
    QStringList userlst;
    static QString uploader;

    bool hasReadHead = false;
    long avalibleNum;

    //write picture info into database
    void handlePictures();

public slots:
    void work();
};

#endif // PROCESSOR_H
