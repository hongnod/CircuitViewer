#include <QtWidgets>


#include "cell.h"
#include "spreadsheet.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"
#include "xlsxworksheet.h"
#include "xlsxcellrange.h"

Spreadsheet::Spreadsheet(QWidget *parent)
    : QTableWidget(parent)
{
    autoRecalc = false;

    setItemPrototype(new Cell);
    setSelectionMode(ContiguousSelection);

    connect(this, SIGNAL(itemChanged(QTableWidgetItem *)),
            this, SLOT(somethingChanged()));
    db = QSqlDatabase::database();
    clear();
}

QString Spreadsheet::currentLocation() const
{
    return QChar('A' + currentColumn())
           + QString::number(currentRow() + 1);
}

QString Spreadsheet::currentFormula() const
{
    return formula(currentRow(), currentColumn());
}

QTableWidgetSelectionRange Spreadsheet::selectedRange() const
{
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if (ranges.isEmpty())
        return QTableWidgetSelectionRange();
    return ranges.first();
}

void Spreadsheet::clear()
{
    setRowCount(10);
    setColumnCount(10);
 //   setRowCount();
  //  setColumnCount(ColumnCount);

 /*   for (int i = 0; i < ColumnCount; ++i) {
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setText(QString(QChar('A' + i)));
        setHorizontalHeaderItem(i, item);
    }
*/
    setCurrentCell(0, 0);
}

bool Spreadsheet::readFile(const QString &fileName)
{
    Qtxl::Document xlsx(fileName);
    qDebug() << fileName;
    qDebug() << xlsx.sheetNames();
    clear();

    quint16 row;
    quint16 column;
    QString str;
    xlsx.selectSheet("对应列表");
    Qtxl::CellRange cr = xlsx.dimension();
    RowCount = cr.rowCount();
    ColumnCount = cr.columnCount();
    setRowCount(RowCount-1);
    setColumnCount(ColumnCount);

    qDebug() << RowCount << ColumnCount;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString s;

    for(column =0 ;column < ColumnCount; column++)
    {
       qDebug() << column;
       Qtxl::Cell *header = xlsx.cellAt(1,column+1);
       if(header)
       {
           QTableWidgetItem *item = new QTableWidgetItem;
           s = (header->value()).toString();
           if(s.length() == 0) s = "None";
           item->setText(s);
           setHorizontalHeaderItem(column, item);
       }
    }
     qDebug() << "RowCount: " << RowCount;

    for(row = 1; row <RowCount; row++)
        for(column =0 ;column < ColumnCount; column++)
            if (Qtxl::Cell *cell=xlsx.cellAt(row+1, column+1))
            {
               if(cell)
               {
                if(cell->isDateTime())
                {
                    QDateTime dt = cell->dateTime();
                    qDebug() << dt;
                    str = dt.toString(QStringLiteral("yyyy年MM月dd日"));
                    qDebug() << str;
                }else{
                    str = (cell->value()).toString();
                }
                setFormula(row-1, column, str);
               }
            }
    QApplication::restoreOverrideCursor();
    return true;
}

