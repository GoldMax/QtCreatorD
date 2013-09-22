#ifndef DPROJECTWIZARD_H
#define DPROJECTWIZARD_H

#include <coreplugin/basefilewizard.h>
#include <utils/wizard.h>

namespace Utils { class FileWizardPage; }

namespace DProjectManager {
namespace Internal {

class DProjectWizardDialog : public Utils::Wizard
{
    Q_OBJECT

public:
    DProjectWizardDialog(QWidget *parent = 0);

    QString path() const;
    QString projectName() const;

    void setPath(const QString &path);

    Utils::FileWizardPage *m_firstPage;
};

class DProjectWizard : public Core::BaseFileWizard
{
    Q_OBJECT

public:
    DProjectWizard();
    Core::FeatureSet requiredFeatures() const;

    static Core::BaseFileWizardParameters parameters();

protected:
    QWizard *createWizardDialog(QWidget *parent,
                                const Core::WizardDialogParameters &wizardDialogParameters) const;
    Core::GeneratedFiles generateFiles(const QWizard *w, QString *errorMessage) const;
    bool postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l, QString *errorMessage);
};

} // namespace Internal
} // namespace DProjectManager

#endif // DPROJECTWIZARD_H
