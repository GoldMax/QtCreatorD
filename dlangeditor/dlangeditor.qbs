import qbs.base 1.0

import "../QtcPlugin.qbs" as QtcPlugin

QtcPlugin {
    name: "DLangEditor"

    Depends { name: "Qt.widgets" }
    Depends { name: "Core" }
    Depends { name: "TextEditor" }
    Depends { name: "ProjectExplorer" }
    Depends { name: "CppTools" }
    Depends { name: "CPlusPlus" }

    files: [
        "dlangautocompleter.cpp",
        "dlangautocompleter.h",
        "dlangcompletionassist.cpp",
        "dlangcompletionassist.h",
        "dlangeditor.cpp",
        "dlangeditor.h",
        "dlangeditor.qrc",
        "dlangeditorconstants.h",
        "dlangeditoreditable.cpp",
        "dlangeditoreditable.h",
        "dlangeditorfactory.cpp",
        "dlangeditorfactory.h",
        "dlangeditorplugin.cpp",
        "dlangeditorplugin.h",
        "dlangfilewizard.cpp",
        "dlangfilewizard.h",
        "dlanghighlighter.cpp",
        "dlanghighlighter.h",
        "dlanghighlighterfactory.cpp",
        "dlanghighlighterfactory.h",
        "dlanghoverhandler.cpp",
        "dlanghoverhandler.h",
        "dlangindenter.cpp",
        "dlangindenter.h",
        "reuse.cpp",
        "reuse.h",
    ]
}
