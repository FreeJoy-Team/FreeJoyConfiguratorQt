QT += widgets testlib
CONFIG += c++17 testcase
TARGET = bcd-tests
INCLUDEPATH += ../src ../src/widgets ../src/widgets/pins
SOURCES += \
    bcd-tests.cpp \
    ../src/deviceconfig.cpp \
    ../src/stm_main.c \
    ../src/configtofile.cpp \
    ../src/reportconverter.cpp \
    ../src/widgets/centered_cbox.cpp \
    ../src/widgets/pins/pincombobox.cpp \
    ../src/widgets/pins/pinconfig.cpp \
    ../src/widgets/pins/currentconfig.cpp \
    ../src/widgets/pins/pinsbluepill.cpp \
    ../src/widgets/pins/pinscontrlite.cpp \
    ../src/widgets/pins/pintypehelper.cpp
HEADERS += \
    ../src/deviceconfig.h \
    ../src/stm_main.h \
    ../src/configtofile.h \
    ../src/reportconverter.h \
    ../src/widgets/centered_cbox.h \
    ../src/widgets/pins/pincombobox.h \
    ../src/widgets/pins/pinconfig.h \
    ../src/widgets/pins/currentconfig.h \
    ../src/widgets/pins/pinsbluepill.h \
    ../src/widgets/pins/pinscontrlite.h \
    ../src/widgets/pins/pintypehelper.h
FORMS += \
    ../src/widgets/pins/pincombobox.ui \
    ../src/widgets/pins/pinconfig.ui \
    ../src/widgets/pins/currentconfig.ui \
    ../src/widgets/pins/pinsbluepill.ui \
    ../src/widgets/pins/pinscontrlite.ui \
    ../src/widgets/pins/pintypehelper.ui
RESOURCES += ../src/resources.qrc
