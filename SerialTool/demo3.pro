QT       += \
        core gui \
        widgets \
        serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


#包含第三方串口库 (后面发现根本用不到，官方的库目前看来已经够用了)
#include(3rd_qextserialport/3rd_qextserialport.pri)


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
    main.cpp \
    serialthread.cpp \
    widget.cpp \
#    ./qextserialport/qextserialport.cpp
#    ./qextserialport/qextserialport_win.cpp



HEADERS += \
    serialthread.h \
    widget.h \
#    ./qextserialport/qextserialport.h \
#    ./qextserialport/qextserialport_global.h \
#    ./qextserialport/qextserialport_p.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin


!isEmpty(target.path): INSTALLS += target

RC_FILE += logo.rc


RESOURCES += \
    image.qrc
