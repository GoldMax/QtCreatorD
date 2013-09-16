import qbs.base 1.0

import "../QtcPlugin.qbs" as QtcPlugin

QtcPlugin {
    name: "DLangProjectManager"

    Depends { name: "Qt.widgets" }
    Depends { name: "Core" }
    Depends { name: "CPlusPlus" }
    Depends { name: "CppTools" }
    Depends { name: "TextEditor" }
    Depends { name: "ProjectExplorer" }
    Depends { name: "Find" }
    Depends { name: "Locator" }
    Depends { name: "QtSupport" }

    files: [
        "dlangbuildconfiguration.cpp",
        "dlangbuildconfiguration.h",
        "dlangmakestep.cpp",
        "dlangmakestep.h",
        "dlangmakestep.ui",
        "dlangproject.cpp",
        "dlangproject.h",
        "dlangproject.qrc",
        "dlangprojectconstants.h",
        "dlangprojectfileseditor.cpp",
        "dlangprojectfileseditor.h",
        "dlangprojectmanager.cpp",
        "dlangprojectmanager.h",
        "dlangprojectnodes.cpp",
        "dlangprojectnodes.h",
        "dlangprojectplugin.cpp",
        "dlangprojectplugin.h",
        "dlangprojectwizard.cpp",
        "dlangprojectwizard.h"
    ]
}
