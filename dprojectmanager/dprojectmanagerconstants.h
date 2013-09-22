#ifndef DPROJECTMANAGERCONSTANTS_H
#define DPROJECTMANAGERCONSTANTS_H

namespace DProjectManager {
namespace Constants {

const char DPROJECTCONTEXT[]     = "DLangProject.ProjectContext";
const char DPROJECT_MIMETYPE[]    = "text/x-dproject";

// Contexts
const char C_FILESEDITOR[]      = ".files Editor";
const char FILES_EDITOR_ID[]    = "QT4.FilesEditor";

const char FILES_MIMETYPE[]     = "application/vnd.qtcreator.generic.files";

// Project
const char DPROJECT_ID[]  = "DProjectManager.DProject";

const char HIDE_FILE_FILTER_SETTING[] = "DProject/FileFilter";
const char HIDE_FILE_FILTER_DEFAULT[] = "Makefile*; *.o; *.obj; *~; *.files; *.config; *.creator; *.user; *.includes; *.autosave";

const char SHOW_FILE_FILTER_SETTING[] = "DProject/ShowFileFilter";
const char SHOW_FILE_FILTER_DEFAULT[] = "*.d; *.di;";

} // namespace DProjectManager
} // namespace Constants

#endif // DPROJECTMANAGERCONSTANTS_H

