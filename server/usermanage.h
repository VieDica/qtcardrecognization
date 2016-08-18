#ifndef USERMANAGE_H
#define USERMANAGE_H

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class UserManage;
}

class UserManage : public QDialog
{
    Q_OBJECT

public:
    explicit UserManage(QWidget *parent = 0);
    ~UserManage();

    QSqlTableModel *model;

public slots:
    void addItem();
    void deleteItem();

private:
    Ui::UserManage *ui;
};

#endif // USERMANAGE_H
