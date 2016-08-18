#ifndef CLIENT_H
#define CLIENT_H

#include "dialog.h"
#include <QMainWindow>
#include <QTcpSocket>
#include <QNetworkSession>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

namespace Ui {
class Client;
}

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

    QByteArray formHead(QByteArray by);

signals:
    void getPicNames(QStringList fn);

public slots:
    void openFile();
    void display(QStringList fn);
    void allPick();
    void reversePick();
    void removeAll();

    void sessionOpened();
    void transportPictures();

    void removeFrog();

private:
    Ui::Client *ui;

//    QComboBox *hostCombo;
//    QLineEdit *portLineEdit;
//    QLabel *statusLabel;
//    QPushButton *getConnectionButton;

    QTcpSocket *tcpSocket;
    QNetworkSession *networkSession;
};

#endif // CLIENT_H
