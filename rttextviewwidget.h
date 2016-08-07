#ifndef RTTEXTVIEWDLG_H
#define RTTEXTVIEWDLG_H

#include <QtWidgets>



class RtTextViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RtTextViewWidget(QWidget *parent = 0);
    ~RtTextViewWidget();
    void setRtText(QString &content);
private:
    QPlainTextEdit *RtTextView;
    QAction *pasteAction;
};

#endif // RTTEXTVIEWDLG_H
