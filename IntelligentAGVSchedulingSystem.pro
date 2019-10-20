QT       += core gui network

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
    AgvBase.cpp \
    ArmAgv.cpp \
    ForkAgv.cpp \
    LiftingAgv.cpp \
    ProtocolBase.cpp \
    ProtocolPlc.cpp \
    ProtocolStm32.cpp \
    PullAgv.cpp \
    RfidBase.cpp \
    SubmersibleAgv.cpp \
    TransferAgv.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AgvBase.h \
    ArmAgv.h \
    ForkAgv.h \
    LiftingAgv.h \
    ProtocolBase.h \
    ProtocolPlc.h \
    ProtocolStm32.h \
    PullAgv.h \
    RfidBase.h \
    SubmersibleAgv.h \
    TransferAgv.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../Document/3206c6b60775ba2f86cc0aaf13fcda8.png \
    Document/IntelligentAGVSchedulingSystem.mdj \
    Document/仓库平面布局图（2.3横梁不含一区货架）V3.0-20191021.dwg \
    Document/分布式智能AGV调度系统通信协议.docx
