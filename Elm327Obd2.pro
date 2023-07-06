#-------------------------------------------------
#
# Project created by QtCreator 2019-08-21T17:33:32
#
#-------------------------------------------------

QT += core gui
QT += bluetooth network
QT += concurrent


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Elm327Obd2
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


CONFIG += c++17

SOURCES += \
        connectionmanager.cpp \
        elm.cpp \
        elmblesocket.cpp \
        elmtcpsocket.cpp \
        global.cpp \
        main.cpp \
        mainwindow.cpp \
        obdgauge.cpp \
        obdscan.cpp \
        qcgaugewidget.cpp \
        settingsmanager.cpp

HEADERS += \
        connectionmanager.h \
        elm.h \
        elmblesocket.h \
        elmtcpsocket.h \
        global.h \
        mainwindow.h \
        obdgauge.h \
        obdscan.h \
        qcgaugewidget.h \
        settingsmanager.h

FORMS += \
        mainwindow.ui \
        obdgauge.ui \
        obdscan.ui

RESOURCES += \
    resources.qrc    

ios {
    QMAKE_INFO_PLIST = ios/Info.plist
    QMAKE_ASSET_CATALOGS = $$PWD/ios/Images.xcassets
    QMAKE_ASSET_CATALOGS_APP_ICON = "AppIcon"
}

android{
    QT += androidextras
    DISTFILES += \
        android/AndroidManifest.xml \
        android/build.gradle \
        android/gradle/wrapper/gradle-wrapper.jar \
        android/gradle/wrapper/gradle-wrapper.properties \
        android/gradlew \
        android/gradlew.bat \
        android/res/values/libs.xml

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}

DISTFILES += \
    icons/app_ico.ico \
    icons/app_ico.png \
    ios/Images.xcassets/AppIcon.appiconset/Contents.json \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-20x20@1x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-20x20@2x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-20x20@3x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-29x29@1x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-29x29@2x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-29x29@3x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-40x40@1x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-40x40@2x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-40x40@3x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-60x60@2x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-60x60@3x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-76x76@1x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-76x76@2x.png \
    ios/Images.xcassets/AppIcon.appiconset/Icon-App-83.5x83.5@2x.png \
    ios/Images.xcassets/AppIcon.appiconset/ItunesArtwork@2x.png \
    ios/Images.xcassets/iTunesArtwork@1x.png \
    ios/Images.xcassets/iTunesArtwork@2x.png \
    ios/Images.xcassets/iTunesArtwork@3x.png

ANDROID_ABIS = armeabi-v7a

#  python3 -m elm -s car -n 35000
