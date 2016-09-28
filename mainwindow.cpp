#include "mainwindow.h"
#include "graphicsrouteview.h"
#include "adddatawidget.h"
#include "configimport.h"
#include "database.h"
#include "xlsx.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    QString fileName("/style/system.qss");
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
      file.setFileName(":/style/system.qss");
      file.open(QFile::ReadOnly);
    }
    setStyleSheet(file.readAll());
    file.close();
    CreateCoreView();
    createActions();
    //createMenus();
    createToolBars();
    createStatusBar();
    setWindowIcon(QIcon(":/images/wx.png"));
}

MainWindow::~MainWindow()
{

}


void MainWindow::CreateCoreView()
{
    QSqlDatabase db = QSqlDatabase::database();
    qDebug() << db.databaseName() << db.tables();
    circuitModel = new QSqlTableModel;
    routeModel = new QSqlTableModel;
    circuitModel->setTable("circuit");
    routeModel->setTable("route");

    circuitModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    routeModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    circuitModel->setHeaderData(0, Qt::Horizontal, "SelfId");
    for(int n=0; n < labels.count(); n++)circuitModel->setHeaderData(n+1,Qt::Horizontal, labels.at(n));
    routeModel->setHeaderData(0,Qt::Horizontal,"selfId");
    routeModel->setHeaderData(1,Qt::Horizontal,"Pid");
    routeModel->setHeaderData(2,Qt::Horizontal,QStringLiteral("路由节点"));
    routeModel->setHeaderData(3,Qt::Horizontal,QStringLiteral("节点类型"));
    circuitModel->select();
    //circuitModel->
    circuitView = new QTableView(this); //电路视图
    circuitView->setFrameStyle(QFrame::NoFrame);
    circuitView->setEditTriggers (QAbstractItemView::NoEditTriggers);
    circuitView->setModel(circuitModel);
    circuitView->setSelectionBehavior(QAbstractItemView::SelectRows);
    circuitView->hideColumn(0);
    circuitView->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色

    tableRouteView = new QTableView(this);//路由表格视图
    tableRouteView->setFrameStyle(QFrame::NoFrame);
    tableRouteView->setEditTriggers (QAbstractItemView::NoEditTriggers);
    tableRouteView->setModel(routeModel);
    //tableRouteView->hideColumn(0);
    //tableRouteView->hideColumn(1);
    //tableRouteView->hideColumn(2);
    //tableRouteView->hideColumn(3);
    tableRouteView->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色

    //NetPlanView *graphicRouteView = new NetPlanView(this); //路由图形视图
    //GraphicsRoutePage*graphicRoutePage = new GraphicsRoutePage(this);
    graphicRouteView = new GraphicsRouteView(this);
    graphicRouteView->setMode(routeModel);
    //graphicRouteView->setMinimumSize(1000,1000);
    QScrollArea * scroll = new QScrollArea(this);
    scroll->setStyleSheet("background-color: transparent");
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->setWidgetResizable(true);
    scroll->setWidget(graphicRouteView);
    QTabWidget *tabRouteView = new  QTabWidget(this); //标签切换表格视图 或 图形视图

    tabRouteView->addTab(tableRouteView, QIcon(":/images/table.png"), QStringLiteral("表格模式"));
    tabRouteView->addTab(scroll, QIcon(":/images/topo.png"), QStringLiteral("图形模式"));

    tabRouteView->setContentsMargins(0,0,2,0);



    textRouteView = new RtTextViewWidget(this); //原文本路由
    //newrttv->setStyleSheet("border : 3px; ");
    QSplitter *vSplit = new QSplitter(Qt::Vertical);
    vSplit->addWidget(tabRouteView);
    vSplit->addWidget(textRouteView);
    vSplit->setStretchFactor(0,90);
    vSplit->setStretchFactor(1,10);
    vSplit->setStyleSheet(
                    QString("QSplitter::handle {background: qlineargradient("
                            "x1: 0, y1: 0, x2: 0, y2: 1,"
                            "stop: 0 %1, stop: 0.07 %2);}").
                    arg(qApp->palette().background().color().name()).
                    arg(qApp->palette().color(QPalette::Dark).name()));
    QSplitter *hSplit = new QSplitter(Qt::Horizontal);
    hSplit->addWidget(circuitView);
    hSplit->addWidget(vSplit);
    hSplit->setStretchFactor(0,50);
    hSplit->setStretchFactor(1,50);
    hSplit->setHandleWidth(1);
    hSplit->setStyleSheet(
                    QString("QSplitter::handle {background: qlineargradient("
                            "x1: 0, y1: 0, x2: 0, y2: 1,"
                            "stop: 0 %1, stop: 0.07 %2);}").
                    arg(qApp->palette().background().color().name()).
                    arg(qApp->palette().color(QPalette::Dark).name()));
    hSplit->setContentsMargins(0,0,0,1);
    setCentralWidget(hSplit);
    //connect(circuitView, SIGNAL(clicked(QModelIndex)), this, SLOT(showRoute(QModelIndex)));
    //cv->setColumnWidth(1,1000);
    connect(circuitView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateCircuitView()));
}

