#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QtNetwork>
#include <QTest>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    tcpSocket(new QTcpSocket(this)),
    networkSession(Q_NULLPTR),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    tcpSocket->connectToHost(QHostAddress::LocalHost,12345,QTcpSocket::ReadWrite);

    connect(ui->Connect_Button,SIGNAL(clicked(bool)),this,SLOT(connectToServer()));
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(tcpSocket,static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),this,&Dialog::displayError);
    connect(tcpSocket,&QIODevice::readyRead,this,&Dialog::getMessage);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        //获得已保存的网络配置
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        //如果尚未发现保存的网络配置，使用系统配置
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &Dialog::sessionOpened);

        networkSession->open();
    }
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::connectToServer() {
    QByteArray username = ui->username->text().toUtf8();
    QByteArray password = ui->password->text().toUtf8();
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText(QString::fromLocal8Bit("用户名或密码不能为空"));
        msgBox.exec();
    }
//    tcpSocket->abort();

    if (!tcpSocket->isWritable()) {
        QMessageBox msgBox;
        msgBox.setText(tr("something wrong with tcpsocket!"));
        msgBox.exec();
    }

    username.prepend("user");
    password.prepend("user");

    tcpSocket->write(username);
    tcpSocket->flush();
    QTest::qSleep(100);
    tcpSocket->write(password);
}

void Dialog::displayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }
}

void Dialog::sessionOpened() {
    //保存已经使用的配置
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();
}

void Dialog::getMessage() {
    QByteArray message = tcpSocket->readAll();
    qDebug() << "message: " << QString(message);
    if (message == "0") {
        qDebug() << "success";
        emit removeFrog();
        tcpSocket->disconnectFromHost();
        this->close();
    } else {
        ui->username->clear();
        ui->password->clear();
    }
    QMessageBox msgBox;
    if (message == "1")
        msgBox.setText(QString::fromLocal8Bit("用户名不存在，请重新输入！"));
    else if (message == "2")
        msgBox.setText(QString::fromLocal8Bit("密码错误，请重新输入！"));
    else if (message == "0")
        msgBox.setText(QString::fromLocal8Bit("连接成功！"));
    msgBox.exec();
}
