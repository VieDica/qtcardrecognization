#include "client.h"
#include "ui_client.h"
#include <QFileDialog>
#include <QDebug>
#include <QDateTime>
#include <QtNetwork>
#include <QtWidgets>
#include <QMessageBox>
#include <QDataStream>
#include <QTest>

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    tcpSocket(new QTcpSocket(this)),
    networkSession(Q_NULLPTR),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    this->show();

    Dialog *d = new Dialog;
    connect(d,SIGNAL(removeFrog()),this,SLOT(removeFrog()));
    d->exec();

    tcpSocket->connectToHost(QHostAddress::LocalHost,12345,QTcpSocket::ReadWrite);
    tcpSocket->waitForBytesWritten(30000);

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
        connect(networkSession, &QNetworkSession::opened, this, &Client::sessionOpened);

        networkSession->open();
    }

    connect(ui->Open_Button,SIGNAL(clicked(bool)),this,SLOT(openFile()));
    connect(this,SIGNAL(getPicNames(QStringList)),this,SLOT(display(QStringList)));

    connect(ui->Allpick_Button,SIGNAL(clicked(bool)),this,SLOT(allPick()));
    connect(ui->Reversepick_Button,SIGNAL(clicked(bool)),this,SLOT(reversePick()));
    connect(ui->Upload_Button,SIGNAL(clicked(bool)),this,SLOT(transportPictures()));
    connect(ui->Clear_Button,SIGNAL(clicked(bool)),this,SLOT(removeAll()));
}

Client::~Client()
{
    delete ui;
}

void Client::openFile() {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    emit getPicNames(fileNames);
}

void Client::display(QStringList fileNames) {
    if (!fileNames.isEmpty()) {
        ui->Pics_List->setIconSize(QSize(200,200));
        ui->Pics_List->setResizeMode(QListView::Adjust);
        ui->Pics_List->setViewMode(QListView::IconMode);
        ui->Pics_List->setMovement(QListView::Static);
        ui->Pics_List->setSpacing(10);
        ui->Pics_List->setSelectionMode(QListView::MultiSelection);
        for (auto s : fileNames) {
            qDebug() << s;
            QPixmap pic(s);
            QListWidgetItem *item = new QListWidgetItem(QIcon(pic.scaled(QSize(100,100))),s);
            item->setSizeHint(QSize(200,210));
            ui->Pics_List->addItem(item);
        }
    }
}

void Client::allPick() {
    int count = ui->Pics_List->count();
    if (count == 0) {
        QMessageBox msgBox;
        msgBox.setText(QString::fromLocal8Bit("没有图片可以选择！"));
        msgBox.exec();
    }
    for (int i = 0;i < count;i++) {
        QListWidgetItem *item = ui->Pics_List->item(i);
        item->setSelected(true);
    }
}

void Client::reversePick() {
    int count = ui->Pics_List->count();
    if (count == 0) {
        QMessageBox msgBox;
        msgBox.setText(QString::fromLocal8Bit("没有图片被选中！"));
        msgBox.exec();
    }
    for (int i = 0;i < count;i++) {
        QListWidgetItem *item = ui->Pics_List->item(i);
        item->setSelected(false);
    }
}

void Client::removeAll() {
    ui->Pics_List->clear();
}

void Client::sessionOpened() {
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

void Client::transportPictures() {
    long len = 0;
    QList<QListWidgetItem> imageList;
    if (!tcpSocket->isWritable()) {
        QMessageBox msgBox;
        msgBox.setText(tr("something wrong with tcpsocket!"));
        msgBox.exec();
    }
    int count = ui->Pics_List->count();
    for (int i = 0;i < count;++i) {
        QListWidgetItem *item = ui->Pics_List->item(i);
        if (item->isSelected()) {
            imageList.push_back(*item);
        }
    }
    if (!imageList.isEmpty()) {
        for(auto i : imageList) {
            QFile file(i.text());
            file.open(QIODevice::ReadOnly);
            QByteArray imagebytes;
            imagebytes = file.readAll();
            len = imagebytes.length();
            qDebug() << "image length(sure): " << len;
            imagebytes.prepend(formHead(QByteArray::number((qlonglong)len)));
            len = tcpSocket->write(imagebytes);
            if (len == -1) {
                qDebug() << "send pictures failed";
                break;
            } else {
                tcpSocket->flush();
                qDebug() << len << "bytes have been written!";
            }
            QTest::qSleep(500);
        }
        qDebug() << "client has sent pictures";
        QMessageBox msgBox;
        msgBox.setText(QString::fromLocal8Bit("上传成功"));
        msgBox.exec();
        ui->Pics_List->clear();
    } else {
        QMessageBox msgBox;
        msgBox.setText(QString::fromLocal8Bit("没有选中任何图片"));
        msgBox.exec();
    }
    connect(tcpSocket,&QAbstractSocket::disconnected,tcpSocket,&QObject::deleteLater);
//    tcpSocket->disconnectFromHost();
    tcpSocket->waitForBytesWritten();
}

QByteArray Client::formHead(QByteArray by) {
    int len = by.length();
    int i = 0;
    for (i = 10-len;i > 0;i--) {
        by = by.prepend('0');
    }
    return by;
}

void Client::removeFrog() {
    qDebug() << "here";
    ui->graphicsView_2->close();
}
