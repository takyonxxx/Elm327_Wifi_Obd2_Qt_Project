#-------------------------------------------------
#
# Project created by QtCreator 2019-08-21T17:33:32
#
#-------------------------------------------------

QT += core gui
QT += bluetooth network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Elm327Obd2
TEMPLATE = app

win32:RC_ICONS += $$PWD\icons\app_ico.ico

ios {
    message("* Using settings for Ios.")
    QMAKE_INFO_PLIST = ios/Info.plist
    QMAKE_ASSET_CATALOGS = $$PWD/ios/Images.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"
}

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
        bluetoothmanager.cpp \
        elm.cpp \
        main.cpp \
        mainwindow.cpp \
        methods.cpp \
        networkmanager.cpp \
        obdgauge.cpp \
        obdscan.cpp \
        qcgaugewidget.cpp

HEADERS += \
        bluetoothmanager.h \
        elm.h \
        mainwindow.h \
        methods.h \
        networkmanager.h \
        obdgauge.h \
        obdscan.h \
        pid.h \
        qcgaugewidget.h

FORMS += \
        mainwindow.ui \
        obdgauge.ui \
        obdscan.ui

RESOURCES += \
    resources.qrc

android {
    message("* Using settings for Android.")
    QT += androidextras

    DISTFILES += \
        android/AndroidManifest.xml \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradlew \
        android/res/values/libs.xml \
        android/build.gradle \
        android/gradle/wrapper/gradle-wrapper.properties \
        android/gradlew.bat

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}