bool Spreadsheet::writeFile(/*const QString &fileName*/)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QSqlQuery query;
    QString  sql, t;
    QStringList tblname = db.tables();
    bool hasMap = false, hasCircuit = false, hasRoute = false;
    if(tblname.contains("map")) hasMap = true;
    if(tblname.contains("circuit")) hasCircuit = true;
    if(tblname.contains("route")) hasRoute = true;

    if(!hasMap) query.exec("CREATE TABLE map(id integer, title varchar(255));");
    if(!hasRoute) query.exec("CREATE TABLE route(selfId integer primary key,pid integer, cmiid text, cmccid text, item text, type varchar(5));");
    if(!hasCircuit) sql = "CREATE TABLE circuit(selfId integer primary key";
    if(!hasMap || !hasCircuit)
    {
        for(int i = 0; i < ColumnCount; i++ )
        {
         if(!hasCircuit) { sql+=", col"; sql+=QString("%1").arg(i+1);sql += " text ";}
         if(!hasMap)
         {
              t = "insert into map values("; t+=QString("%1").arg(i+1); t+=", '";
              t+= horizontalHeaderItem(i)->text(); t += "');";
                      qDebug() << t;
              query.exec(t);
         }
        }
        if(!hasCircuit)
        {
            sql+=");";
            qDebug() << sql;
            query.exec(sql);
        }
   }
    QString cmi_id, cmcc_id, rt_detail;
    for(int row = 0; row < RowCount-1; row++)
    {
        sql = "replace into circuit values(NULL,";
        for(int col = 0; col < ColumnCount; col++)
        {
            sql += "'";
            QTableWidgetItem *itm= item(row, col);
            if(itm)
            {
                sql += itm->text();
                if(1 == col) cmi_id = itm->text();
                if(10 == col) cmcc_id = itm->text();
                if(15 == col) rt_detail = itm->text();
            }
            else
            {sql += " ";}

           col!=ColumnCount-1? sql += "'," :sql += "'";

        }
        sql+=");";
        qDebug() << sql;
        query.exec(sql);
        QString pid = (query.lastInsertId()).toString();
        qDebug() << "\n Info:" << cmi_id << cmcc_id << rt_detail;
        if(rt_detail.contains(QStringLiteral("《")))
        {
            qDebug() << "\nHas Route.";
            routes(pid, cmi_id, cmcc_id, rt_detail);
        }
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool Spreadsheet::routes(QString &pid, QString &cmi_id, QString &cmcc_id, QString &rt)
{

    int idx = 0;
    QSqlQuery qry;
    QString  tsql, sql= "insert into route values(NULL,";
    sql += pid; sql += ", '";
    sql += cmi_id; sql+="','";
    sql += cmcc_id; sql+="','";
    //sql += rt; sql+="','";

    //qDebug() << sql;
    QString item;
    rt.replace(QStringLiteral("《"),QStringLiteral("《`"));
    rt.replace(QStringLiteral("WDM 10"),QStringLiteral("WDM10"));
    QStringList list =rt.split(QRegExp(QStringLiteral("\\s|《|》|分支")));
    qDebug() << list;
    while(idx < list.count())
    {
        item = list.at(idx);
        if(item.startsWith('`'))
        {
            item = item.mid(1);
            tsql = sql + item + "','piple');";
                    qry.exec(tsql);
            qDebug() << tsql;
        }
        else if(item.contains('('))
        {
                qDebug() <<"\nWe do have (";
                QString item1,item2;
                int ii = item.indexOf('(');
                if(ii != 0)
                {
                   item1 = item.left(ii);
                   item2 = item.mid(ii);
                }
                else
                {
                    ii = item.lastIndexOf(')');
                    if(ii < item.length())
                    {
                      item2 = item.left(ii+1);
                      item1 = item.mid(ii+1);
                    }
                    else
                    {
                        item2 = item;
                        item1 = "UNKNOW";
                    }
                }
                tsql = sql + item1 + "','site');";
                        qry.exec(tsql);
                tsql = sql + item2 + "','port');";
                        qry.exec(tsql);
        }
        else
        {
            if(item =="1:" || item =="2:")
            {
                 tsql = sql + item + "','mark');";
            }else{
               tsql = sql + item + "','site');";
            }
                    qry.exec(tsql);
        }
        idx++;
    }

    return true;
}

void Spreadsheet::sort(const SpreadsheetCompare &compare)
{
    QList<QStringList> rows;
    QTableWidgetSelectionRange range = selectedRange();
    int i;

    for (i = 0; i < range.rowCount(); ++i) {
        QStringList row;
        for (int j = 0; j < range.columnCount(); ++j)
            row.append(formula(range.topRow() + i,
                               range.leftColumn() + j));
        rows.append(row);
    }

    qStableSort(rows.begin(), rows.end(), compare);

    for (i = 0; i < range.rowCount(); ++i) {
        for (int j = 0; j < range.columnCount(); ++j)
            setFormula(range.topRow() + i, range.leftColumn() + j,
                       rows[i][j]);
    }

    clearSelection();
    somethingChanged();
}

void Spreadsheet::cut()
{
    copy();
    del();
}

void Spreadsheet::copy()
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str;

    for (int i = 0; i < range.rowCount(); ++i) {
        if (i > 0)
            str += "\n";
        for (int j = 0; j < range.columnCount(); ++j) {
            if (j > 0)
                str += "\t";
            str += formula(range.topRow() + i, range.leftColumn() + j);
        }
    }
    QApplication::clipboard()->setText(str);
}

void Spreadsheet::paste()
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');
    int numRows = rows.count();
    int numColumns = rows.first().count('\t') + 1;

    if (range.rowCount() * range.columnCount() != 1
            && (range.rowCount() != numRows
                || range.columnCount() != numColumns)) {
        QMessageBox::information(this, tr("Spreadsheet"),
                tr("The information cannot be pasted because the copy "
                   "and paste areas aren't the same size."));
        return;
    }

    for (int i = 0; i < numRows; ++i) {
        QStringList columns = rows[i].split('\t');
        for (int j = 0; j < numColumns; ++j) {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;
            if (row < RowCount && column < ColumnCount)
                setFormula(row, column, columns[j]);
        }
    }
    somethingChanged();
}

void Spreadsheet::del()
{
    QList<QTableWidgetItem *> items = selectedItems();
    if (!items.isEmpty()) {
        foreach (QTableWidgetItem *item, items)
            delete item;
        somethingChanged();
    }
}

