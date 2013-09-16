#include "dlangprojectwizard.h"

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

namespace DLangProjectManager {
namespace Internal {

//----------------------------------------------------------------------------
//
// DLangProjectWizardDialog
//
//----------------------------------------------------------------------------

DLangProjectWizardDialog::DLangProjectWizardDialog(QWidget *parent)
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

QString DLangProjectWizardDialog::path() const
{
 return m_firstPage->path();
}

void DLangProjectWizardDialog::setPath(const QString &path)
{
 m_firstPage->setPath(path);
}

QString DLangProjectWizardDialog::projectName() const
{
 return m_firstPage->fileName();
}

//----------------------------------------------------------------------------
//
// DLangProjectWizard
//
//----------------------------------------------------------------------------

DLangProjectWizard::DLangProjectWizard()
    : Core::BaseFileWizard(parameters())
{ }

Core::FeatureSet DLangProjectWizard::requiredFeatures() const
{
 return Core::FeatureSet();
}

Core::BaseFileWizardParameters DLangProjectWizard::parameters()
{
 Core::BaseFileWizardParameters parameters(ProjectWizard);
 // TODO do something about the ugliness of standard icons in sizes different than 16, 32, 64, 128
 {
  QPixmap icon(22, 22);
  icon.fill(Qt::transparent);
  QPainter p(&icon);
  p.drawPixmap(3, 3, 16, 16, qApp->style()->standardIcon(QStyle::SP_DirIcon).pixmap(16));
  parameters.setIcon(icon);
 }
 parameters.setDisplayName(tr("D Project"));
 parameters.setId(QLatin1String("Z.Makefile"));
 parameters.setDescription(tr("Create a D language project."));
 parameters.setCategory(QLatin1String(ProjectExplorer::Constants::PROJECT_WIZARD_CATEGORY));
 parameters.setDisplayCategory(QLatin1String(ProjectExplorer::Constants::PROJECT_WIZARD_CATEGORY_DISPLAY));
 parameters.setFlags(Core::IWizard::PlatformIndependent);
 return parameters;
}

QWizard *DLangProjectWizard::createWizardDialog(QWidget *parent,
                                                  const Core::WizardDialogParameters &wizardDialogParameters) const
{
 DLangProjectWizardDialog *wizard = new DLangProjectWizardDialog(parent);
 setupWizard(wizard);

 wizard->setPath(wizardDialogParameters.defaultPath());

 foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
  BaseFileWizard::applyExtensionPageShortTitle(wizard, wizard->addPage(p));

 return wizard;
}

Core::GeneratedFiles DLangProjectWizard::generateFiles(const QWizard *w,
                                                         QString *errorMessage) const
{
 Q_UNUSED(errorMessage)

 const DLangProjectWizardDialog *wizard = qobject_cast<const DLangProjectWizardDialog *>(w);
 const QString projectPath = wizard->path();
 const QString projectName = wizard->projectName();
 const QString projectDir = projectPath + QDir::separator() + projectName;
 const QDir dir(projectDir);
 dir.mkpath(projectDir);

 const QString creatorFileName = QFileInfo(dir, projectName + QLatin1String(".qdp")).absoluteFilePath();
 const QString filesFileName = QFileInfo(dir, projectName + QLatin1String(".files")).absoluteFilePath();
 const QString mainFileName = QFileInfo(dir, QLatin1String("main.d")).absoluteFilePath();

 Core::GeneratedFile generatedCreatorFile(creatorFileName);
 generatedCreatorFile.setContents(QLatin1String("[General]\n"));
 generatedCreatorFile.setAttributes(Core::GeneratedFile::OpenProjectAttribute);

 Core::GeneratedFile generatedFilesFile(filesFileName);
 generatedFilesFile.setContents(QLatin1String("main.d\n"));

 Core::GeneratedFile generatedmainFile(mainFileName);
 generatedmainFile.setContents(QLatin1String("module main;\n\nimport std.stdio;\n\nvoid main(string[] args)\n{\n writeln(\"Hello, world!\");\n}\n"));

 Core::GeneratedFiles files;
 files.append(generatedFilesFile);
 files.append(generatedCreatorFile);
 files.append(generatedmainFile);

 return files;
}

bool DLangProjectWizard::postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l, QString *errorMessage)
{
 Q_UNUSED(w);
 return ProjectExplorer::CustomProjectWizard::postGenerateOpen(l, errorMessage);
}

} // namespace Internal
} // namespace DLangProjectManager
