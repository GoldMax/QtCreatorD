include(../../qtcreatorplugin.pri)

DEFINES += DPROJECTMANAGER_LIBRARY

# DProjectManager files

SOURCES += dprojectmanagerplugin.cpp \
				dproject.cpp \
				dprojectnodes.cpp \
				dbuildconfiguration.cpp \
				dmakestep.cpp \
				drunconfiguration.cpp \
				dprojectwizard.cpp

HEADERS += dprojectmanagerplugin.h \
								dprojectmanager_global.h \
								dprojectmanagerconstants.h \
				dprojectnodes.h \
				dproject.h \
				dbuildconfiguration.h \
				dmakestep.h \
				drunconfiguration.h \
				dprojectwizard.h


QTC_PLUGIN_NAME = DProjectManager
QTC_LIB_DEPENDS += \
				extensionsystem \
				utils

QTC_PLUGIN_DEPENDS += \
				projectexplorer \
				coreplugin \
				deditor

QTC_PLUGIN_RECOMMENDS += \
				# optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######


RESOURCES += \
				dprojectmanager.qrc

OTHER_FILES += \
				DProjectManager.mimetypes.xml

FORMS += \
				dmakestep.ui

