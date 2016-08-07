#include "graphicsrouteview.h"

GraphicsRouteView::GraphicsRouteView(QWidget *parent) : QWidget(parent),delta(100)
{
   items.clear();
   types.clear();
   mItems.clear();
   sItems.clear();
}

void GraphicsRouteView::setMode(QSqlTableModel *mo)
{
    mode = mo;
}

void GraphicsRouteView::updateData()
{
     items.clear();
     types.clear();

     for(int i=0; i < mode->rowCount(); i++)
     {
         QModelIndex indx = mode->index(i, 2);
         items.push_back(mode->data(indx).toString());

         indx = mode->index(i, 3);
         types.push_back(mode->data(indx).toString());
     }
     update();
}

void GraphicsRouteView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if(items.isEmpty()) return;
    painter.setPen(QPen(Qt::black, 2));
    int mCount = 0;
    int sCount = 0;
    int i;
    bool onWorking = false;
    bool onProtecting = false;



    int pre_x = 200, pre_y = 50;
    int cur_x = 200, cur_y = 50;

    QString item, preitem = "", type, pretype = "" ;
    for(i = 0; i < items.count(); i++)
    {
           item = items.at(i);
           type = types.at(i);
        // site-type item
           if(type == "site")  //节点是站点
           {
               if(item.isEmpty()) item = "UNKNOW";
               if(pretype == "site") //空站点或前一个节点也是站点
               {
                   continue;    //那就跳过吧
               }
               else
               {
                   if(preitem.isEmpty()&&pretype.isEmpty()) //前一个节点是空，说明是首个站点
                   {
                       preitem = item;
                       pretype = type;
                       painter.setPen(QPen(Qt::black, 2));
                       painter.drawText(QRect(cur_x-180,cur_y-10,160,40),item);
                       painter.drawRoundRect(QRect(cur_x-10,cur_y-10, 20, 20));
                   }
                   if(pretype == "piple")
                   {
                       if(onWorking)
                       {
                           mItems.append(item);
                           mTypes.append(type);
                       }
                       else if(onProtecting)
                       {
                           sItems.append(item);
                           sTypes.append(type);
                       }else
                       {
                           painter.setPen(QPen(Qt::black, 2));
                           painter.drawText(QRect(cur_x-180,cur_y-10,160,40),item);
                           painter.drawRoundRect(QRect(cur_x-10,cur_y-10, 20, 20));
                           painter.drawLine(QPoint(pre_x,pre_y),QPoint(cur_x,cur_y));
                           painter.setPen(QPen(Qt::blue, 4));
                           painter.drawPoint(QPoint(pre_x, pre_y));
                           painter.drawPoint(QPoint(cur_x, cur_y));
                       }
                       pretype = type;
                       preitem = item;
                   }
                   if(pretype == "port")
                   {
                      //What to do?
                   }
               }
           }

           //mark-type item
           if(type == "mark")
           {

               if(item.contains("1:"))
               {
                 if(!onWorking && !onProtecting) fork_y = cur_y;
                 if(onProtecting)
                 {
                     drawBranches(painter);
                     onProtecting = false;
                 }
                 onWorking = true;

               }

               if(item.contains( "2:"))
               {
                   onProtecting = true;
                   onWorking =false;
               }
               pretype = type;
           }

           //piple-type item
           if(type == "piple")
           {
              if(onWorking)
              {
                  mItems.append(item);
                  mTypes.append(type);
              }else if(onProtecting)
              {
                  sItems.append(item);
                  sTypes.append(type);
              }
              else
              {
                  pre_y = cur_y;
                  cur_y +=delta;
                  painter.setPen(QPen(Qt::red, 2));
                  painter.drawText(QRect(cur_x,pre_y+delta/2-10,180,60),item);
              }
              preitem = item;
              pretype = type;
           }

           if(type == "port")
           {

               if(onWorking)
               {
                   mItems.append(item);
                   mTypes.append(type);
               }
               else if(onProtecting)
               {
                   sItems.append(item);
                   sTypes.append(type);
               }else
               {
                 if(pretype == "piple")
                 {
                   painter.setPen(QPen(Qt::black, 2));
                   painter.drawLine(QPoint(pre_x,pre_y),QPoint(cur_x,cur_y));
                   painter.setPen(QPen(Qt::blue, 4));
                   painter.drawPoint(QPoint(pre_x, pre_y));
                   painter.drawPoint(QPoint(cur_x, cur_y));
                 }
                 if(pretype == "port")
                 {
                     cur_y += delta/2;
                     painter.setPen(QPen(Qt::blue, 1));
                     painter.drawLine(QPoint(pre_x,pre_y),QPoint(cur_x,cur_y));
                     pre_y = cur_y;
                 }
                 pretype = type;
                 preitem = item;
                 painter.setPen(QPen(Qt::black, 2));
                 painter.drawText(QRect(cur_x-180,cur_y-10,160,40),item);
                 painter.drawRoundRect(QRect(cur_x-10,cur_y-10, 20, 20));
                 painter.setPen(QPen(Qt::green, 3));
                 painter.drawEllipse(QPoint(cur_x,cur_y),5,5);
               }
           }

    }
    if(onWorking || onProtecting) drawBranches(painter);
    setMinimumSize(500,i*50+100);
}

