#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QNetworkSession>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void connectToServer();
    void displayError(QAbstractSocket::SocketError socketError);
    void sessionOpened();
    void getMessage();

signals:
    void removeFrog();

private:
    Ui::Dialog *ui;

    QTcpSocket *tcpSocket;
    QNetworkSession *networkSession;
};

#endif // DIALOG_H
