#include "dprojectmanagerconstants.h"
#include "dprojectmanager.h"
#include "dproject.h"

#include "deditor/qcdassist.h"

#include <coreplugin/icore.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/session.h>

#include <QDebug>


namespace DProjectManager {
namespace Internal {

Manager::Manager() { }

QString Manager::mimeType() const
{
 return QLatin1String(Constants::DPROJECT_MIMETYPE);
}

ProjectExplorer::Project *Manager::openProject(const QString &fileName, QString *errorString)
{
 if (!QFileInfo(fileName).isFile())
 {
  if (errorString)
   *errorString = tr("Failed opening project '%1': Project is not a file").arg(fileName);
  return 0;
 }
 DProject* prj = new DProject(this, fileName);

 QString path = prj->rootProjectNode()->projectFilePath();
 if(path.endsWith(QLatin1String(".qcd")))
  path.chop(path.length() - path.lastIndexOf(QDir::separator()));
 QcdAssist::sendAddImportToDCD(path);

 return prj;
}

void Manager::registerProject(DProject *project)
{
 m_projects.append(project);
}

void Manager::unregisterProject(DProject *project)
{
 m_projects.removeAll(project);
}

} // namespace Internal
} // namespace DProjectManager
