#include "processor.h"
#include <QDebug>
#include <QThread>
#include <QImage>
#include <QString>
#include <QDateTime>
#include <QDir>
#include <iostream>
#include <QSqlQuery>

QString Processor::uploader = "anonymous";

Processor::Processor(QTcpSocket *socket,QObject *parent)
    :m_socket(socket), QObject(parent)
{
}

void Processor::work() {
    qDebug() << "current thread: " << QThread::currentThreadId();
    QByteArray by = m_socket->readAll();
    QByteArray prefix = by.left(4);
    if (prefix == "user") {
        //验证用户名和密码
        by.remove(0,4);
        userlst.push_back(QString(by));
        if (userlst.size() == 2) {
            QString username = userlst.at(0);
            QString password = userlst.at(1);
            QString queryinfo;
            QSqlQuery query;
            query.prepare("SELECT password FROM UserInfo WHERE username = (:username)");
            query.bindValue(":username",username);
            query.exec();
            if (query.next())
                queryinfo = query.value(0).toString();
            if (queryinfo == "") {
                QString message("1");
                m_socket->write(message.toUtf8());
                m_socket->flush();
            } else if (queryinfo != password) {
                QString message("2");
                m_socket->write(message.toLocal8Bit());
                m_socket->flush();
            } else {
                Processor::uploader = username;
                QString message("0");
                m_socket->write(message.toLocal8Bit());
                m_socket->flush();
            }
            userlst.clear();
        }
    } else {
        //处理图片
        qDebug() << "handle pictures";
        if (!hasReadHead) {
            imageData.clear();
//            QByteArray by = m_socket->readAll();
            qDebug() << "by size: " << by.length();
            avalibleNum = by.left(10).toLong();
            QByteArray test = by.left(30);
            QString str = QString(test);
            qDebug() << "30 left of array: " << str;
            by.remove(0,10);
            imageData.append(by);
            qDebug() << "imageData size: " << imageData.length();
            hasReadHead = true;
            if (imageData.length() >= avalibleNum) {
                handlePictures();
            }
            qDebug() << "have read head";
        } else {
//            QByteArray by = m_socket->readAll();
            qDebug() << "by size: " << by.length();
            imageData.append(by);
            if (imageData.length() >= avalibleNum) {
                handlePictures();
            }
            qDebug() << "dont have read head";
        }
        qDebug() << "avalibleNum: " << avalibleNum;
    }

}

void Processor::handlePictures() {
    qDebug() << "receive a image,length =" << avalibleNum;
    QImage image = QImage::fromData(imageData);
    QString imageName = QDateTime::currentDateTime().toString("yyMMddHHmmsszzz");
    QString fileName = "./Pics/"+imageName + ".jpg";
    qDebug() << "current path: " << QDir::currentPath();
    if (image.save(fileName))
        qDebug() << "save success";
    else
        qDebug() << "failed";
    hasReadHead = false;
    avalibleNum = 0;

    //插入数据库数据
    QString pictureName = imageName;
    QString pictureAddr = fileName;
    QString time = QDateTime::currentDateTime().toString("yyMMdd");
    QString flag = "0";
    QSqlQuery query;
    query.prepare("INSERT INTO PhotoInfo (pName,pAddr,pTime,pLoader,pFlag) "
                  "VALUES (?,?,?,?,?)");
    query.bindValue(0,pictureName);
    query.bindValue(1,pictureAddr);
    query.bindValue(2,time);
    query.bindValue(3,Processor::uploader);
    query.bindValue(4,flag);
    query.exec();
}
