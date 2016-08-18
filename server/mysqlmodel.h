#ifndef MYSQLMODEL_H
#define MYSQLMODEL_H

#include <QSqlQueryModel>

class MysqlModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    MysqlModel(QObject *parent = 0);
    void creatPicture(int row,int column,QImage *image);
};

#endif // MYSQLMODEL_H
