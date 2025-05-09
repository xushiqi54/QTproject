QT       += core gui multimedia multimediawidgets mqtt network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    add_equipment_dialog.cpp \
    login_diolog.cpp \
    main.cpp \
    my_mqtt_client.cpp \
    my_sql.cpp \
    pullflowthread.cpp \
    widget.cpp

HEADERS += \
    add_equipment_dialog.h \
    login_diolog.h \
    my_mqtt_client.h \
    my_sql.h \
    pullflowthread.h \
    widget.h

FORMS += \
    ImppTs.ui \
    add_equipment_dialog.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/1.qrc

DISTFILES += \
    res/24gl-extractLeft.png \
    res/3.1 好友.png \
    res/B5_icon.png \
    res/background.png \
    res/bk.png \
    res/button.png \
    res/button2.png \
    res/clutchWin.png \
    res/csgo.png \
    res/dataplayer.png \
    res/ez_play.png \
    res/individual.png \
    res/more.png \
    res/play_start.png \
    res/search_button.png \
    res/search_button_2.png \
    res/sqrs.png \
    res/ys.png \
    res/yx.png \
    res/zs.png \
    res/zx.png \
    res/最小化.png


LIBS += -L$$PWD/ffmpeg/lib/x64 -lavcodec \
                -lavdevice \
                -lavfilter \
                -lavformat \
                -lavutil \
                -lpostproc \
                -lswresample \
                -lswscale

INCLUDEPATH += $$PWD/ffmpeg/include
DEPENDPATH += $$PWD/ffmpeg/include




