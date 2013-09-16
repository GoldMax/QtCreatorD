#ifndef BUILDRUNCONFIGWIDGET_H
#define BUILDRUNCONFIGWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLineEdit;
class QComboBox;
class QLabel;
class QAbstractButton;
QT_END_NAMESPACE

namespace Utils {
class DetailsWidget;
class PathChooser;
}

namespace DLangProjectManager
{
class BuildRunConfig;

namespace Internal {

class BuildRunConfigWidget : public QWidget
{
    Q_OBJECT

public:
    BuildRunConfigWidget(BuildRunConfig *rc);

private slots:
    void changed();

    void executableEdited();
    void argumentsEdited(const QString &arguments);
    void workingDirectoryEdited();
    void termToggled(bool);
    void environmentWasChanged();

private:
    bool m_ignoreChange;
    BuildRunConfig *m_runConfiguration;
    Utils::PathChooser *m_executableChooser;
    QLineEdit *m_commandLineArgumentsLineEdit;
    Utils::PathChooser *m_workingDirectory;
    QCheckBox *m_useTerminalCheck;
    Utils::DetailsWidget *m_detailsContainer;
};

} // namespace Internal
} // namespace DLangProjectManager

#endif // BUILDRUNCONFIGWIDGET_H
