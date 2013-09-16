#ifndef DLANGPROJECTMANAGER_H
#define DLANGPROJECTMANAGER_H

#include <projectexplorer/iprojectmanager.h>

namespace DLangProjectManager {
namespace Internal {

class DLangProject;

class Manager : public ProjectExplorer::IProjectManager
{
 Q_OBJECT

public:
 Manager();

 virtual QString mimeType() const;
 virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString);

 void registerProject(DLangProject *project);
 void unregisterProject(DLangProject *project);

private:
 QList<DLangProject *> m_projects;
};

} // namespace Internal
} // namespace DLangProjectManager

#endif // DLANGPROJECTMANAGER_H
