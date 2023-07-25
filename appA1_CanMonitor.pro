#-------------------------------------------------
#
# Project created by QtCreator 2022-05-02T15:58:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Installation Folder
TARGET      = appA1_CanMonitor

target.path         = /opt/oxApps/$${TARGET}/ReleaseIMX8
INSTALLS           += target

dataFolder.files    = Data/*
dataFolder.path     = /opt/oxApps/$${TARGET}/Data
INSTALLS           += dataFolder

manifestFile.files  = manifest.json
manifestFile.files += icon.png
manifestFile.path   = /opt/oxApps/$${TARGET}
INSTALLS           += manifestFile

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        Source/mainwindow.cpp \
        Source/appManLink.cpp \
        Source/dataPool.cpp

HEADERS += \
        Source/mainwindow.h \
        Source/appManLink.h \
        Source/dataPool.h

FORMS += \
        Source/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/pogApps/$${TARGET}/ReleaseIMX8
!isEmpty(target.path): INSTALLS += target




##############################################################################################
#       BUILD CONFIG

unix:{
    linux-g++{
            message(Compiling for Linux X64-QT5)
            RELEASE_DIR = ReleaseX64/
#            message(Compiling for Linux X86-QT5)
#            RELEASE_DIR = ReleaseX86_QT5/
            DEFINES += __VIRTUALMACHINE
            DEFINES += TARGET_X64
    }
    linux-imx6-g++{
            message(Compiling for IMX6)
            RELEASE_DIR = ReleaseVS/
            DEFINES += TARGET_VS
    }
    linux-imx8-g++{
            message(Compiling for IMX8)
            RELEASE_DIR = ReleaseIMX8/
            CONFIG  += c++11
            DEFINES += TARGET_IMX8
    }
    linux-oe-g++{
            message(Compiling for Linux ARM-VS)
            RELEASE_DIR = ReleaseVS/
            DEFINES += TARGET_VS
    }
    linux-arm-none-gnueabi-g++ { # Qt5 LinX version iMX5
            message(Compiling for Linux ARM-XS IMX5)
            RELEASE_DIR = ReleaseXS/
            DEFINES += TARGET_XS
    }
}

##############################################################################################
#       FRAMEWORK IMPORT



# Libraries
LIBS            += -L$$PWD/../../../oxLibs/oxCommon/$${RELEASE_DIR} -loxCommon
PRE_TARGETDEPS  +=   $$PWD/../../../oxLibs/oxCommon/$${RELEASE_DIR}liboxCommon.a

INCLUDEPATH     += $$PWD/../../../oxLibs/oxCommon
DEPENDPATH      += $$PWD/../../../oxLibs/oxCommon


RESOURCES += \
    Data/Images/images.qrc

