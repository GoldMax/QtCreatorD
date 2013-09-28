#ifndef DFILEWIZARD_H
#define DFILEWIZARD_H

#include <coreplugin/basefilewizard.h>

namespace DEditor {
namespace Internal {

class DFileWizard: public Core::BaseFileWizard
{
 Q_OBJECT

public:
 //typedef Core::BaseFileWizardParameters BaseFileWizardParameters;

 enum FileType { Source, Header };

 explicit DFileWizard(//const BaseFileWizardParameters &parameters,
                          FileType fileType, QObject *parent = 0);

 virtual Core::FeatureSet requiredFeatures() const;
 virtual WizardFlags flags() const;

protected:
 virtual QWizard *createWizardDialog(QWidget *parent,
                                     const Core::WizardDialogParameters &wizardDialogParameters) const;

 virtual Core::GeneratedFiles generateFiles(const QWizard *w,
                                            QString *errorMessage) const;

 virtual QString preferredSuffix(FileType fileType) const;

private:
 FileType m_fileType;
};

} // namespace DEditor
} // namespace DEditor

#endif // DFILEWIZARD_H
