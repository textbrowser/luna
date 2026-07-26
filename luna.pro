macx {
dmg.commands        = make install && \
                      hdiutil create luna.d.dmg -srcfolder Luna.d
QMAKE_EXTRA_TARGETS += dmg
}

unix {
purge.commands      = find . -name \'*~\' -exec rm {} \;
QMAKE_EXTRA_TARGETS += purge
}

CONFIG      += qt release warn_on
DEFINES     +=
LANGUAGE    = C++
QMAKE_CLEAN += luna
QT          += concurrent gui sql widgets

contains(QMAKE_HOST.arch, armv7l) {
QMAKE_CXXFLAGS_RELEASE += -march=armv7
}

QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE -= -O2

android {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-qual \
                          -Wenum-compare \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat=2 \
                          -Wno-c++20-attribute-extensions \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIC \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++20
} else:freebsd-* {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Werror \
                          -Wextra \
                          -Wformat=2 \
                          -Wno-c++20-attribute-extensions \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++20
} else:macx {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wenum-compare \
                          -Wextra \
                          -Wformat=2 \
                          -Wno-c++20-attribute-extensions \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -std=c++20
QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.0
} else:win32 {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wenum-compare \
                          -Wextra \
                          -Wformat=2 \
                          -Wl,-z,relro \
                          -Wno-class-memaccess \
                          -Wno-deprecated-copy \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIE \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++20
} else {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Wenum-compare \
                          -Wextra \
                          -Wfloat-equal \
                          -Wformat=2 \
                          -Wformat-overflow=2 \
                          -Wl,-z,relro \
                          -Wlogical-op \
                          -Wno-class-memaccess \
                          -Wno-deprecated-copy \
                          -Wold-style-cast \
                          -Woverloaded-virtual \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -Wstringop-overflow=4 \
                          -Wundef \
                          -fPIE \
                          -fstack-protector-all \
                          -funroll-loops \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++20
contains(QMAKE_HOST.arch, ppc) {
QMAKE_CXXFLAGS_RELEASE -= -Wformat-overflow=2 \
                          -Wstringop-overflow=4
}
}

greaterThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE += -std=c++20
QMAKE_CXXFLAGS_RELEASE -= -std=c++11
}

macx {
QMAKE_DISTCLEAN += -r Luna.d
}

win32 {
QMAKE_DISTCLEAN += rm -fr .qmake* Temporary debug release
} else {
QMAKE_DISTCLEAN += -f .qmake* -fr Temporary
}

QMAKE_STRIP = echo

macx {
ICON = Icons/luna.icns
}

win32 {
RC_FILE = Icons/luna.rc
}

INCLUDEPATH += Source

macx {
LIBS              += -framework AppKit -framework Cocoa
OBJECTIVE_HEADERS += Source/CocoaInitializer.h
OBJECTIVE_SOURCES += Source/CocoaInitializer.mm
} else {
LIBS +=
}

MOC_DIR     = Temporary/moc
OBJECTS_DIR = Temporary/obj
RCC_DIR     = Temporary/rcc
RESOURCES   = Documentation/documentation.qrc Icons/icons.qrc
UI_DIR      = Temporary/ui

FORMS   += UI/luna-calendar.ui
HEADERS += Source/luna-calendar.h
SOURCES += Source/luna-calendar.cc

PROJECTNAME = Luna
TARGET      = Luna
TEMPLATE    = app

android {
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android
DISTFILES += Android/AndroidManifest.xml \
             Android/build.gradle \
             Android/res/values/libs.xml
android: include(/home/saturn/Android/Sdk/android_openssl/openssl.pri)
}

# Installation Procedures

macx {
luna.extra        = cp -r ./Luna.app Luna.d/.
luna.path         = Luna.d
macdeployqt.extra = $$[QT_INSTALL_BINS]/macdeployqt Luna.d/Luna.app \
                    -executable=Luna.d/Luna.app/Contents/MacOS/Luna
macdeployqt.path  = Luna.app
preinstall.extra  = rm -fr Luna.d/Luna.app
preinstall.path   = Luna.d

INSTALLS = preinstall \
           luna \
           macdeployqt
}

win32 {
bat.files         = Distributions/luna-darkmode.bat
bat.path          = release\\.
plugins.files     = $$[QT_INSTALL_PLUGINS]\\*
plugins.path      = release\\plugins\\.
qt.files          = Qt\\qt.conf
qt.path           = release\\.
qtlibraries.files = $$[QT_INSTALL_BINS]\\Qt*Core.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Gui.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Sql.dll \
                    $$[QT_INSTALL_BINS]\\Qt*Widgets.dll \
                    $$[QT_INSTALL_BINS]\\libgcc_s_seh-*.dll \
                    $$[QT_INSTALL_BINS]\\libstdc++-*.dll \
                    $$[QT_INSTALL_BINS]\\libwinpthread-*.dll
qtlibraries.path = release\\.

INSTALLS = bat \
           plugins \
           qt \
           qtlibraries
}

android {
DISTFILES += Android/AndroidManifest.xml \
             Android/build.gradle \
             Android/gradle.properties \
             Android/gradle/wrapper/gradle-wrapper.jar \
             Android/gradle/wrapper/gradle-wrapper.properties \
             Android/gradlew \
             Android/gradlew.bat \
             Android/res/values/libs.xml \
             Android/res/xml/qtprovider_paths.xml
}
