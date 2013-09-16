#ifndef DLangPROJECTCONSTANTS_H
#define DLangPROJECTCONSTANTS_H

namespace DLangProjectManager {
namespace Constants {

const char PROJECTCONTEXT[]     = "DLangProject.ProjectContext";
const char DLangMIMETYPE[]    = "text/x-dlang-project"; // ### FIXME

// Contexts
const char C_FILESEDITOR[]      = ".files Editor";
const char FILES_EDITOR_ID[]    = "QT4.FilesEditor";

const char FILES_MIMETYPE[]     = "application/vnd.qtcreator.generic.files";

// Project
const char DLangPROJECT_ID[]  = "DLangProjectManager.DLangProject";

const char HIDE_FILE_FILTER_SETTING[] = "DLangProject/FileFilter";
const char HIDE_FILE_FILTER_DEFAULT[] = "Makefile*; *.o; *.obj; *~; *.files; *.config; *.creator; *.user; *.includes; *.autosave";

const char SHOW_FILE_FILTER_SETTING[] = "DLangProject/ShowFileFilter";
const char SHOW_FILE_FILTER_DEFAULT[] = "*.d; *.di;";

} // namespace Constants
} // namespace DLangProjectManager

#endif // DLangPROJECTCONSTANTS_H