void MainWindow::createActions()
{
    addAction = new QAction(tr("&Add..."), this);
    addAction->setIcon(QIcon(":/images/add.png"));
    //addAction->setShortcut(QKeySequence::Open);
    addAction->setStatusTip(tr("add Ciruit lines"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(Add()));

    importAction = new QAction(tr("&Import..."), this);
    importAction->setIcon(QIcon(":/images/import.png"));
    //addAction->setShortcut(QKeySequence::Open);
    importAction->setStatusTip(tr("import from .xlsx file"));
    connect(importAction, SIGNAL(triggered()), this, SLOT(Import()));

    exportAction = new QAction(tr("&Export..."), this);
    exportAction->setIcon(QIcon(":/images/export.png"));
    //addAction->setShortcut(QKeySequence::Open);
    exportAction->setStatusTip(tr("Export to .xlsx file"));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(Export()));

}

void MainWindow::createMenus()
{
//  dataMenu = menuBar()->addMenu(tr("&Data"));
//  dataMenu->addAction(addAction);
}


void MainWindow::createToolBars()
{
    dataToolBar = addToolBar(tr("&Data"));
    dataToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //dataToolBar->addAction(addAction);
    dataToolBar->addAction(importAction);
    dataToolBar->addAction(exportAction);
}


void MainWindow::Add()
{
   AddDataWidget *add = new AddDataWidget;
   add->setMinimumSize(800,600);
   add->show();
}

void MainWindow::Import()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                               tr("Open Spreadsheet"), ".",
                               tr("Spreadsheet files (*.xlsx)"));
    qDebug() << fileName;
     if (!fileName.isEmpty())
        loadFile(fileName);
    circuitView->horizontalHeader()->setVisible(circuitModel->rowCount()>0);
}


