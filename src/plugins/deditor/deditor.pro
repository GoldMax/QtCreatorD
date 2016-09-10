DEFINES += DEDITOR_LIBRARY

# DEditor files

SOURCES += deditorplugin.cpp \
				deditorfactory.cpp \
				dfilewizard.cpp \
				dhoverhandler.cpp \
				dcompletionassist.cpp \
				qcdassist.cpp \
				deditorhighlighter.cpp \
    dindenter.cpp \
    dautocompleter.cpp \
    dsnippetprovider.cpp

HEADERS += deditorplugin.h \
								deditor_global.h \
								deditorconstants.h \
				deditorfactory.h \
				dfilewizard.h \
				dhoverhandler.h \
				dcompletionassist.h \
				qcdassist.h \
				deditorhighlighter.h \
    dindenter.h \
    dautocompleter.h \
    dsnippetprovider.h

# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/opt/Qt/qt-creator-opensource-src-4.1.0

## set the QTC_BUILD environment variable to override the setting here
#IDE_BUILD_TREE = $$(QTC_BUILD)
#isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/opt/Qt/build-qtcreator-Debug

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on Mac
# USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = DEditor
QTC_LIB_DEPENDS += \
				utils \
				cplusplus
QTC_PLUGIN_DEPENDS += \
				coreplugin \
				texteditor \
				cpptools

QTC_PLUGIN_RECOMMENDS += \
				# optional plugin dependencies. nothing here at this time
###### End _dependencies.pri contents ######


include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

RESOURCES += \
				deditor.qrc

OTHER_FILES += \
				DEditor.mimetypes.xml


#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libs/cplusplus/release/ -lCPlusPlus
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libs/cplusplus/debug/ -lCPlusPlus
#else:unix: LIBS += -L$$OUT_PWD/../../libs/cplusplus/ -lCPlusPlus

#INCLUDEPATH += $$PWD/../../libs/cplusplus
#DEPENDPATH += $$PWD/../../libs/cplusplus
