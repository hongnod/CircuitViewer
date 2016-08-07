
#include "mainwindow.h"
#include <QApplication>
#include "database.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!createConnection())
    {
        qDebug() << "failed to open database";
        return 1;
    }
    QSqlDatabase db = QSqlDatabase::database();
    qDebug() << db.databaseName() << db.tables();
    MainWindow w;
    w.show();

    return a.exec();
}
