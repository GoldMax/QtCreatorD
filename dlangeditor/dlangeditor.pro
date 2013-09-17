include(../../qtcreatorplugin.pri)

DEFINES += \
    DLANGEDITOR_LIBRARY

HEADERS += \
dlangeditorplugin.h \
    _dlangeditorconstants.h \
    dlangeditorfactory.h \
    dlangfilewizard.h \
    dlangtexteditor.h

SOURCES += \
dlangeditorplugin.cpp \
    dlangeditorfactory.cpp \
    dlangfilewizard.cpp \
    dlangtexteditor.cpp

RESOURCES += dlangeditor.qrc

OTHER_FILES += \
    dlangeditor_dependencies.pri
