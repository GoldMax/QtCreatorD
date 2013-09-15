#ifndef DLangFILEWIZARD_H
#define DLangFILEWIZARD_H

#include <coreplugin/basefilewizard.h>

namespace DLangEditor {

class DLangFileWizard: public Core::BaseFileWizard
{
 Q_OBJECT

public:
 typedef Core::BaseFileWizardParameters BaseFileWizardParameters;

 enum FileType { Empty };

 explicit DLangFileWizard(const BaseFileWizardParameters &parameters,
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

} // namespace DLangEditor

#endif // DLangFILEWIZARD_H
