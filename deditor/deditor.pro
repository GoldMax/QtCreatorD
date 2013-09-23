DEFINES += DEDITOR_LIBRARY

# DEditor files

SOURCES += deditorplugin.cpp \
    deditorfactory.cpp \
    dfilewizard.cpp \
    dtexteditor.cpp \
    dhoverhandler.cpp \
    dcompletionassist.cpp \
    qcdassist.cpp

HEADERS += deditorplugin.h \
        deditor_global.h \
        deditorconstants.h \
    deditorfactory.h \
    dfilewizard.h \
    dtexteditor.h \
    dhoverhandler.h \
    dcompletionassist.h \
    qcdassist.h

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
    deditor.qrc
