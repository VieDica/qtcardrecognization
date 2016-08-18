#include "mysqlmodel.h"

MysqlModel::MysqlModel(QObject *parent) : QSqlQueryModel(parent)
{

}

void MysqlModel::creatPicture(int row, int column, QImage *image) {
    this->createIndex(row,column,image);
}
