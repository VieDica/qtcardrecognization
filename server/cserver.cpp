#include "cserver.h"
#include "ui_cserver.h"
#include "ctcpserver.h"
#include "usermanage.h"
#include "mysqlmodel.h"
#include <QSignalMapper>
#include <QtNetwork>
#include <QMessageBox>
#include <QDebug>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QGraphicsPixmapItem>

CServer::CServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CServer)
{
    ui->setupUi(this);


    //其实我们要用的就是本地地址
    if (!server.listen(QHostAddress::LocalHost,12345)) {
        QMessageBox::critical(this, tr("多线程服务器"),
                              tr("无法启动服务器: %1.")
                              .arg(server.errorString()));
        close();
        return;
    }

    qDebug() << "Ip: " << server.serverAddress();
    qDebug() << "port: " << server.serverPort();


    QSignalMapper *signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(ui->Recognization_Button,1);
    signalMapper->setMapping(ui->Submit_Button,2);
    signalMapper->setMapping(ui->Query_Button,3);

    ui->stackedWidget->setCurrentWidget(ui->page);

    connect(ui->Recognization_Button,SIGNAL(clicked(bool)),signalMapper,SLOT(map()));
    connect(ui->Submit_Button,SIGNAL(clicked(bool)),signalMapper,SLOT(map()));
    connect(ui->Query_Button,SIGNAL(clicked(bool)),signalMapper,SLOT(map()));

    connect(signalMapper,SIGNAL(mapped(int)),this,SLOT(showWidgets(int)));

    //Page 2
    connect(ui->Recognization_Button,SIGNAL(clicked(bool)),this,SLOT(refreshInfo()));
//    connect(ui->Allpick_Button,SIGNAL(clicked(bool)),this,SLOT(allPick()));
//    connect(ui->Allpick_Button_2,SIGNAL(clicked(bool)),this,SLOT(allPick()));
    connect(ui->Pic_View_2,SIGNAL(clicked(QModelIndex)),this,SLOT(showClickedPictures(QModelIndex)));

    //Page 3
    connect(ui->Submit_Button,SIGNAL(clicked(bool)),this,SLOT(refreshSubmit()));
    connect(ui->Allpick_Button,SIGNAL(clicked(bool)),this,SLOT(chooseAll()));
    connect(ui->Delete_Button,SIGNAL(clicked(bool)),this,SLOT(deleteData()));
    connect(ui->Upload_Button,SIGNAL(clicked(bool)),this,SLOT(uploadInfo()));

    //Page 4
//    ui->calendarWidget->setMaximumDate(QDate::currentDate());
//    ui->calendarWidget_2->setMaximumDate(QDate::currentDate());
    connect(ui->Check_Button,SIGNAL(clicked(bool)),this,SLOT(query()));

    //MenuBar
    UserManage *um = new UserManage;
    connect(ui->Usermanage_action,SIGNAL(triggered(bool)),um,SLOT(show()));
    connect(ui->Exit_action,SIGNAL(triggered(bool)),this,SLOT(close()));

}

CServer::~CServer()
{
    delete ui;
}

void CServer::showWidgets(int i) {
    switch (i) {
    case 1:
        ui->stackedWidget->setCurrentWidget(ui->page_2);
        ui->stackedWidget_2->setCurrentWidget(ui->page_6);
        break;
    case 2:
        ui->stackedWidget->setCurrentWidget(ui->page_3);
        break;
    case 3:
        ui->stackedWidget->setCurrentWidget(ui->page_4);
        break;
    default:
        break;
    }
}

void CServer::refreshInfo() {
    qDebug() << "show refresh Info";
    QSqlQuery query;
    query.prepare("SELECT pAddr FROM PhotoInfo WHERE pFlag = '0'");
    query.exec();
    MysqlModel *model = new MysqlModel;
    model->setQuery("SELECT pName,pTime,pLoader FROM PhotoInfo WHERE pFlag = '0'");
    model->insertColumn(3);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("名称"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("上传时间"));
    model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("上传者"));
    model->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("车牌图片"));

    int i = 0;
    if (query.next()) {
        QString addr = query.value(0).toString();
        QImage *image = new QImage(addr);
        model->creatPicture(i++,3,image);
    }

    ui->Pic_View_2->setModel(model);
    ui->Pic_View_2->show();

}

void CServer::allPick() {
    ui->Pic_View_2->selectAll();
}

void CServer::showClickedPictures(const QModelIndex &index) {
    QString name = ui->Pic_View_2->model()->data(index).toString();
    QString address;
    QSqlQuery query;
    query.prepare("SELECT pAddr FROM PhotoInfo WHERE pName = (:name)");
    query.bindValue(":name",name);
    if (!query.exec()) {
        qDebug() << "query failed";
        QString text = query.lastError().text();
        qDebug() << text;
    }
    if (query.next()) {
        address = query.value(0).toString();
        qDebug() << "addresss: " << address;
    }
    QImage image;
    if (!image.load(address))
        qDebug() << "load picture failed.";
    image = image.scaled(465,355);
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addItem(item);

    ui->Card_View->setScene(scene);
    ui->Card_View->show();
}

