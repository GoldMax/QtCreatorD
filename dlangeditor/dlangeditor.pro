include(../../qtcreatorplugin.pri)

DEFINES += \
    DLANGEDITOR_LIBRARY

HEADERS += \
dlangeditor.h \
dlangeditorconstants.h \
dlangeditoreditable.h \
dlangeditorfactory.h \
dlangeditorplugin.h \
dlangfilewizard.h \
dlanghighlighter.h \
dlanghighlighterfactory.h \
dlangautocompleter.h \
dlangindenter.h \
dlanghoverhandler.h \
    dlangcompletionassist.h \
    reuse.h

SOURCES += \
dlangeditor.cpp \
dlangeditoreditable.cpp \
dlangeditorfactory.cpp \
dlangeditorplugin.cpp \
dlangfilewizard.cpp \
dlanghighlighter.cpp \
dlanghighlighterfactory.cpp \
dlangautocompleter.cpp \
dlangindenter.cpp \
dlanghoverhandler.cpp \
    dlangcompletionassist.cpp \
    reuse.cpp

RESOURCES += dlangeditor.qrc
