#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QMainWindow>
#include <QtSql>
#include <QtWidgets>
#include "rttextviewwidget.h"
class GraphicsRouteView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void updateCircuitView();
    void Add();
    void Import();
    void Export();
    //void showRoute(QModelIndex idx);
    void slot_import(const QStringList &records);
private:
    void CreateCoreView();
    void createActions();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void updateStatusBar(const QString &info);
    bool loadFile(const QString &xlsxName);
    void importRoutes(QString &cid, QString &rt);
    QSqlTableModel *circuitModel;
    QSqlTableModel *routeModel;
    QMenu *dataMenu;
    QMenu *editMenu;
    QMenu *selectSubMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;

    QToolBar *dataToolBar;
    QTableView *circuitView;
    QTableView *tableRouteView;
    GraphicsRouteView *graphicRouteView;
    RtTextViewWidget *textRouteView;
    QLabel *statusLabel;


    QAction *separatorAction;
    QAction *addAction;
    QAction *importAction;
    QAction *exportAction;

    QAction *exitAction;

    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *selectRowAction;
    QAction *selectColumnAction;
    QAction *selectAllAction;
    QAction *findAction;
    QAction *goToCellAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
};

#endif // MAINWINDOW_H
