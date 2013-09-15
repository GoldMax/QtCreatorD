#include "dlangfilewizard.h"
#include "dlangeditorconstants.h"

#include <utils/filewizarddialog.h>
#include <utils/qtcassert.h>

#include <QFileInfo>
#include <QTextStream>
#include <QWizard>
#include <QPushButton>

namespace {
class DLangFileWizardDialog : public Utils::FileWizardDialog
{
    Q_OBJECT
public:
				DLangFileWizardDialog(QWidget *parent = 0)
        : Utils::FileWizardDialog(parent)
    {
    }
};
} // anonymous namespace

using namespace DLangEditor;

DLangFileWizard::DLangFileWizard(const BaseFileWizardParameters &parameters,
                               ShaderType shaderType, QObject *parent):
    Core::BaseFileWizard(parameters, parent),
    m_shaderType(shaderType)
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

    const QString fileName = Core::BaseFileWizard::buildFileName(path, name, preferredSuffix(m_shaderType));

    Core::GeneratedFile file(fileName);
    file.setContents(fileContents(fileName, m_shaderType));
    file.setAttributes(Core::GeneratedFile::OpenEditorAttribute);
    return Core::GeneratedFiles() << file;
}

QString DLangFileWizard::fileContents(const QString &, ShaderType shaderType) const
{
    QString contents;
    QTextStream str(&contents);

    switch (shaderType) {
				case DLangFileWizard::VertexShaderES:
        str << QLatin1String("attribute highp vec4 qt_Vertex;\n")
            << QLatin1String("attribute highp vec4 qt_MultiTexCoord0;\n")
            << QLatin1String("uniform highp mat4 qt_ModelViewProjectionMatrix;\n")
            << QLatin1String("varying highp vec4 qt_TexCoord0;\n")
            << QLatin1String("\n")
            << QLatin1String("void main(void)\n")
            << QLatin1String("{\n")
            << QLatin1String("    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;\n")
            << QLatin1String("    qt_TexCoord0 = qt_MultiTexCoord0;\n")
            << QLatin1String("}\n");
        break;
				case DLangFileWizard::FragmentShaderES:
        str << QLatin1String("uniform sampler2D qt_Texture0;\n")
            << QLatin1String("varying highp vec4 qt_TexCoord0;\n")
            << QLatin1String("\n")
            << QLatin1String("void main(void)\n")
            << QLatin1String("{\n")
            << QLatin1String("    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);\n")
            << QLatin1String("}\n");
        break;
				case DLangFileWizard::VertexShaderDesktop:
        str << QLatin1String("attribute vec4 qt_Vertex;\n")
            << QLatin1String("attribute vec4 qt_MultiTexCoord0;\n")
            << QLatin1String("uniform mat4 qt_ModelViewProjectionMatrix;\n")
            << QLatin1String("varying vec4 qt_TexCoord0;\n")
            << QLatin1String("\n")
            << QLatin1String("void main(void)\n")
            << QLatin1String("{\n")
            << QLatin1String("    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;\n")
            << QLatin1String("    qt_TexCoord0 = qt_MultiTexCoord0;\n")
            << QLatin1String("}\n");
        break;
				case DLangFileWizard::FragmentShaderDesktop:
        str << QLatin1String("uniform sampler2D qt_Texture0;\n")
            << QLatin1String("varying vec4 qt_TexCoord0;\n")
            << QLatin1String("\n")
            << QLatin1String("void main(void)\n")
            << QLatin1String("{\n")
            << QLatin1String("    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);\n")
            << QLatin1String("}\n");
        break;
    default: break;
    }

    return contents;
}

QWizard *DLangFileWizard::createWizardDialog(QWidget *parent,
                                            const Core::WizardDialogParameters &wizardDialogParameters) const
{
				DLangFileWizardDialog *wizardDialog = new DLangFileWizardDialog(parent);
    wizardDialog->setWindowTitle(tr("New %1").arg(displayName()));
    setupWizard(wizardDialog);
    wizardDialog->setPath(wizardDialogParameters.defaultPath());
    foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
        BaseFileWizard::applyExtensionPageShortTitle(wizardDialog, wizardDialog->addPage(p));
    return wizardDialog;
}

QString DLangFileWizard::preferredSuffix(ShaderType shaderType) const
{
    switch (shaderType) {
				case DLangFileWizard::VertexShaderES:
        return QLatin1String("vsh");
				case DLangFileWizard::FragmentShaderES:
        return QLatin1String("fsh");
				case DLangFileWizard::VertexShaderDesktop:
        return QLatin1String("vert");
				case DLangFileWizard::FragmentShaderDesktop:
        return QLatin1String("frag");
    default:
        return QLatin1String("glsl");
    }
}

#include "dlangfilewizard.moc"
