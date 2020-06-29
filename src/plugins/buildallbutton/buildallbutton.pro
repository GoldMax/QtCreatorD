DEFINES += \
			BUILDALLBUTTON_LIBRARY

SOURCES += \
			buildallbuttonplugin.cpp

HEADERS += \
			buildallbuttonplugin.h \
			buildallbutton_global.h \
			buildallbuttonconstants.h

# Qt Creator linking
QTC_LIB_DEPENDS += \
				extensionsystem \
				utils
QTC_PLUGIN_DEPENDS += \
				coreplugin

include(../../qtcreatorplugin.pri)
