#ifndef ADDDATAWIDGET_H
#define ADDDATAWIDGET_H

#include <QtWidgets>
#include "spreadsheet.h"


class AddDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddDataWidget(QWidget *parent = 0);
    ~AddDataWidget();
private:
    void CreateActions();
    void createContextMenu();
    //void createToolBars();
private:
    Spreadsheet *spreadsheet;
    QPushButton *addButton;
    QPushButton *cancelButton;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *selectRowAction;
    QAction *selectColumnAction;
    QAction *selectAllAction;
};

#endif // ADDDATAWIDGET_H
