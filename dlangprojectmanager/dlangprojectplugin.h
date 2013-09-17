#ifndef DLangPROJECTPLUGIN_H
#define DLangPROJECTPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QObject>

namespace ProjectExplorer {
class Project;
class Node;
}

namespace DLangProjectManager {
namespace Internal {

class ProjectFilesFactory;

class DLangProjectPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DLangProjectManager.json")

public:
    DLangProjectPlugin();
    ~DLangProjectPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

private:
    ProjectExplorer::Project *m_contextMenuProject;
};

} // namespace Internal
} // namespace DLangProject

#endif // DLangPROJECTPLUGIN_H
