DEFINES += DEDITOR_LIBRARY

# DEditor files

SOURCES += deditorplugin.cpp \
	deditor.cpp \
	deditordocument.cpp \
				dfilewizard.cpp \
	dhighlighter.cpp \
				dhoverhandler.cpp \
				dcompletionassist.cpp \
				qcdassist.cpp \
    dindenter.cpp \
    dautocompleter.cpp \
    dsnippetprovider.cpp

HEADERS += deditorplugin.h \
	deditor.h \
								deditor_global.h \
								deditorconstants.h \
	deditordocument.h \
				dfilewizard.h \
	dhighlighter.h \
				dhoverhandler.h \
				dcompletionassist.h \
				qcdassist.h \
    dindenter.h \
    dautocompleter.h \
    dsnippetprovider.h

# Qt Creator linking

QTC_PLUGIN_NAME = DEditor
QTC_LIB_DEPENDS += \
				utils \
				cplusplus
QTC_PLUGIN_DEPENDS += \
				coreplugin \
				texteditor \
				cpptools \
				cppeditor




RESOURCES += \
				deditor.qrc

OTHER_FILES +=

include(../../qtcreatorplugin.pri)

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libs/cplusplus/release/ -lCPlusPlus
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libs/cplusplus/debug/ -lCPlusPlus
#else:unix: LIBS += -L$$OUT_PWD/../../libs/cplusplus/ -lCPlusPlus

#INCLUDEPATH += $$PWD/../../libs/cplusplus
#DEPENDPATH += $$PWD/../../libs/cplusplus
