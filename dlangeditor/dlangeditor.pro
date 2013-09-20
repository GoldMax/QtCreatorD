include(../../qtcreatorplugin.pri)

DEFINES += \
    DLANGEDITOR_LIBRARY

HEADERS += \
dlangeditorplugin.h \
    _dlangeditorconstants.h \
    dlangeditorfactory.h \
    dlangfilewizard.h \
    dlangtexteditor.h \
    dlangcompletionassist.h \
    dlanghoverhandler.h \
    dlangindenter.h \
    qcdassist.h

SOURCES += \
dlangeditorplugin.cpp \
    dlangeditorfactory.cpp \
    dlangfilewizard.cpp \
    dlangtexteditor.cpp \
    dlangcompletionassist.cpp \
    dlanghoverhandler.cpp \
    dlangindenter.cpp \
    qcdassist.cpp

RESOURCES += dlangeditor.qrc

OTHER_FILES += \
    dlangeditor_dependencies.pri

PROVIDER = GoldMax
