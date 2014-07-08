#include "dprojectwizard.h"

#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
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

DProjectWizardDialog::DProjectWizardDialog(QWidget *parent)
				: Utils::Wizard(parent)
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
				: Core::BaseFileWizard()
{
	setWizardKind(ProjectWizard);
	setId(QLatin1String("A.DProject"));
	setDisplayName(tr("D Project"));
	setDescription(tr("Create a D language project."));
	setCategory(QLatin1String(ProjectExplorer::Constants::QT_PROJECT_WIZARD_CATEGORY));
	setDisplayCategory(QLatin1String(ProjectExplorer::Constants::QT_PROJECT_WIZARD_CATEGORY_DISPLAY));
	setIcon(QIcon(QLatin1String(":/dprojectmanager/dproj.png")));
	setFlags(Core::IWizard::PlatformIndependent);
}

Core::FeatureSet DProjectWizard::requiredFeatures() const
{
	return Core::FeatureSet();
}


QWizard *DProjectWizard::createWizardDialog(QWidget *parent,
																																																		const Core::WizardDialogParameters &wizardDialogParameters) const
{
	DProjectWizardDialog *wizard = new DProjectWizardDialog(parent);
	wizard->setPath(wizardDialogParameters.defaultPath());

	foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
		BaseFileWizard::applyExtensionPageShortTitle(wizard, wizard->addPage(p));

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
	generatedProjectFile.setContents(QLatin1String("[Files]\n"));
	generatedProjectFile.setAttributes(Core::GeneratedFile::OpenProjectAttribute);


	Core::GeneratedFiles files;
	files.append(generatedProjectFile);

	return files;
}

bool DProjectWizard::postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l, QString *errorMessage)
{
	Q_UNUSED(w);
	return ProjectExplorer::CustomProjectWizard::postGenerateOpen(l, errorMessage);
}

} // namespace DProjectManager
