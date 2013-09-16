include(../../qtcreatorplugin.pri)

HEADERS = dlangproject.h \
    dlangprojectplugin.h \
    dlangprojectmanager.h \
    dlangprojectconstants.h \
    dlangprojectnodes.h \
    dlangprojectwizard.h \
    dlangprojectfileseditor.h \
    dlangmakestep.h \
    dlangbuildconfiguration.h \
    buildrunconfig.h \
    buildrunconfigwidget.h
SOURCES = dlangproject.cpp \
    dlangprojectplugin.cpp \
    dlangprojectmanager.cpp \
    dlangprojectnodes.cpp \
    dlangprojectwizard.cpp \
    dlangprojectfileseditor.cpp \
    dlangmakestep.cpp \
    dlangbuildconfiguration.cpp \
    buildrunconfig.cpp \
    buildrunconfigwidget.cpp
RESOURCES += dlangproject.qrc
FORMS += dlangmakestep.ui