void MainWindow::Export()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save xlsx File"),
                                                      tr("d:\\out.xlsx"),
                                                      tr("Office 2007 XLSX (*.xlsx)"));

    if(fileName.isEmpty())  return;
    Qtxl::Document xl(fileName);
    xl.addSheet(QStringLiteral("详细路由"));
    xl.write(1, 1, QStringLiteral("记录ID"));

    //xl.write(1, 2, QStringLiteral("CMI电路名称"));
    xl.write(1, 2, QStringLiteral("电路名称"));
    xl.write(1, 3, QStringLiteral("站点"));

    xl.write(1, 4, QStringLiteral("路由"));
    xl.write(1, 5, QStringLiteral("类型"));
    QSqlQuery qry;
    int row = 2;
    QString prepid, colpid, colitem, coltype, ms_mark, presite, cursite;
    qry.exec(QStringLiteral("select route.selfId, route.pid, circuit.`集团统一电路编号或各省自用电路编号`,route.`路由项目`, route.`项目类型` from route join circuit on circuit.selfId = route.pid order by route.selfId"));
    while(qry.next()) {
       colpid = qry.value(1).toString();//pid
       if(colpid != prepid) { ms_mark = ""; prepid = colpid; cursite = "";}
       colitem = qry.value(3).toString();
       coltype = qry.value(4).toString();
      if(coltype == "site")
      {
         if(colitem != cursite)
         {
             presite = cursite;
             cursite = colitem;
         }
      }else if(coltype == "mark")
         {
             qDebug() << "we noticed the mark";

             if(colitem.contains("1:")) ms_mark =QStringLiteral("主");
             if(colitem.contains("2:")) ms_mark =QStringLiteral("备");
         }
     else{
         xl.write(row, 1, qry.value(0).toString()); //selfId

         xl.write(row, 2, qry.value(2).toString()); //cmiid
         xl.write(row, 3, cursite); //cmccid
         xl.write(row, 4, qry.value(3).toString());


         xl.write(row, 5, ms_mark);
         row++;
     }
    }
    xl.save();
    exportAction->setChecked(false);
    updateStatusBar(QStringLiteral("导出成功"));
}
 /*
void MainWindow::showRoute(QModelIndex idx)
{
   QModelIndex primaryKeyIndex = circuitModel->index(idx.row(), 0);
     int cid = circuitModel->data(primaryKeyIndex).toInt();
   routeModel->setFilter(QObject::tr("pid ='%1'").arg(cid));
   routeModel->select();
   graphicRouteView->updateData();
   QModelIndex rtIndex = circuitModel->index(idx.row(), 16);
   textRouteView->setRtText(circuitModel->data(rtIndex).toString());
}
*/
void MainWindow::createStatusBar()
{

    statusLabel = new QLabel;
    statusLabel->setIndent(3);

    statusBar()->addWidget(statusLabel);
    //statusBar()->addWidget(formulaLabel, 1);
/*
    connect(spreadsheet, SIGNAL(currentCellChanged(int, int, int, int)),
            this, SLOT(updateStatusBar()));
    connect(spreadsheet, SIGNAL(modified()),
            this, SLOT(spreadsheetModified()));
*/
    updateStatusBar("ok");
}

void MainWindow::updateStatusBar(const QString &info)
{
    statusLabel->setText(info);
}

bool MainWindow::loadFile(const QString &xlsxName)
{
   qDebug() << xlsxName;
   ConfigImport *cfgImport = new ConfigImport(xlsxName);
   connect(cfgImport, SIGNAL(sig_import(QStringList)), this, SLOT(slot_import(QStringList)));
    cfgImport->show();
return true;
}


void MainWindow::updateCircuitView()
{
    QModelIndex index = circuitView->currentIndex();
    if(index.isValid())
    {
        QSqlRecord record = circuitModel->record(index.row());
        int id = record.value("selfId").toInt();
        routeModel->setFilter(QString("pid = %1").arg(id));
    }else{
        routeModel->setFilter("pid = -1");
    }
    routeModel->select();
    graphicRouteView->updateData();
    tableRouteView->horizontalHeader()->setVisible(routeModel->rowCount()>0);
    circuitView->horizontalHeader()->setVisible(circuitModel->rowCount()>0);
    QModelIndex rtIndex = circuitModel->index(index.row(), 16);
    textRouteView->setRtText(circuitModel->data(rtIndex).toString());
}


void MainWindow::slot_import(const QStringList &records)
{
  qDebug() << "Start to import...";
  int row = circuitModel->rowCount();
  updateStatusBar("Importing line:" + QString("%1").arg(row));
  qDebug() << "Row Number: " << row;
  circuitModel->insertRow(row);
  for(int col = 0; col < records.count(); col++)
  {
      circuitModel->setData(circuitModel->index(row,col+1),records.at(col));
  }
  circuitModel->submitAll();
  QString cid = circuitModel->query().lastInsertId().toString();
  QString rt_details = records.at(15);
  importRoutes(cid, rt_details);
}