void GraphicsRouteView::drawBranches(QPainter &painter)
{
 qDebug() <<"Working Route Length:" << mItems.length();
 qDebug() <<"Protecting Route Length:" << sItems.length();
 QString item, type, end_item;
 int mDelta, sDelta;

 int pre_x, pre_y;
 int cur_x, cur_y;



 int mRow = mItems.length();
 int sRow = sItems.length();
 if(mRow ==0 || sRow == 0)
 {
     mItems.clear();
     sItems.clear();
     mTypes.clear();
     sTypes.clear();
     return;
 }
 if(mRow <= sRow)
 {
     float k = static_cast<float>(sRow)/static_cast<float>(mRow);
     mDelta = static_cast<int>(k*delta);
     sDelta = delta;
     qDebug() << mDelta;
     qDebug() << sDelta;
 }
 else
 {
     float k = static_cast<float>(mRow)/static_cast<float>(sRow);
     sDelta = static_cast<int>(k*delta);
     mDelta = delta;
 }


 cur_x = 200;
 pre_x = 200;
 pre_y = fork_y;
 cur_y = fork_y;
 QString pre_type;
 qDebug() << mItems;
 qDebug() << mTypes;

 for(int ii = 0; ii < mItems.length(); ii++)
 {
     item = mItems.at(ii);
     type = mTypes.at(ii);
     if(type == "piple")
     {
         pre_y = cur_y;
         cur_y += mDelta;
         painter.setPen(QPen(Qt::red, 2));
         painter.drawText(QRect(cur_x-180,pre_y+mDelta/2-10,160,60),item);
         pre_type = "piple";
     }
     if(type == "site")
     {
         painter.setPen(QPen(Qt::black, 2));
         painter.drawText(QRect(cur_x-180,cur_y-10,160,40),item);
         painter.drawRoundRect(QRect(cur_x-10,cur_y-10, 20, 20));
         painter.drawLine(QPoint(pre_x,pre_y),QPoint(cur_x,cur_y));
         painter.setPen(QPen(Qt::blue, 4));
         painter.drawPoint(QPoint(pre_x, pre_y));
         painter.drawPoint(QPoint(cur_x, cur_y));
         pre_type = "site";
     }
 }
 int end_y = cur_y;
 end_item = item;

//处理备用路由
 cur_x = 300;
 pre_x = 200;
 pre_y = fork_y;
 cur_y = fork_y;
 qDebug() << sItems;
 qDebug() << sTypes;
 for(int jj = 0; jj < sItems.length(); jj++)
 {
     item = sItems.at(jj);
     type = sTypes.at(jj);

     if(type == "piple")
     {
         pre_y = cur_y;
         cur_y += sDelta;
         painter.setPen(QPen(Qt::red, 2));
         painter.drawText(QRect(cur_x+10,pre_y+sDelta/2-10,160,60),item);
         pre_type = "piple";
     }

     if(type == "site")
     {
         painter.setPen(QPen(Qt::black, 2));
         painter.drawText(QRect(cur_x+10,cur_y-10,160,40),item);
         painter.drawRoundRect(QRect(cur_x-10,cur_y-10, 20, 20));
         painter.drawLine(QPoint(pre_x,pre_y),QPoint(cur_x,cur_y));
         painter.setPen(QPen(Qt::blue, 4));
         painter.drawPoint(QPoint(pre_x, pre_y));
         painter.drawPoint(QPoint(cur_x, cur_y));
         pre_x = 300;
         pre_type = "site";
     }
     if(type == "port")
     {
         if(item.contains(end_item))
         {
            if(pre_type == "port")
            {
                cur_y += delta/2;
                painter.setPen(QPen(Qt::blue, 1));
                painter.drawLine(QPoint(pre_x,pre_y),QPoint(cur_x,cur_y));
                painter.setPen(QPen(Qt::black, 2));
                painter.drawText(QRect(cur_x+10,cur_y-10,160,40),item);
                painter.drawRoundRect(QRect(cur_x-10,cur_y-10, 20, 20));
                painter.setPen(QPen(Qt::green, 3));
                painter.drawEllipse(QPoint(cur_x,cur_y),5,5);

                pre_y = cur_y;
            }else
            {
                 //pre_y = cur_y;
                 cur_x = 200;
                 cur_y = end_y;

                 painter.setPen(QPen(Qt::black, 2));
                 painter.drawText(QRect(cur_x+10,cur_y-10,160,40),item);
                 if(pre_y == fork_y)
                 {
                    QPainterPath path;
                    path.moveTo(pre_x, pre_y);

                    path.cubicTo(220, (pre_y+cur_y)/4, 300, (pre_y+cur_y)/2, cur_x, cur_y);
                    painter.drawPath(path);
                 }
                 else
                 {
                    painter.drawLine(QPoint(pre_x,pre_y),QPoint(cur_x,cur_y));
                 }
                 painter.setPen(QPen(Qt::green, 3));
                 painter.drawEllipse(QPoint(cur_x,cur_y),5,5);
                 pre_y = cur_y;
                 pre_type = "port";
                 pre_x = cur_x;
            }
         }
     }
 }

 fork_y = end_y;

 mItems.clear();
 sItems.clear();
 mTypes.clear();
 sTypes.clear();
}
