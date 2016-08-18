#include "cserver.h"
#include "usermanage.h"
#include <QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QtGui>
#include <QTextCodec>

bool createConnection();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!createConnection()) {
        qDebug() << "error in open database!";
    } else {
        qDebug() << "success";
    }
    CServer w;
    QFile qss("./CServer.qss");
    qss.open(QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());
    qss.close();
    w.show();

    return a.exec();
}

bool createConnection() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("CServer.db");
    if (!db.open()) {
        QMessageBox::critical(0,QObject::tr("Database Error"),db.lastError().text());
        return false;
    }
    return true;
}
