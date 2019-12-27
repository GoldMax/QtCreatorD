#include "dprojectwizard.h"
#include "dprojectmanagerconstants.h"

#include <coreplugin/icore.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/customwizard/customwizard.h>

#include <utils/filewizardpage.h>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QCheckBox>

namespace DProjectManager {

//----------------------------------------------------------------------------
//
// DProjectWizardDialog
//
//----------------------------------------------------------------------------
DProjectWizardDialog::DProjectWizardDialog(const Core::BaseFileWizardFactory *factory,
																																											QWidget *parent) :
Core::BaseFileWizard(factory, QVariantMap(), parent)
{
	setWindowTitle(tr("Create a D language Project"));

	// first page
	m_firstPage = new Utils::FileWizardPage;
	m_firstPage->setTitle(tr("Project Name and Location"));
	m_firstPage->setFileNameLabel(tr("Project name:"));
	m_firstPage->setPathLabel(tr("Location:"));
	const int firstPageId = addPage(m_firstPage);
	wizardProgress()->item(firstPageId)->setTitle(tr("Location"));
}
QString DProjectWizardDialog::path() const
{
	return m_firstPage->path();
}
void DProjectWizardDialog::setPath(const QString &path)
{
	m_firstPage->setPath(path);
}
QString DProjectWizardDialog::projectName() const
{
	return m_firstPage->fileName();
}

//----------------------------------------------------------------------------
//
// DProjectWizard
//
//----------------------------------------------------------------------------
DProjectWizard::DProjectWizard()
{
	setSupportedProjectTypes({ Constants::DPROJECT_ID });
	setId("A.DProject");
	setDisplayName(tr("D Project"));
	setDescription(tr("Create a D language project."));
	setCategory(QLatin1String(ProjectExplorer::Constants::QT_PROJECT_WIZARD_CATEGORY));
	setDisplayCategory(QLatin1String(ProjectExplorer::Constants::QT_PROJECT_WIZARD_CATEGORY_DISPLAY));
	setIcon(QIcon(QLatin1String(":/dprojectmanager/dlogo.png")));
	setFlags(Core::IWizardFactory::PlatformIndependent);
}
Core::BaseFileWizard* DProjectWizard::create(QWidget *parent,
																														const Core::WizardDialogParameters &params) const
{
	DProjectWizardDialog *wizard = new DProjectWizardDialog(this, parent);
	wizard->setPath(params.defaultPath());

	foreach (QWizardPage *p, wizard->extensionPages())
		wizard->addPage(p);

	return wizard;
}
Core::GeneratedFiles DProjectWizard::generateFiles(const QWizard *w,
																																																									QString *errorMessage) const
{
	Q_UNUSED(errorMessage)

	const DProjectWizardDialog *wizard = qobject_cast<const DProjectWizardDialog *>(w);
	const QString projectName = wizard->projectName();
	const QDir dir(wizard->path());

	const QString projectFileName = QFileInfo(dir, projectName + QLatin1String(".qcd")).absoluteFilePath();

	Core::GeneratedFile generatedProjectFile(projectFileName);
	auto group = QLatin1String("[") + QLatin1String(Constants::INI_FILES_ROOT_KEY) +
														QLatin1String("]\n");
	generatedProjectFile.setContents(group);
	generatedProjectFile.setAttributes(Core::GeneratedFile::OpenProjectAttribute);


	Core::GeneratedFiles files;
	files.append(generatedProjectFile);

	return files;
}
bool DProjectWizard::postGenerateFiles(const QWizard *w,
																																							const Core::GeneratedFiles &l,
																																							QString *errorMessage) const
{
	Q_UNUSED(w)
	return ProjectExplorer::CustomProjectWizard::postGenerateOpen(l, errorMessage);
}

//----------------------------------------------------------------------------
//
// DProjectGroupWizardDialog
//
//----------------------------------------------------------------------------
DProjectGroupWizardDialog::DProjectGroupWizardDialog(
		const Core::BaseFileWizardFactory *factory,
		QWidget *parent) :
Core::BaseFileWizard(factory, QVariantMap(), parent)
{
	setWindowTitle(tr("Create a D language projects group"));

	// first page
	m_firstPage = new Utils::FileWizardPage;
	m_firstPage->setTitle(tr("Project group name and location"));
	m_firstPage->setFileNameLabel(tr("Project group name:"));
	m_firstPage->setPathLabel(tr("Location:"));
	const int firstPageId = addPage(m_firstPage);
	wizardProgress()->item(firstPageId)->setTitle(tr("Location"));
}
QString DProjectGroupWizardDialog::path() const
{
	return m_firstPage->path();
}
void DProjectGroupWizardDialog::setPath(const QString &path)
{
	m_firstPage->setPath(path);
}
QString DProjectGroupWizardDialog::projectName() const
{
	return m_firstPage->fileName();
}

//----------------------------------------------------------------------------
//
// DProjectGroupWizard
//
//----------------------------------------------------------------------------
DProjectGroupWizard::DProjectGroupWizard()
{
	setSupportedProjectTypes({ Constants::DPROJECTGROUP_ID });
	setId("A.DProjectGroup");
	setDisplayName(tr("D projects group"));
	setDescription(tr("Create a D language projects group."));
	setCategory(QLatin1String(ProjectExplorer::Constants::QT_PROJECT_WIZARD_CATEGORY));
	setDisplayCategory(QLatin1String(ProjectExplorer::Constants::QT_PROJECT_WIZARD_CATEGORY_DISPLAY));
	setIcon(QIcon(QLatin1String(":/dprojectmanager/dfolder_64.png")));
	setFlags(Core::IWizardFactory::PlatformIndependent);
}
Core::BaseFileWizard* DProjectGroupWizard::create(QWidget *parent,
																														const Core::WizardDialogParameters &params) const
{
	DProjectGroupWizardDialog *wizard = new DProjectGroupWizardDialog(this, parent);
	wizard->setPath(params.defaultPath());

	foreach (QWizardPage *p, wizard->extensionPages())
		wizard->addPage(p);

	return wizard;
}
Core::GeneratedFiles DProjectGroupWizard::generateFiles(const QWizard *w,
																																																									QString *errorMessage) const
{
	Q_UNUSED(errorMessage)

	const DProjectGroupWizardDialog *wizard = qobject_cast<const DProjectGroupWizardDialog *>(w);
	const QString projectName = wizard->projectName();
	const QDir dir(wizard->path());

	const QString projectFileName = QFileInfo(dir, projectName + QLatin1String(".qcdg")).absoluteFilePath();

	Core::GeneratedFile generatedProjectFile(projectFileName);
	auto group = QLatin1String("[") + QLatin1String(Constants::INI_PROJECTS_ROOT_KEY) +
														QLatin1String("]\n");
	generatedProjectFile.setContents(group);
	generatedProjectFile.setAttributes(Core::GeneratedFile::OpenProjectAttribute);


	Core::GeneratedFiles files;
	files.append(generatedProjectFile);

	return files;
}
bool DProjectGroupWizard::postGenerateFiles(const QWizard *w,
																																							const Core::GeneratedFiles &l,
																																							QString *errorMessage) const
{
	Q_UNUSED(w)
	return ProjectExplorer::CustomProjectWizard::postGenerateOpen(l, errorMessage);
}


} // namespace DProjectManager
