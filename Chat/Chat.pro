QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
RC_ICONS = tao.ico
DESTDIR = ./bin
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clickedlabel.cpp \
    global.cpp \
    httpmanager.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    registerdialog.cpp \
    resetdialog.cpp \
    tcpmgr.cpp \
    timerbtn.cpp

HEADERS += \
    clickedlabel.h \
    global.h \
    httpmanager.h \
    logindialog.h \
    mainwindow.h \
    registerdialog.h \
    resetdialog.h \
    singleton.h \
    tcpmgr.h \
    timerbtn.h

FORMS += \
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


