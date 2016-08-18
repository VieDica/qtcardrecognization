#include "usermanage.h"
#include "ui_usermanage.h"

#include <QDebug>
#include <QSqlQuery>

UserManage::UserManage(QWidget *parent) :
    QDialog(parent),model(new QSqlTableModel),
    ui(new Ui::UserManage)
{
    ui->setupUi(this);

    model->setTable("UserInfo");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("用户名"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("密码"));

    ui->tableView->setModel(model);
    ui->tableView->show();

    connect(ui->Allpick_Button,SIGNAL(clicked(bool)),ui->tableView,SLOT(selectAll()));
    connect(ui->Add_Button,SIGNAL(clicked(bool)),this,SLOT(addItem()));
    connect(ui->Delete_Button,SIGNAL(clicked(bool)),this,SLOT(deleteItem()));
}

UserManage::~UserManage()
{
    delete ui;
}

void UserManage::addItem() {
    int row = model->rowCount();
    qDebug() << "row: " << row;
    model->insertRow(row);
}

void UserManage::deleteItem() {

}
