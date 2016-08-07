#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T22:58:19
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CircuitViewer
TEMPLATE = app
RC_ICONS = images\wx.ico

SOURCES += main.cpp\
        mainwindow.cpp \
    spreadsheet.cpp \
    cell.cpp \
    adddatawidget.cpp \
    rttextviewwidget.cpp \
    configimport.cpp \
    graphicsrouteview.cpp

HEADERS  += mainwindow.h \
    database.h \
    spreadsheet.h \
    cell.h \
    adddatawidget.h \
    rttextviewwidget.h \
    xlsx.h \
    configimport.h \
    graphicsrouteview.h

INCLUDEPATH += $$PWD/../Qtxlsx/inc \

DISTFILES +=

RESOURCES += \
    circuitviewer.qrc

CONFIG(debug, debug|release){
LIBS += -L$$PWD/../Qtxlsx/lib -lqtxlsxd
}else{
LIBS += -L$$PWD/../Qtxlsx/lib -lqtxlsx
}

FORMS +=
