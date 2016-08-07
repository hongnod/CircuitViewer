#ifndef GRAPHICSROUTEVIEW_H
#define GRAPHICSROUTEVIEW_H

#include <QtWidgets>
#include <QtSql>

class GraphicsRouteView : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsRouteView(QWidget *parent = 0);
    void draw(QPainter *painter);
    void updateData();
    void setMode(QSqlTableModel *mo);
protected:
    void paintEvent(QPaintEvent *event);
signals:

public slots:

private:
    QStringList items;
    QStringList types;
    QSqlTableModel *mode;
    QStringList mItems, mTypes;
    QStringList sItems, sTypes;
    int fork_y;
    int delta;
    void drawBranches(QPainter &painter);
};

/*
class NodeItem
{
public:
    QString item;
    QString type;
};


*/
#endif // GRAPHICSROUTEVIEW_H
