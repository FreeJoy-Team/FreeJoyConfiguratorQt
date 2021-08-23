TEMPLATE = app

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += static \
          c++17

RC_ICONS = Images/icon.ico

TARGET = FreeJoyQt

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
WIN_DESKTOP

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    centered_cbox.cpp \
    converter.cpp \
    deviceconfig.cpp \
    firmwareupdater.cpp \
    hiddevice.cpp \
    main.cpp \
    mainwindow.cpp \
    mainwindow_conf.cpp \
    mousewheelguard.cpp \
    reportconverter.cpp \
    stm_main.c \
    switchbutton.cpp \
    widgets/advancedsettings.cpp \
    widgets/axes.cpp \
    widgets/axesconfig.cpp \
    widgets/axescurves.cpp \
    widgets/axescurvesconfig.cpp \
    widgets/axescurvesplot.cpp \
    widgets/axescurvesprofiles.cpp \
    widgets/axesextended.cpp \
    widgets/axestobuttonsslider.cpp \
    widgets/buttonconfig.cpp \
    widgets/buttonlogical.cpp \
    widgets/buttonphysical.cpp \
    widgets/currentconfig.cpp \
    widgets/debugwindow.cpp \
    widgets/encoders.cpp \
    widgets/encodersconfig.cpp \
    widgets/flasher.cpp \
    widgets/led.cpp \
    widgets/ledconfig.cpp \
    widgets/pincombobox.cpp \
    widgets/pinconfig.cpp \
    widgets/pinsbluepill.cpp \
    widgets/pinscontrlite.cpp \
    widgets/shiftregisters.cpp \
    widgets/shiftregistersconfig.cpp

HEADERS += \
    centered_cbox.h \
    common_defines.h \
    common_types.h \
    converter.h \
    deviceconfig.h \
    firmwareupdater.h \
    global.h \
    hidapi.h \
    hiddevice.h \
    mainwindow.h \
    mousewheelguard.h \
    reportconverter.h \
    stm_main.h \
    switchbutton.h \
    version.h \
    widgets/advancedsettings.h \
    widgets/axes.h \
    widgets/axesconfig.h \
    widgets/axescurves.h \
    widgets/axescurvesconfig.h \
    widgets/axescurvesplot.h \
    widgets/axescurvesprofiles.h \
    widgets/axesextended.h \
    widgets/axestobuttonsslider.h \
    widgets/buttonconfig.h \
    widgets/buttonlogical.h \
    widgets/buttonphysical.h \
    widgets/currentconfig.h \
    widgets/debugwindow.h \
    widgets/encoders.h \
    widgets/encodersconfig.h \
    widgets/flasher.h \
    widgets/led.h \
    widgets/ledconfig.h \
    widgets/pincombobox.h \
    widgets/pinconfig.h \
    widgets/pinsbluepill.h \
    widgets/pinscontrlite.h \
    widgets/shiftregisters.h \
    widgets/shiftregistersconfig.h

FORMS += \
    mainwindow.ui \
    widgets/advancedsettings.ui \
    widgets/axes.ui \
    widgets/axesconfig.ui \
    widgets/axescurves.ui \
    widgets/axescurvesconfig.ui \
    widgets/axesextended.ui \
    widgets/axestobuttonsslider.ui \
    widgets/buttonconfig.ui \
    widgets/buttonlogical.ui \
    widgets/buttonphysical.ui \
    widgets/currentconfig.ui \
    widgets/debugwindow.ui \
    widgets/encoders.ui \
    widgets/encodersconfig.ui \
    widgets/flasher.ui \
    widgets/led.ui \
    widgets/ledconfig.ui \
    widgets/pincombobox.ui \
    widgets/pinconfig.ui \
    widgets/pinsbluepill.ui \
    widgets/pinscontrlite.ui \
    widgets/shiftregisters.ui \
    widgets/shiftregistersconfig.ui

TRANSLATIONS += \
    FreeJoyQt_ru.ts \
    FreeJoyQt_zh_CN.ts


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc \
    styles/qdarkstyle/style.qrc \
    styles/white/qss.qrc

linux {
    QMAKE_LFLAGS += -no-pie
    LIBS += -ludev
    SOURCES += \
        linux/hidapi.c
}

win32 {
    RC_FILE = winapp.rc
    LIBS += -lhid -lsetupapi
    SOURCES += \
        windows/hidapi.c
}

macx {
    LIBS += -framework CoreFoundation -framework IOkit
    SOURCES += \
    mac/hidapi.c
}
