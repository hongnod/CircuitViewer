#include "configimport.h"
#include "database.h"

ConfigImport::ConfigImport(QWidget *parent) :
    QWidget(parent)
{

}

ConfigImport::ConfigImport(const QString &xlsxName, QWidget *parent) :
    QWidget(parent)
{
    selSheetLabel = new QLabel(this);
    selSheetLabel->setText(QStringLiteral("选择导入的Sheet："));
    selSheet = new QComboBox(this);
    //selSheet->setDisabled(true);
    QLabel *startRowLabel  = new QLabel(this);
    startRowLabel->setText(QStringLiteral("导入的起始行："));
    startRow = new QLineEdit(this);
    QRegExp regx("[0-9]+$");
                QValidator *validator = new QRegExpValidator(regx);
    startRow->setValidator( validator );
    QGridLayout *gLay = new QGridLayout;
    gLay->addWidget(selSheetLabel,0,0,1,1);
    gLay->addWidget(selSheet,0,1,1,1);
    gLay->addWidget(startRowLabel,0,2,1,1);
    gLay->addWidget(startRow,0,3,1,1);
    /*
    QStringList labels;
    labels << QStringLiteral("CMIiD:")
           << QStringLiteral("CircuitId(CMI):")
           << QStringLiteral("ProductName:")
           << QStringLiteral("OrderType:")
           << QStringLiteral("Bandwidth:")
           << QStringLiteral("CustomerName:")
           << QStringLiteral("CompletionDate:")
           << QStringLiteral("A端/B端/自用:")
           << QStringLiteral("是否传输专线:")
           << QStringLiteral("CMCC相关:")
           << QStringLiteral("CMCC电路编号:")
           << QStringLiteral("修改说明:")
           << QStringLiteral("是否确认:")
           << QStringLiteral("集团调单号")
           << QStringLiteral("集团统一电路编号或各省自用电路编号:")
           << QStringLiteral("路由:")
           << QStringLiteral("维护省:")
           << QStringLiteral("所属EM:")
           << QStringLiteral("复核:")
           << QStringLiteral("所属SDH系统:")
           << QStringLiteral("附加信息:");
           */
    for(int i=0; i < 21; i++)
    {
        selColLabel[i] = new QLabel(this);
        selColLabel[i]->setText(labels[i]);
        selCol[i] = new QComboBox(this);
        gLay->addWidget(selColLabel[i], 1+i/2, i%2 == 0?0:2, 1, 1);
        gLay->addWidget(selCol[i], 1+i/2, i%2 == 0?1:3, 1, 1);
     }
    QLabel *endRowLabel  = new QLabel(this);
    endRowLabel->setText(QStringLiteral("导入的结束行："));
    endRow = new QLineEdit(this);
    endRow->setValidator( validator );
    gLay->addWidget(endRowLabel,11,2,1,1);
    gLay->addWidget(endRow,11,3,1,1);

    QPushButton *okButton =new QPushButton(this);
    okButton->setText("Import");
    QPushButton *cancelButton = new QPushButton(this);
    cancelButton->setText("Cancel");
    gLay->addWidget(okButton,12,1,1,1);
    gLay->addWidget(cancelButton,12,3,1,1);
    gLay->setColumnStretch(1,1);
    gLay->setColumnStretch(3,1);
    setLayout(gLay);

    xlsx = new Qtxl::Document(xlsxName);
    QStringList lst = xlsx->sheetNames();
    if(lst.count() > 0)
    {
        selSheet->addItems(lst);
        selSheet->setCurrentIndex(0);
        updateColSel(lst.at(0));
    }
    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(Import()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(selSheet, SIGNAL(currentTextChanged(QString)), this, SLOT(updateColSel(QString)));
}


ConfigImport::~ConfigImport()
{
   if(xlsx) delete xlsx;
}


void ConfigImport::updateColSel(QString sheetName)
{

    qDebug() << sheetName;

    if(!(xlsx->selectSheet(sheetName))) return;
    Qtxl::CellRange cr = xlsx->dimension();
    RowCount = cr.rowCount();
    ColumnCount = cr.columnCount();
    qDebug() << "Rows: " << RowCount;
    qDebug() <<"Cols: " << ColumnCount;

    QStringList options;
    options << "";
    int MaxCol = ColumnCount < 26 ? ColumnCount : 26;
    for(int k=0; k<MaxCol; k++)
    {
        options << QChar('A'+ k);
    }

    for(int i = 0; i <21; i++)
    {
      selCol[i]->clear();
      selCol[i]->addItems(options);
      if(i < MaxCol)
      {
          selCol[i]->setCurrentIndex(i+1);
      }
    }
    startRow->setText("2");
    endRow->setText(QString("%1").arg(RowCount));
}


void ConfigImport::Import()
{
    QString singleitem;
    QStringList recorditems;
    Qtxl::Cell *cell;
    qDebug() << "Beginning of Import....";
    QString rowStr = startRow->text();
    int start_row = rowStr.toInt();
    rowStr = endRow->text();
    int end_row = rowStr.toInt();
    qDebug() << start_row << " to " << end_row;
    if(start_row < 1 || start_row > end_row || end_row > RowCount)
    {
        qDebug() << "abnormal start or end setting";
        return;
    }


    for(int row = start_row; row <= end_row; row++)
    {
       for(int col = 1; col <=21; col++)
       {
          if(selCol[col-1]->currentIndex() == 0)
          {
              singleitem = "";
          }else
          {
            if(cell=xlsx->cellAt(row, selCol[col-1]->currentIndex()))
            {
              if(cell->isDateTime())
              {
                  QDateTime dt = cell->dateTime();
                  qDebug() << dt;
                  singleitem = dt.toString(QStringLiteral("yyyy年MM月dd日"));
                  qDebug() << singleitem;
              }else{
                  singleitem = (cell->value()).toString();
              }
            }
       }
      recorditems << singleitem;
    }

    emit sig_import(recorditems);
    /*
    if(!InsertRecords(recorditems))
    {
       recorditems.clear();
       QMessageBox::information(NULL, "database insertion error!!!","Row: "+QString("%1").arg(row)+" insert error!");
       return;
    }
    */
    recorditems.clear();
   }
    QMessageBox::information(NULL, "Importing done.",QString("%1").arg(end_row-start_row+1)+" row/rows imported");

    close();
}
