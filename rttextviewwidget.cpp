#include "rttextviewwidget.h"
//#include "ui_rttextviewdlg.h"

RtTextViewWidget::RtTextViewWidget(QWidget *parent) :
    QWidget(parent)
{
   RtTextView = new QPlainTextEdit(this);
   QVBoxLayout *thisLayout = new QVBoxLayout();
   thisLayout->addWidget(RtTextView);
   thisLayout->setContentsMargins(0,0,2,0);
   setLayout(thisLayout);
}

RtTextViewWidget::~RtTextViewWidget()
{
  // if(RtTextView) delete RtTextView;
}

void RtTextViewWidget::setRtText(QString &content)
{
    RtTextView->setPlainText(content);
}
