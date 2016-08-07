#include "adddatawidget.h"
#include <QtCore>

AddDataWidget::AddDataWidget(QWidget *parent) :
    QWidget(parent)
{
  spreadsheet = new Spreadsheet(this);
  spreadsheet->setStyleSheet("selection-background-color:lightblue;"); //设置选中背景色

  spreadsheet->horizontalHeader()->setStyleSheet("QHeaderView{background:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                                          stop: 0 #ffffff, stop: 0.4 #ffffff,\
                                                          stop: 0.5 #F6F7F9, stop: 1.0 #F1F2F4);color: #000000;}");
  spreadsheet->horizontalHeader()->setStyleSheet("QHeaderView::section { \
        border-right: 1px solid qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, \
                                                                stop: 0 #f0f0f0, stop: 1.0 #D5D5D5);\
        border-bottom: 1px solid #D5D5D5; \
        border-top: none; \
        border-left: none; \
        background-color: transparent; \
        padding: 2px; \
        padding-left: 3px;}");

  spreadsheet->setColumnCount(21);
  QStringList header;
  header << QStringLiteral("CMIiD")
         << QStringLiteral("CircuitId(CMI)")
         << QStringLiteral("ProductName")
         << QStringLiteral("OrderType")
         << QStringLiteral("Bandwidth")
         << QStringLiteral("CustomerName")
         << QStringLiteral("CompletionDate")
         << QStringLiteral("A端/B端/自用")
         << QStringLiteral("是否传输专线")
         << QStringLiteral("CMCC相关")
         << QStringLiteral("CMCC电路编号")
         << QStringLiteral("修改说明")
         << QStringLiteral("是否确认"	)
         << QStringLiteral("集团调单号")
         << QStringLiteral("集团统一电路编号或各省自用电路编号")
         << QStringLiteral("路由")
         << QStringLiteral("维护省")
         << QStringLiteral("所属EM")
         << QStringLiteral("复核")
         << QStringLiteral("所属SDH系统")
         << QStringLiteral("附加信息");
  spreadsheet->setHorizontalHeaderLabels(header);

  addButton =new QPushButton(this);
  addButton->setText("OK");
  cancelButton = new QPushButton(this);
  cancelButton->setText("Cancel");
  QHBoxLayout *hLay = new  QHBoxLayout;
  QVBoxLayout *vLay = new QVBoxLayout;
  hLay->addWidget(addButton);
  hLay->addWidget(cancelButton);

  vLay->addWidget(spreadsheet,1);
  vLay->addLayout(hLay);
  setLayout(vLay);
  CreateActions();
  createContextMenu();
  connect(addButton, SIGNAL(clicked(bool)), this, SLOT(Add(bool)));
  connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

}

AddDataWidget::~AddDataWidget()
{

}

void AddDataWidget::CreateActions()
{
    cutAction = new QAction(tr("Cu&t"), this);
    cutAction->setIcon(QIcon(":/images/cut.png"));
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Cut the current selection's contents "
                               "to the clipboard"));
    connect(cutAction, SIGNAL(triggered()), spreadsheet, SLOT(cut()));

    copyAction = new QAction(tr("&Copy"), this);
    copyAction->setIcon(QIcon(":/images/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copy the current selection's contents "
                                "to the clipboard"));
    connect(copyAction, SIGNAL(triggered()), spreadsheet, SLOT(copy()));


    pasteAction = new QAction(tr("&Paste"), this);
    pasteAction->setIcon(QIcon(":/images/paste.png"));
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("Paste the clipboard's contents into "
                                 "the current selection"));
    connect(pasteAction, SIGNAL(triggered()),
           spreadsheet, SLOT(paste()));

    deleteAction = new QAction(tr("&Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setStatusTip(tr("Delete the current selection's "
                                  "contents"));
    connect(deleteAction, SIGNAL(triggered()),
            spreadsheet, SLOT(del()));

    selectRowAction = new QAction(tr("Select &Row"), this);
    selectRowAction->setStatusTip(tr("Select all the cells in the "
                                     "current row"));
    connect(selectRowAction, SIGNAL(triggered()),
            spreadsheet, SLOT(selectCurrentRow()));

    selectColumnAction = new QAction(tr("Select &Column"), this);
    selectColumnAction->setStatusTip(tr("Select all the cells in the "
                                        "current column"));
    connect(selectColumnAction, SIGNAL(triggered()),
            spreadsheet, SLOT(selectCurrentColumn()));

    selectAllAction = new QAction(tr("Select &All"), this);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    selectAllAction->setStatusTip(tr("Select all the cells in the "
                                     "spreadsheet"));
    connect(selectAllAction, SIGNAL(triggered()),
            spreadsheet, SLOT(selectAll()));

}

void AddDataWidget::createContextMenu()
{
    spreadsheet->addAction(pasteAction);
    spreadsheet->addAction(cutAction);
    spreadsheet->addAction(copyAction);
    spreadsheet->addAction(deleteAction);
    spreadsheet->addAction(selectRowAction);
    spreadsheet->addAction(selectColumnAction);
    spreadsheet->addAction(selectAllAction);
    spreadsheet->setContextMenuPolicy(Qt::ActionsContextMenu);
}

/*
 * void AddDataWidget::createToolBars()
{
    QToolBar *editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addAction(pasteAction);
}
*/
