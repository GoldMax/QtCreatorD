#include "deditorconstants.h"
#include "dfilewizard.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <utils/filewizarddialog.h>
#include <utils/qtcassert.h>
#include <utils/fileutils.h>

#include <QFileInfo>
#include <QTextStream>
#include <QWizard>
#include <QPushButton>
#include <QDir>
#include <QSettings>

namespace {
class DFileWizardDialog : public Utils::FileWizardDialog
{
	Q_OBJECT
public:
	DFileWizardDialog(QWidget *parent,const QString projFile)
		: Utils::FileWizardDialog(parent), m_projFile(projFile)
	{
	}
	QString m_projFile;
};
} // anonymous namespace

using namespace DEditor::Internal;

DFileWizard::DFileWizard(//const BaseFileWizardParameters &parameters,
																																	FileType fileType, QObject *parent):
		Core::BaseFileWizard(/*parameters,*/ parent), m_fileType(fileType)
{
}

Core::FeatureSet DFileWizard::requiredFeatures() const
{
	return Core::FeatureSet();
}

Core::IWizard::WizardFlags DFileWizard::flags() const
{
	return Core::IWizard::PlatformIndependent;
}

Core::GeneratedFiles DFileWizard::generateFiles(const QWizard *w,
																																																				QString * /*errorMessage*/) const
{
	const DFileWizardDialog *wizardDialog = qobject_cast<const DFileWizardDialog *>(w);
	const QString path = wizardDialog->path();
	const QString name = wizardDialog->fileName();
	const QString fileName = Core::BaseFileWizard::buildFileName(path, name, preferredSuffix(m_fileType));

	Core::GeneratedFile file(fileName);
	file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);

	QString contents = QLatin1String("module ");
	QString modul = wizardDialog->m_projFile;
	if(modul.length() == 0)
		modul = name;
	else
	{
		QSettings sets(modul, QSettings::IniFormat);
		QString bds = sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString();
		Utils::FileName dir = Utils::FileName::fromString(modul);
		dir = dir.parentDir();
		if(bds.length() > 0 && bds != QLatin1String("."))
			dir.appendPath(bds);

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
				modul.append(QChar::fromAscii('.'));
		}
		if(name != QLatin1String("package"))
			modul.append(name);
	}
	contents.append(modul);
	contents.append(QLatin1String(";\n"));
	//switch(m_fileType) {}
	file.setContents(contents);

	return Core::GeneratedFiles() << file;
}

QWizard *DFileWizard::createWizardDialog(QWidget *parent,
																																													const Core::WizardDialogParameters &wizardDialogParameters) const
{
	QLatin1String key(ProjectExplorer::Constants::PREFERED_PROJECT_NODE);
	QString projFile = wizardDialogParameters.extraValues().value(key).toString();

	DFileWizardDialog *wizardDialog = new DFileWizardDialog(parent, projFile);
	wizardDialog->setWindowTitle(tr("New %1").arg(displayName()));
	//setupWizard(wizardDialog);
	wizardDialog->setPath(wizardDialogParameters.defaultPath());
	foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
		BaseFileWizard::applyExtensionPageShortTitle(wizardDialog, wizardDialog->addPage(p));
	return wizardDialog;
}

QString DFileWizard::preferredSuffix(FileType fileType) const
{
	if(fileType == Header)
		return QLatin1String("di");
	return QLatin1String("d");
}

#include "dfilewizard.moc"
