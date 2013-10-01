#ifndef DPROJECTMANAGERCONSTANTS_H
#define DPROJECTMANAGERCONSTANTS_H

#include <QtGlobal>

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

// Build configuration
const char D_BC_ID[] = "DProjectManager.DBuildConfiguration";
const char D_BC_NAME[] = "BuildConfigurationName";

const char D_MS_ID[] = "DProjectManager.DMakeStep";
const char D_MS_DISPLAY_NAME[] = QT_TRANSLATE_NOOP("DProjectManager::Internal::DMakeStep",
																																																							"Make");
const char BUILD_TARGETS_KEY[]  = "DProjectManager.DMakeStep.BuildTargets";
const char MAKE_ARGUMENTS_KEY[] = "MakeArguments";
const char MAKE_COMMAND_KEY[]   = "MakeCommand";
const char TARGET_NAME_KEY[]    = "TargetName";
const char TARGET_DIRNAME_KEY[] = "TargetDirName";
const char TARGET_TYPE_KEY[]    = "TargetTypeName";
const char OBJ_DIRNAME_KEY[]    = "ObjDirName";

} // namespace DProjectManager
} // namespace Constants

#endif // DPROJECTMANAGERCONSTANTS_H

