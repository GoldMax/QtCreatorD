DEFINES += DPROJECTMANAGER_LIBRARY

# DProjectManager files

SOURCES += dprojectmanagerplugin.cpp \
    dprojectmanager.cpp \
    dproject.cpp \
    dprojectnodes.cpp \
    dprojectwizard.cpp \
    dbuildconfiguration.cpp \
    dmakestep.cpp \
    drunconfiguration.cpp

HEADERS += dprojectmanagerplugin.h \
        dprojectmanager_global.h \
        dprojectmanagerconstants.h \
    dprojectmanager.h \
    dproject.h \
    dprojectnodes.h \
    dprojectwizard.h \
    dbuildconfiguration.h \
    dmakestep.h \
    drunconfiguration.h

# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/opt/qt-creator

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/opt/build-qtcreator-Desktop_Qt_5_1_1_GCC_64bit-Debug

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on Mac
# USE_USER_DESTDIR = yes

PROVIDER = GoldMax

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

RESOURCES += \
    dprojectmanager.qrc

OTHER_FILES += \
    DProjectManager.mimetypes.xml

FORMS += \
    dmakestep.ui



