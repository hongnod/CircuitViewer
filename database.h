#ifndef DATABASE_H
#define DATABASE_H


#include <QtSql>
#include <QtCore>

//QStringList fields;
//fields <<
static bool createConnection()
{
   QSqlDatabase selfdb = QSqlDatabase::addDatabase("QSQLITE");
   selfdb.setDatabaseName("./circuit.db3");

   if(!selfdb.open()){
          qDebug() << "Can not open connection.";
          //exit(CONNECTION_FAILED);
          return false;
     }

    if(!selfdb.tables().contains("circuit"))
    {
        QSqlQuery query;
        QString circuit = QStringLiteral("CREATE TABLE `circuit` (\
                            `selfId`	integer,            \
                            `CMIiD`	text,                   \
                            `CircuitId(CMI)`	text,       \
                            `ProductName`	text,           \
                            `OrderType`	text,               \
                            `Bandwidth`	text,               \
                            `CustomerName`	text,           \
                            `CompletionDate`	text,       \
                            `A端/B端/自用`	text,               \
                            `是否传输专线`	text,               \
                            `CMCC相关`	text,               \
                            `CMCC电路编号`	text,               \
                            `修改说明`	text,                   \
                            `是否确认`	text,                   \
                            `集团调单号`	text,                   \
                            `集团统一电路编号或各省自用电路编号`	text,       \
                            `路由`	text,                   \
                            `维护省`	text,                   \
                            `所属EM`	text,                   \
                            `复核`	text,                   \
                            `所属SDH系统`	text,               \
                            `附加信息`	text,                   \
                            PRIMARY KEY(selfId)             \
                        );");
        qDebug() << circuit;
        query.exec(circuit);
        query.exec(QStringLiteral("CREATE INDEX  circuit_index ON circuit(CMIiD,'CircuitId(CMI)',CMCC电路编号,路由);"));
    }
    if(!selfdb.tables().contains("route"))
    {
        QSqlQuery query;
        QString route = QStringLiteral("CREATE TABLE `route` ( \
                        `selfId`	integer,    \
                        `pid`	integer,    \
                        `路由项目`	text,           \
                        `项目类型`	varchar(5),     \
                        PRIMARY KEY(selfId)     \
                    );");
        query.exec(route);
    }
 return true;
}

/*
static bool routes(QString &pid, QString &rt)
{
    int idx = 0;
    QSqlQuery qry;
    QString  tsql, sql= "insert into route values(NULL,";
    sql += pid; sql += ", '";


    qDebug() << rt;
    QString item;


    rt.replace(QStringLiteral("《"),QStringLiteral("《`"));
   qDebug() << "after replace ZSM" << rt;
    rt.replace(QStringLiteral("（"),QStringLiteral("("));
   qDebug() << "after replace ZZWKH" << rt;
    rt.replace(QStringLiteral("）"),QStringLiteral(")"));
   qDebug() << "after replace YZWKH" << rt;
    rt.replace(QStringLiteral("："),QStringLiteral(":"));
    qDebug() << "after replace ZWMH" << rt;
    //rt.replace(QRegularExpression(QStringLiteral(" \\S+ 分支| \\S+分支")),QStringLiteral("分支"));
    qDebug() << "after replace FENZHI" << rt;
    rt.replace(QStringLiteral("()"),QStringLiteral(""));
    qDebug() << "Before Split: " << rt;
    QStringList list =rt.split(QRegularExpression(QStringLiteral("《|》|分支|～")));

    qDebug() << "After Split: " << list;
    while(idx < list.count())
    {
        item = list.at(idx);
        if(item.startsWith('`'))
        {
            item = item.mid(1);
            item.replace(QRegularExpression(QStringLiteral("\\s")),QStringLiteral(""));
            tsql = sql + item + "','piple');";
                    qry.exec(tsql);
                    if(qry.lastError().isValid()) return false;
            qDebug() << tsql;
        }
        else if(item.contains('('))
        {
             qDebug() <<"\nWe do have (";
             QStringList lst = item.split(QRegularExpression(QStringLiteral("\\s")));
             for(int k=0; k<lst.length(); k++)
             {
                QString titem = lst.at(k);
                if(!titem.contains(" ") || !titem.isEmpty())
                {
                   tsql = sql + titem + "','port');";
                        qry.exec(tsql);
                        if(qry.lastError().isValid()) return false;
                }
             }
        }
        else
        {
            if(item.contains("1:") || item.contains("2:"))
            {
                 tsql = sql + item + "','mark');";
            }else{
                  //item.replace(QRegularExpression(QStringLiteral("\\s")),QStringLiteral("###"));

                  tsql = sql + item.left(item.indexOf(QRegularExpression(QStringLiteral("\\s")))) + "','site');";
            }
                    qry.exec(tsql);
                    if(qry.lastError().isValid()) return false;
        }
        idx++;
    }

    return true;
}


static bool InsertRecords(const QStringList &records)
{
    qDebug() << records;
    QSqlQuery query;
    QString  sql, t;
    if(21 != records.count())
    {
        return false;
    }

    QSqlDatabase::database().transaction();
    sql = "REPLACE INTO circuit VALUES(NULL";

    for(int col = 0; col < records.count(); col++)
    {
        sql += ", '";
        t = records.at(col);
        t.replace("'",QStringLiteral("’"));
        sql += t;
        sql += "'";
    }
    sql += ");";
    qDebug() << sql;
    query.exec(sql);
    if(query.lastError().isValid()) return false;
    QString pid = (query.lastInsertId()).toString();
    QString rt_details = records.at(15);
    if(rt_details.contains(QStringLiteral("《")))
    {
        qDebug() << "\nHas Route.";
        routes(pid, rt_details);
    }
    QSqlDatabase::database().commit();

 return true;
}
*/


static QStringList labels = {
 QStringLiteral("CMIiD:")
, QStringLiteral("CircuitId(CMI):")
, QStringLiteral("ProductName:")
, QStringLiteral("OrderType:")
, QStringLiteral("Bandwidth:")
, QStringLiteral("CustomerName:")
, QStringLiteral("CompletionDate:")
, QStringLiteral("A端/B端/自用:")
, QStringLiteral("是否传输专线:")
, QStringLiteral("CMCC相关:")
, QStringLiteral("CMCC电路编号:")
, QStringLiteral("修改说明:")
, QStringLiteral("是否确认:")
, QStringLiteral("集团调单号")
, QStringLiteral("集团统一电路编号或各省自用电路编号:")
, QStringLiteral("路由:")
, QStringLiteral("维护省:")
, QStringLiteral("所属EM:")
, QStringLiteral("复核:")
, QStringLiteral("所属SDH系统:")
, QStringLiteral("附加信息:")};

#endif // DATABASE_H
