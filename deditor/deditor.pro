DEFINES += DEDITOR_LIBRARY

# DEditor files

SOURCES += deditorplugin.cpp \
    deditorfactory.cpp \
    dfilewizard.cpp \
    dtexteditor.cpp \
    dhoverhandler.cpp \
    dcompletionassist.cpp \
    qcdassist.cpp \
    deditorhighlighter.cpp

HEADERS += deditorplugin.h \
        deditor_global.h \
        deditorconstants.h \
    deditorfactory.h \
    dfilewizard.h \
    dtexteditor.h \
    dhoverhandler.h \
    dcompletionassist.h \
    qcdassist.h \
    deditorhighlighter.h

# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/opt/qt-creator

## set the QTC_BUILD environment variable to override the setting here
#IDE_BUILD_TREE = $$(QTC_BUILD)
#CONFIG(debug)
# isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/opt/build-qtcreator-Desktop_Qt_5_1_1_GCC_64bit-Debug
#else
# CONFIG(release):isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/opt/build-qtcreator-Desktop_Qt_5_1_1_GCC_64bit-Release

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libs/cplusplus/release/ -lCPlusPlus
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libs/cplusplus/debug/ -lCPlusPlus
else:unix: LIBS += -L$$OUT_PWD/../../libs/cplusplus/ -lCPlusPlus

INCLUDEPATH += $$PWD/../../libs/cplusplus
DEPENDPATH += $$PWD/../../libs/cplusplus