void MainWindow::importRoutes(QString &cid, QString &rt)
{
    int idx = 0;
    int r;
    QSqlRecord record = routeModel->record();
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
           // tsql = sql + item + "','piple');";
                    //qry.exec(tsql);
                    //if(qry.lastError().isValid()) return false;
            //qDebug() << tsql;

            r = routeModel->rowCount();
            routeModel->insertRow(r);
            routeModel->setData(routeModel->index(r,1),cid);
            routeModel->setData(routeModel->index(r,2),item);
            routeModel->setData(routeModel->index(r,3),QStringLiteral("piple"));
            //record.setValue("pid", cid);
            //record.setValue(QStringLiteral("路由项目"),item);
            //record.setValue(QStringLiteral("项目类型"),"piple");
            //routeModel->insertRecord(1,record);

            qDebug()<< "In piple: " << item;
            qDebug()<< routeModel->query().lastQuery();
            qDebug()<< "Running insertion result: " << routeModel->lastError().text();
        }
        else if(item.contains('('))
        {
             //qDebug() <<"\nWe do have (";
             QStringList lst = item.split(QRegularExpression(QStringLiteral("\\s")));
             for(int k=0; k<lst.length(); k++)
             {
                QString titem = lst.at(k);
                if(!titem.contains(" ") || !titem.isEmpty())
                {
                   //tsql = sql + titem + "','port');";
                        //qry.exec(tsql);
                        //if(qry.lastError().isValid()) return false;

                    r = routeModel->rowCount();
                    routeModel->insertRow(r);
                    routeModel->setData(routeModel->index(r,1),cid);
                    routeModel->setData(routeModel->index(r,2),titem);
                    routeModel->setData(routeModel->index(r,3),QStringLiteral("port"));
                    /*
                    record.setValue("pid", cid);
                    record.setValue(QStringLiteral("路由项目"),titem);
                    record.setValue(QStringLiteral("项目类型"),"port");
                    routeModel->insertRecord(1,record);
                    */
                    qDebug()<< "In port: " << titem;
                    qDebug()<< "Running insertion result: " << routeModel->lastError().text();
                }
             }
        }
        else
        {
            if(item.contains("1:") || item.contains("2:"))
            {
                 //tsql = sql + item + "','mark');";

                 r = routeModel->rowCount();
                 routeModel->insertRow(r);
                 routeModel->setData(routeModel->index(r,1),cid);
                 routeModel->setData(routeModel->index(r,2),item);
                 routeModel->setData(routeModel->index(r,3),QStringLiteral("mark"));

                 /*
                record.setValue("pid", cid);
                record.setValue(QStringLiteral("路由项目"),item);
                record.setValue(QStringLiteral("项目类型"),"mark");
                routeModel->insertRecord(1,record);
                 */
                 qDebug()<< "In mark: " << item;
                 qDebug()<< "Running insertion result: " << routeModel->lastError().text();
            }else{
                  //item.replace(QRegularExpression(QStringLiteral("\\s")),QStringLiteral("###"));

                  //tsql = sql + item.left(item.indexOf(QRegularExpression(QStringLiteral("\\s")))) + "','site');";
                 qDebug()<< "In site: " << item << " and after processing: " << item.left(item.indexOf(QRegularExpression(QStringLiteral("\\s"))));

                  r = routeModel->rowCount();
                  routeModel->insertRow(r);
                  routeModel->setData(routeModel->index(r,1),cid);
                  routeModel->setData(routeModel->index(r,2),item.left(item.indexOf(QRegularExpression(QStringLiteral("\\s")))));
                  routeModel->setData(routeModel->index(r,3),QStringLiteral("site"));
                  /*
                 record.setValue("pid", cid);
                 record.setValue(QStringLiteral("路由项目"),item.left(item.indexOf(QRegularExpression(QStringLiteral("\\s")))));
                 record.setValue(QStringLiteral("项目类型"),"site");
                 routeModel->insertRecord(1,record);
                 */
                 qDebug()<< "Running insertion result: " << routeModel->lastError().text();
            }
                    //qry.exec(tsql);
                    //if(qry.lastError().isValid()) return false;
        }
        idx++;
    }
    if(!routeModel->submitAll()) qDebug() << "Submitting failed!!!";
    //qDebug()<< "Running insertion result: " << routeModel->lastError().text();
}
