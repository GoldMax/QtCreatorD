#ifndef DLangFILEWIZARD_H
#define DLangFILEWIZARD_H

#include <coreplugin/basefilewizard.h>

namespace DLangEditor {

class DLangFileWizard: public Core::BaseFileWizard
{
    Q_OBJECT

public:
    typedef Core::BaseFileWizardParameters BaseFileWizardParameters;

    enum ShaderType
    {
        VertexShaderES,
        FragmentShaderES,
        VertexShaderDesktop,
        FragmentShaderDesktop
    };

				explicit DLangFileWizard(const BaseFileWizardParameters &parameters,
                            ShaderType shaderType, QObject *parent = 0);

    virtual Core::FeatureSet requiredFeatures() const;
    virtual WizardFlags flags() const;

protected:
    QString fileContents(const QString &baseName, ShaderType shaderType) const;

    virtual QWizard *createWizardDialog(QWidget *parent,
                                        const Core::WizardDialogParameters &wizardDialogParameters) const;

    virtual Core::GeneratedFiles generateFiles(const QWizard *w,
                                               QString *errorMessage) const;

    virtual QString preferredSuffix(ShaderType shaderType) const;

private:
    ShaderType m_shaderType;
};

} // namespace DLangEditor

#endif // DLangFILEWIZARD_H
