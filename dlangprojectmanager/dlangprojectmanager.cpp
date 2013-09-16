#include "dlangprojectmanager.h"
#include "dlangprojectconstants.h"
#include "dlangproject.h"

#include <coreplugin/icore.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/session.h>

#include <QDebug>


namespace DLangProjectManager {
namespace Internal {

Manager::Manager()
{ }


QString Manager::mimeType() const
{
 return QLatin1String(Constants::DLangMIMETYPE);
}

ProjectExplorer::Project *Manager::openProject(const QString &fileName, QString *errorString)
{
 if (!QFileInfo(fileName).isFile())
 {
  if (errorString)
   *errorString = tr("Failed opening project '%1': Project is not a file").arg(fileName);
  return 0;
 }

 return new DLangProject(this, fileName);
}

void Manager::registerProject(DLangProject *project)
{
 m_projects.append(project);
}

void Manager::unregisterProject(DLangProject *project)
{
 m_projects.removeAll(project);
}

} // namespace Internal
} // namespace DLangProjectManager
