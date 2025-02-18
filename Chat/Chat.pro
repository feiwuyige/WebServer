QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
RC_ICONS = tao.ico
DESTDIR = ./bin
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adduseritem.cpp \
    bubbleframe.cpp \
    chatdialog.cpp \
    chatitembase.cpp \
    chatpage.cpp \
    chatuserlist.cpp \
    chatuserwid.cpp \
    chatview.cpp \
    clickedbtn.cpp \
    clickedlabel.cpp \
    customizeedit.cpp \
    global.cpp \
    httpmanager.cpp \
    listitembase.cpp \
    loadingdlg.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    messagetextedit.cpp \
    picturebubble.cpp \
    registerdialog.cpp \
    resetdialog.cpp \
    searchlist.cpp \
    statewidget.cpp \
    tcpmgr.cpp \
    textbubble.cpp \
    timerbtn.cpp \
    userdata.cpp \
    usermgr.cpp

HEADERS += \
    adduseritem.h \
    bubbleframe.h \
    chatdialog.h \
    chatitembase.h \
    chatpage.h \
    chatuserlist.h \
    chatuserwid.h \
    chatview.h \
    clickedbtn.h \
    clickedlabel.h \
    customizeedit.h \
    global.h \
    httpmanager.h \
    listitembase.h \
    loadingdlg.h \
    logindialog.h \
    mainwindow.h \
    messagetextedit.h \
    picturebubble.h \
    registerdialog.h \
    resetdialog.h \
    searchlist.h \
    singleton.h \
    statewidget.h \
    tcpmgr.h \
    textbubble.h \
    timerbtn.h \
    userdata.h \
    usermgr.h

FORMS += \
    adduseritem.ui \
    chatdialog.ui \
    chatpage.ui \
    chatuserwid.ui \
    loadingdlg.ui \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    resetdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    config.ini

win32:CONFIG(debug, debug | release)
{
    # target file
    TargetConfig = $${PWD}/config.ini
    TargetConfig = $$replace(TargetConfig, /, \\)
    # output directory
    OutputDir =  $${OUT_PWD}/$${DESTDIR}
    OutputDir = $$replace(OutputDir, /, \\)
    # copy
    QMAKE_POST_LINK += $$QMAKE_COPY_DIR \"$$TargetConfig\" \"$$OutputDir\" > log.txt
    #QMAKE_POST_LINK += cmd /C copy \"$$TargetConfig\" \"$$OutputDir\" > log.txt
    #QMAKE_POST_LINK += cmd /C copy $$shell_path($$TargetConfig) $$shell_path($$OutputDir)
}


