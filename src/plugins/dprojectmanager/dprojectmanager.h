#ifndef DPROJECTMANAGER_H
#define DPROJECTMANAGER_H

#include <projectexplorer/iprojectmanager.h>

namespace DProjectManager {

class DManager : public ProjectExplorer::IProjectManager
{
	Q_OBJECT

public:
	DManager();

	QString mimeType() const;
	ProjectExplorer::Project* openProject(const QString &fileName,
																																							QString *errorString);

};

} // namespace DProjectManager

#endif // DPROJECTMANAGER_H