void Spreadsheet::selectCurrentRow()
{
    selectRow(currentRow());
}

void Spreadsheet::selectCurrentColumn()
{
    selectColumn(currentColumn());
}

void Spreadsheet::recalculate()
{
    for (int row = 0; row < RowCount; ++row) {
        for (int column = 0; column < ColumnCount; ++column) {
            if (cell(row, column))
                cell(row, column)->setDirty();
        }
    }
    viewport()->update();
}

void Spreadsheet::setAutoRecalculate(bool recalc)
{
    autoRecalc = recalc;
    if (autoRecalc)
        recalculate();
}

void Spreadsheet::findNext(const QString &str, Qt::CaseSensitivity cs)
{
    int row = currentRow();
    int column = currentColumn() + 1;

    while (row < RowCount) {
        while (column < ColumnCount) {
            if (text(row, column).contains(str, cs)) {
                clearSelection();
                setCurrentCell(row, column);
                activateWindow();
                return;
            }
            ++column;
        }
        column = 0;
        ++row;
    }
    QApplication::beep();
}

void Spreadsheet::findPrevious(const QString &str,
                               Qt::CaseSensitivity cs)
{
    int row = currentRow();
    int column = currentColumn() - 1;

    while (row >= 0) {
        while (column >= 0) {
            if (text(row, column).contains(str, cs)) {
                clearSelection();
                setCurrentCell(row, column);
                activateWindow();
                return;
            }
            --column;
        }
        column = ColumnCount - 1;
        --row;
    }
    QApplication::beep();
}

void Spreadsheet::somethingChanged()
{
    if (autoRecalc)
        recalculate();
    emit modified();
}

Cell *Spreadsheet::cell(int row, int column) const
{
    return static_cast<Cell *>(item(row, column));
}

void Spreadsheet::setFormula(int row, int column,
                             const QString &formula)
{
    Cell *c = cell(row, column);
    qDebug() << row << "," << column;
    if (!c) {
        c = new Cell;
        setItem(row, column, c);
        qDebug() << "Current Row: " << row << " Current Col: " << column;
    }
    c->setFormula(formula);
}

QString Spreadsheet::formula(int row, int column) const
{
    Cell *c = cell(row, column);
    if (c) {
        return c->formula();
    } else {
        return "";
    }
}

QString Spreadsheet::text(int row, int column) const
{
    Cell *c = cell(row, column);
    if (c) {
        return c->text();
    } else {
        return "";
    }
}


void Spreadsheet::exportRt()
{
   Qtxl::Document xl("d:\\out.xlsx");
   xl.addSheet(QStringLiteral("详细路由"));
   xl.write(1, 1, QStringLiteral("记录ID"));
   xl.write(1, 2, QStringLiteral("电路ID"));
   xl.write(1, 3, QStringLiteral("CMI电路名称"));
   xl.write(1, 4, QStringLiteral("CMCC电路名称"));
   xl.write(1, 5, QStringLiteral("站点"));
   //xl.write(1, 6, QStringLiteral("Z站"));
   xl.write(1, 6, QStringLiteral("路由"));
   xl.write(1, 7, QStringLiteral("主/备"));
   QSqlQuery qry;
   int row = 2;
   QString prepid, colpid, colitem, coltype, ms_mark, presite, cursite;
   qry.exec("select * from route order by selfId");
   while(qry.next()) {
    colpid = qry.value(1).toString();//pid
    if(colpid != prepid) { ms_mark = ""; prepid = colpid;}
    colitem = qry.value(4).toString();
    coltype = qry.value(5).toString();
    if(coltype == "site" && colitem != cursite)
    {
        presite = cursite;
        cursite = colitem;
    }else if(coltype == "mark")
        {
            qDebug() << "we noticed the mark";

            if(colitem == "1:") ms_mark =QStringLiteral("主");
            if(colitem == "2:") ms_mark =QStringLiteral("备");
        }
    else{
        xl.write(row, 1, qry.value(0).toString()); //selfId
        xl.write(row, 2, colpid);
        xl.write(row, 3, qry.value(2).toString()); //cmiid
        xl.write(row, 4, qry.value(3).toString()); //cmccid
        xl.write(row, 5, cursite);
        //xl.write(row, 6, cursite);
        xl.write(row, 6, colitem);
        xl.write(row, 7, ms_mark);
        row++;
    }
   }
   xl.save();
}





bool SpreadsheetCompare::operator()(const QStringList &row1,
                                    const QStringList &row2) const
{
    for (int i = 0; i < KeyCount; ++i) {
        int column = keys[i];
        if (column != -1) {
            if (row1[column] != row2[column]) {
                if (ascending[i]) {
                    return row1[column] < row2[column];
                } else {
                    return row1[column] > row2[column];
                }
            }
        }
    }
    return false;
}

