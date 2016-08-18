#ifndef CSERVER_H
#define CSERVER_H

#include <QMainWindow>
#include "ctcpserver.h"
#include <QTcpServer>

namespace Ui {
class CServer;
}

class CServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit CServer(QWidget *parent = 0);
    ~CServer();

public slots:
    void showWidgets(int);
    void refreshInfo();
    void allPick();
    void showClickedPictures(const QModelIndex &index);
    void query();
    void refreshSubmit();

    void chooseAll();
    void deleteData();
    void uploadInfo();

private:
    Ui::CServer *ui;

    CTcpServer server;
};

#endif // CSERVER_H
