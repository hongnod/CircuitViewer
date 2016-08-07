#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <QTableWidget>
#include <QtSql>
QT_BEGIN_NAMESPACE
class Cell;
class SpreadsheetCompare;
class QSqlDatabase;
QT_END_NAMESPACE

class Spreadsheet : public QTableWidget
{
    Q_OBJECT

public:
    Spreadsheet(QWidget *parent = 0);

    bool autoRecalculate() const { return autoRecalc; }
    QString currentLocation() const;
    QString currentFormula() const;
    QTableWidgetSelectionRange selectedRange() const;
    void clear();
    bool readFile(const QString &fileName);
    bool writeFile(/*const QString &fileName*/);
    void sort(const SpreadsheetCompare &compare);

public slots:
    void cut();
    void copy();
    void paste();
    void del();
    void selectCurrentRow();
    void selectCurrentColumn();
    void recalculate();
    void exportRt();
    void setAutoRecalculate(bool recalc);
    void findNext(const QString &str, Qt::CaseSensitivity cs);
    void findPrevious(const QString &str, Qt::CaseSensitivity cs);

signals:
    void modified();

private slots:
    void somethingChanged();

private:
    quint16  RowCount =10, ColumnCount = 10;
    bool     routes(QString &pid, QString &cmi_id, QString &cmcc_id, QString &rt);
    Cell *cell(int row, int column) const;
    QString text(int row, int column) const;
    QString formula(int row, int column) const;
    void setFormula(int row, int column, const QString &formula);
    QSqlDatabase db;
    bool autoRecalc;
};

class SpreadsheetCompare
{
public:
    bool operator()(const QStringList &row1,
                    const QStringList &row2) const;

    enum { KeyCount = 3 };
    int keys[KeyCount];
    bool ascending[KeyCount];
};

#endif
