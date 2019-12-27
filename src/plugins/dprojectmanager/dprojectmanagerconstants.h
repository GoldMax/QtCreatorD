#ifndef DPROJECTMANAGERCONSTANTS_H
#define DPROJECTMANAGERCONSTANTS_H

namespace DProjectManager {
namespace Constants {

// Contexts
const char DPROJECT_ID[] = "DProjectManager.DProject";
const char DPROJECTFILE_ID[] = "DProjectManager.DProjectFile";
const char DPROJECTGROUP_ID[] = "DProjectManager.DProjectGroup";

// MIME types:
const char DPROJECT_MIMETYPE[] = "text/x-dproject";
const char DPROJECTGROUP_MIMETYPE[] = "text/x-dproject-group";

const char ICON_DPROJECT[] = ":/dprojectmanager/dlang.png";
const char ICON_DPROJECTGROUP[] = ":/dprojectmanager/dfolder_64.png";
const char ICON_D_FILE[] = ":/dprojectmanager/dmd-source.png";


const char DPROJECTCONTEXT[]     = "DLangProject.ProjectContext";

// Contexts
const char C_FILESEDITOR[]      = ".files Editor";
const char FILES_EDITOR_ID[]    = "QT4.FilesEditor";

const char FILES_MIMETYPE[]     = "application/vnd.qtcreator.generic.files";


const char HIDE_FILE_FILTER_SETTING[] = "DProject/FileFilter";
const char HIDE_FILE_FILTER_DEFAULT[] = "Makefile*; *.o; *.obj; *~; *.files; *.config; *.creator; *.user; *.includes; *.autosave";

const char SHOW_FILE_FILTER_SETTING[] = "DProject/ShowFileFilter";
const char SHOW_FILE_FILTER_DEFAULT[] = "*.d; *.di;";

// Build configuration
const char D_BC_ID[] = "DProjectManager.DBuildConfiguration";
const char D_BC_NAME[] = "BuildConfigurationName";
const char BUILD_TARGETS_KEY[]  = "DProjectManager.DMakeStep.BuildTargets";

const char D_MS_ID[] = "DProjectManager.DMakeStep";
const char D_MS_DISPLAY_NAME[] = "D make step"; //QT_TRANSLATE_NOOP("DProjectManager::Internal::DMakeStep","Make");

// Run configuration
const char BUILDRUN_CONFIG_ID[] = "ProjectExplorer.DRunConfiguration";
const char EXECUTABLE_KEY[] = "ProjectExplorer.DRunConfiguration.Executable";
const char ARGUMENTS_KEY[] = "ProjectExplorer.DRunConfiguration.Arguments";
const char WORKING_DIRECTORY_KEY[] = "ProjectExplorer.DRunConfiguration.WorkingDirectory";
const char USE_TERMINAL_KEY[] = "ProjectExplorer.DRunConfiguration.UseTerminal";

// Settings
const char INI_SOURCE_ROOT_KEY[]   = "SourceRoot";
const char INI_INCLUDES_KEY[]   = "Includes";
const char INI_LIBRARIES_KEY[]   = "Libs";
const char INI_EXTRA_ARGS_KEY[]   = "ExtraArgs";
const char INI_COMPILE_PRIORITY_KEY[]   = "CompilePriority";
const char INI_FILES_ROOT_KEY[]   = "Files";
const char INI_PROJECTS_ROOT_KEY[]   = "Projects";

//const char INI_MAKE_COMMAND_KEY[]   = "MakeCommand";
const char INI_BUILD_PRESET_KEY[]   = "BuildPreset";
const char INI_TARGET_TYPE_KEY[]    = "TargetTypeName";
const char INI_TARGET_NAME_KEY[]    = "TargetName";
const char INI_TARGET_DIRNAME_KEY[] = "TargetDirName";
const char INI_OBJ_DIRNAME_KEY[]    = "ObjDirName";
const char INI_MAKE_ARGUMENTS_KEY[] = "MakeArguments";

} // namespace DProjectManager
} // namespace Constants

#endif // DPROJECTMANAGERCONSTANTS_H

