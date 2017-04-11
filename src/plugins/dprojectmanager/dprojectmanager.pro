DEFINES += DPROJECTMANAGER_LIBRARY

# DProjectManager files

SOURCES += dprojectmanagerplugin.cpp \
				dproject.cpp \
				dprojectnodes.cpp \
				dprojectmanager.cpp \
				dbuildconfiguration.cpp \
				dmakestep.cpp \
				drunconfiguration.cpp \
				dprojectwizard.cpp

HEADERS += dprojectmanagerplugin.h \
								dprojectmanager_global.h \
								dprojectmanagerconstants.h \
				dprojectnodes.h \
				dproject.h \
				dprojectmanager.h \
				dbuildconfiguration.h \
				dmakestep.h \
				drunconfiguration.h \
				dprojectwizard.h

# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/opt/Qt/qt-creator-opensource-src-4.2.1

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

QTC_PLUGIN_NAME = DProjectManager
QTC_LIB_DEPENDS += \
				# nothing here at this time

QTC_PLUGIN_DEPENDS += \
				coreplugin \
				deditor

QTC_PLUGIN_RECOMMENDS += \
				# optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

RESOURCES += \
				dprojectmanager.qrc

OTHER_FILES += \
				DProjectManager.mimetypes.xml

FORMS += \
				dmakestep.ui

