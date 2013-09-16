#include "dlangfilewizard.h"
#include "dlangeditorconstants.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <utils/filewizarddialog.h>
#include <utils/qtcassert.h>

#include <QFileInfo>
#include <QTextStream>
#include <QWizard>
#include <QPushButton>
#include <QDir>

namespace {
class DLangFileWizardDialog : public Utils::FileWizardDialog
{
    Q_OBJECT
public:
    DLangFileWizardDialog(QWidget *parent,const QString projFile)
        : Utils::FileWizardDialog(parent), m_projFile(projFile)
    {
    }
 QString m_projFile;
};
} // anonymous namespace

using namespace DLangEditor;

DLangFileWizard::DLangFileWizard(const BaseFileWizardParameters &parameters,
                               FileType fileType, QObject *parent):
    Core::BaseFileWizard(parameters, parent), m_fileType(fileType)
{
}

Core::FeatureSet DLangFileWizard::requiredFeatures() const
{
    return Core::FeatureSet();
}

Core::IWizard::WizardFlags DLangFileWizard::flags() const
{
    return Core::IWizard::PlatformIndependent;
}

Core::GeneratedFiles DLangFileWizard::generateFiles(const QWizard *w,
                                                    QString * /*errorMessage*/) const
{
 const DLangFileWizardDialog *wizardDialog = qobject_cast<const DLangFileWizardDialog *>(w);
 const QString path = wizardDialog->path();
 const QString name = wizardDialog->fileName();
 const QString fileName = Core::BaseFileWizard::buildFileName(path, name, preferredSuffix(m_fileType));

 Core::GeneratedFile file(fileName);
 file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);

 QString contents = QLatin1String("module ");
 QString module = wizardDialog->m_projFile;
 if(module.length() == 0)
  module = name;
 else
 {
  module = QDir::cleanPath(module);
  module.truncate(module.lastIndexOf(QDir::separator()));
  QString p = wizardDialog->path();
  module = p.replace(module, QString());
  if(module.length() > 0)
  {
   if(module.at(0) == QDir::separator())
    module = module.remove(0,1);
   if(module.endsWith(QDir::separator()))
    module.chop(1);
   module = module.replace(QDir::separator(), QChar::fromLatin1('.'));
   module.append(QChar::fromAscii('.'));
   module.append(name);
  }
 }
 contents.append(module);
 contents.append(QLatin1String(";\n"));
 //switch(m_fileType) {}
 file.setContents(contents);

 return Core::GeneratedFiles() << file;
}

QWizard *DLangFileWizard::createWizardDialog(QWidget *parent,
  const Core::WizardDialogParameters &wizardDialogParameters) const
{
 QLatin1String key(ProjectExplorer::Constants::PREFERED_PROJECT_NODE);
 QString projFile = wizardDialogParameters.extraValues().value(key).toString();

 DLangFileWizardDialog *wizardDialog = new DLangFileWizardDialog(parent, projFile);
 wizardDialog->setWindowTitle(tr("New %1").arg(displayName()));
 setupWizard(wizardDialog);
 wizardDialog->setPath(wizardDialogParameters.defaultPath());
 foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
  BaseFileWizard::applyExtensionPageShortTitle(wizardDialog, wizardDialog->addPage(p));
 return wizardDialog;
}

QString DLangFileWizard::preferredSuffix(FileType fileType) const
{
 Q_UNUSED(fileType);
 return QLatin1String("d");
// switch (fileType)
// {
//  default: return QLatin1String("d");
// }
}

#include "dlangfilewizard.moc"
