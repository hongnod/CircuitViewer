#ifndef CONFIGIMPORT_H
#define CONFIGIMPORT_H

#include <QtWidgets>
#include "xlsx.h"

class ConfigImport : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigImport(QWidget *parent = 0);
    ConfigImport(const QString &xlsxName, QWidget *parent = 0);
    ~ConfigImport();
    //void startImport(const QString &xlsxName);
private:
   QLabel *selSheetLabel;
   QComboBox *selSheet;
   QLineEdit *startRow;
   QLineEdit *endRow;
   QLabel *selColLabel[21];
   QComboBox *selCol[21];
   Qtxl::Document *xlsx;
   int RowCount;
   int ColumnCount;
signals:
  void sig_import(const QStringList &records);
 public slots:
   void updateColSel(QString sheetName);
   void Import();
};

#endif // CONFIGIMPORT_H