void CServer::query() {
    QString start = ui->dateEdit->date().toString("yyMMdd");
    QString end = ui->dateEdit_2->date().toString("yyMMdd");
    QString number = ui->Number_Label->text();
    qDebug() << "start: " << start;
    qDebug() << "end: " << end;

    if (start > end) {
        QMessageBox msgBox;
        msgBox.setText(QString::fromLocal8Bit("起始日期不能超过截至日期！"));
        msgBox.exec();
    }

    if (number.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText(QString::fromLocal8Bit("没有输入号码！"));
        msgBox.exec();
    }

    if (ui->radioButton->isChecked()) {
        qDebug() << "according to card";
        QString id;
        QSqlQuery query;
        query.prepare("SELECT ID FROM LicenceInfo WHERE cNum = (:number)");
        query.bindValue(":number",number);
        query.exec();
        if (query.next()) {
            id = query.value(0).toString();
            qDebug() << "id: " << id;
        }
        QSqlQueryModel *model = new QSqlQueryModel;
        query.prepare("SELECT oName,ID,oPhone,oAddr,oSex FROM OwnerInfo WHERE ID = (:id)");
        query.bindValue(":id",id);
        query.exec();
        model->setQuery(query);
        model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("车主姓名"));
        model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("车主身份证"));
        model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("车主手机号"));
        model->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("车主地址"));
        model->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("车主性别"));

        ui->Info_View->setModel(model);
        ui->Info_View->show();
        qDebug() << "here";
    } else if (ui->radioButton_2->isChecked()) {
        QSqlQuery query;
        qDebug() << "number: " << number;
        query.prepare("SELECT cNum,cType FROM LicenceInfo WHERE ID = (:id)");
        query.bindValue(":id",number);
        query.exec();
        QSqlQueryModel *model = new QSqlQueryModel;
        model->setQuery(query);
        model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("车牌号"));
        model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("车型"));

        ui->Info_View->setModel(model);
        ui->Info_View->show();
    } else if (ui->radioButton_3->isChecked()) {
        QString id;
        QSqlQuery query;
        qDebug() << "name: " << number;
        query.prepare("SELECT ID FROM OwnerInfo WHERE oName = (:name)");
        query.bindValue(":name",number);
        query.exec();
        if (query.next()) {
            id = query.value(0).toString();
        }
        qDebug() << "id: " << id;
        query.prepare("SELECT cNum,cType FROM LicenceInfo WHERE ID = (:id)");
        query.bindValue(":id",id);
        query.exec();
        QSqlQueryModel *model = new QSqlQueryModel;
        model->setQuery(query);
        model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("车牌号"));
        model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("车型"));
        ui->Info_View->setModel(model);
        ui->Info_View->show();
    } else {
        QMessageBox msgBox;
        msgBox.setText(QString::fromLocal8Bit("没有选择分类条件！"));
        msgBox.exec();
    }
}

void CServer::refreshSubmit() {
    qDebug() << "show submit Info";
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT pName,pTime,pLoader,cNum FROM PhotoInfo WHERE pFlag = '0'");
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("名称"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("上传时间"));
    model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("上传者"));
    model->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("车牌号码"));

    ui->Succeess_View->setModel(model);

    QSqlQueryModel *model2 = new QSqlQueryModel;
    model2->setQuery("SELECT pName,pTime,pLoader,cNum FROM PhotoInfo WHERE pFlag = '2'");
    model2->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("名称"));
    model2->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("上传时间"));
    model2->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("上传者"));
    model2->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("车牌号码"));
    ui->Fail_View->setModel(model2);
}

void CServer::chooseAll() {
    ui->Succeess_View->selectAll();
    ui->Fail_View->selectAll();
}

void CServer::deleteData() {
    int curRow = ui->Succeess_View->currentIndex().row();
    qDebug() << "curRow: " << curRow;
    QAbstractItemModel *model = ui->Succeess_View->model();
    qDebug() << "rowcount: " << model->rowCount();
    model->removeRow(curRow);
    model->submit();
    ui->Succeess_View->setModel(model);
    qDebug() << "delete";
//    int ok = QMessageBox::warning(this,tr("删除选中行！"),tr("你确定删除当前选取的行吗?"),QMessageBox::Yes,QMessageBox::No);
//    if (ok == QMessageBox::Yes)
//        ui->Succeess_View->model()->submit();
//    else
//        ui->Succeess_View->model()->revert();
}
void CServer::uploadInfo() {
    QMessageBox msgBox;
    msgBox.setText(tr("上传成功！"));
    msgBox.exec();
}
