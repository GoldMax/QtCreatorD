#include "deditorconstants.h"
#include "dfilewizard.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/project.h>
#include <utils/filewizardpage.h>
#include <utils/qtcassert.h>
#include <utils/fileutils.h>
#include <projectexplorer/customwizard/customwizard.h>

#include <QFileInfo>
#include <QTextStream>
#include <QWizard>
#include <QPushButton>
#include <QDir>
#include <QSettings>

using namespace DEditor;

//----------------------------------------------------------------------------
//
// DFileWizardDialog
//
//----------------------------------------------------------------------------

DFileWizardDialog::DFileWizardDialog(const Core::BaseFileWizardFactory *factory,
																																					const QVariantMap &extraValues,
																																					QWidget *parent) :
Core::BaseFileWizard(factory, extraValues, parent)
{
	setWindowTitle(tr("Create a D language file"));

	// first page
	m_firstPage = new Utils::FileWizardPage;
	m_firstPage->setTitle(tr("File name and location"));
	m_firstPage->setFileNameLabel(tr("File name:"));
	m_firstPage->setPathLabel(tr("Location:"));
	const int firstPageId = addPage(m_firstPage);
	wizardProgress()->item(firstPageId)->setTitle(tr("Location"));
}

QString DFileWizardDialog::path() const
{
	return m_firstPage->path();
}

void DFileWizardDialog::setPath(const QString &path)
{
	m_firstPage->setPath(path);
}

QString DFileWizardDialog::fileName() const
{
	return m_firstPage->fileName();
}

//----------------------------------------------------------------------------
//
// DFileWizardFactory
//
//----------------------------------------------------------------------------

DFileWizardFactory::DFileWizardFactory(FileType fileType) :
		m_fileType(fileType)
{
	if(fileType == FileType::Header)
	{
		setId("B.Header");
		setDisplayName(tr("D Header File"));
		setDescription(tr("Creates a D header file."));
		setCategory(QLatin1String(Constants::WIZARD_CATEGORY_D));
		setDisplayCategory(QCoreApplication::translate("DEditor", Constants::WIZARD_TR_CATEGORY_D));
		setIcon(QIcon(QLatin1String(":/deditor/images/di.png")));
		setFlags(Core::IWizardFactory::PlatformIndependent);
	}
	else
	{
		setId("A.Source");
		setDisplayName(tr("D Source File"));
		setDescription(tr("Creates a D source file."));
		setCategory(QLatin1String(Constants::WIZARD_CATEGORY_D));
		setDisplayCategory(QCoreApplication::translate("DEditor", Constants::WIZARD_TR_CATEGORY_D));
		setIcon(QIcon(QLatin1String(":/deditor/images/d.png")));
		setFlags(Core::IWizardFactory::PlatformIndependent);
	}
}

Core::BaseFileWizard* DFileWizardFactory::create(QWidget *parent,
																														const Core::WizardDialogParameters &params) const
{
	DFileWizardDialog* wizard = new DFileWizardDialog(this, params.extraValues(), parent);

	QLatin1String key(ProjectExplorer::Constants::PROJECT_POINTER);
	QVariant qnode = params.extraValues().value(key);
	if(qnode.isNull() == false)
	{
		auto project = static_cast<ProjectExplorer::Project *>(qnode.value<void *>());
		if(project)
			wizard->projFile = project->projectFilePath().toString();
	}

	wizard->setWindowTitle(tr("New %1").arg(displayName()));
	wizard->setPath(params.defaultPath());
	foreach (QWizardPage *p, wizard->extensionPages())
		wizard->addPage(p);
	return wizard;
}

Core::GeneratedFiles DFileWizardFactory::generateFiles(const QWizard *w,	QString *) const
{
	const DFileWizardDialog *wizardDialog = qobject_cast<const DFileWizardDialog *>(w);
	const QString path = wizardDialog->path();
	const QString name = wizardDialog->fileName();
	const QString fileName = QDir(path).filePath(name)+ tr(".") + this->preferredSuffix();

	Core::GeneratedFile file(fileName);
	file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);

	QString contents = QLatin1String("module ");
	QString modul = wizardDialog->projFile;
	if(modul.length() == 0)
		modul = name;
	else
	{
		QSettings sets(modul, QSettings::IniFormat);
		QString bds = sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString();
		Utils::FilePath dir = Utils::FilePath::fromString(modul);
		dir = dir.parentDir();
		if(bds.length() > 0 && bds != QLatin1String("."))
			dir = dir.pathAppended(bds);

		QDir buildDir(dir.toString());
		modul = buildDir.relativeFilePath(wizardDialog->path());
		if(modul.length() > 0)
		{
			if(modul.at(0) == QDir::separator())
				modul = modul.remove(0,1);
			if(modul.endsWith(QDir::separator()))
				modul.chop(1);
			modul = modul.replace(QDir::separator(), QChar::fromLatin1('.'));
			if(name != QLatin1String("package"))
				modul.append(QChar::fromLatin1('.'));
		}
		if(name != QLatin1String("package"))
			modul.append(name);
	}
	contents.append(modul);
	contents.append(QLatin1String(";\n"));
	file.setContents(contents);

	return Core::GeneratedFiles() << file;
}

bool DFileWizardFactory::postGenerateFiles(const QWizard *w,
																																							const Core::GeneratedFiles &l,
																																							QString *errorMessage) const
{
	Q_UNUSED(w)
	return ProjectExplorer::CustomProjectWizard::postGenerateOpen(l, errorMessage);
}

QString DFileWizardFactory::preferredSuffix() const
{
	if(m_fileType == Header)
		return QLatin1String("di");
	return QLatin1String("d");
}

