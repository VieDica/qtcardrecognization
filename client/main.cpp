#include "client.h"
#include <QApplication>
#include <QFile>
#include <QtGui>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client w;
    QFile qss("./Client.qss");
    qss.open(QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());
    qss.close();
    w.show();
    return a.exec();
}
