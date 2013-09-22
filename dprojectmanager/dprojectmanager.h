#ifndef DPROJECTMANAGER_H
#define DPROJECTMANAGER_H

#include <projectexplorer/iprojectmanager.h>

namespace DProjectManager {
namespace Internal {

class DProject;

class Manager : public ProjectExplorer::IProjectManager
{
 Q_OBJECT

public:
 Manager();

 virtual QString mimeType() const;
 virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString);

 void registerProject(DProject *project);
 void unregisterProject(DProject *project);

private:
 QList<DProject *> m_projects;
};

} // namespace Internal
} // namespace DProjectManager

#endif // DPROJECTMANAGER_H
