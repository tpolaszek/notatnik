QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dictionary.cpp \
    filehandling.cpp \
    grammarloader.cpp \
    main.cpp \
    mainwindow.cpp \
    recentfilesmanager.cpp \
    settingsdialog.cpp \
    settingsmanager.cpp \
    startscreen.cpp \
    syntaxhighlighter.cpp \
    texttools.cpp \
    viewmanager.cpp

HEADERS += \
    dictionary.h \
    filehandling.h \
    grammarloader.h \
    mainwindow.h \
    recentfilesmanager.h \
    settingsdialog.h \
    settingsmanager.h \
    startscreen.h \
    syntaxhighlighter.h \
    texttools.h \
    viewmanager.h

FORMS += \
    mainwindow.ui \
    settings.ui \
    startscreen.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
