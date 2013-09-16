#ifndef DLangMAKESTEP_H
#define DLangMAKESTEP_H

#include <projectexplorer/abstractprocessstep.h>

QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE

namespace DLangProjectManager {
namespace Internal {

class DLangMakeStepConfigWidget;
class DLangMakeStepFactory;
namespace Ui { class DLangMakeStep; }

enum TargetType
{
 Executable = 0,
 StaticLibrary = 1,
 SharedLibrary = 2
};

class DLangMakeStep : public ProjectExplorer::AbstractProcessStep
{
 Q_OBJECT

 friend class DLangMakeStepConfigWidget;
 friend class DLangMakeStepFactory;

public:
 DLangMakeStep(ProjectExplorer::BuildStepList *parent);
 ~DLangMakeStep();

 bool init();
 void run(QFutureInterface<bool> &fi);

 ProjectExplorer::BuildStepConfigWidget *createConfigWidget();
 bool immutable() const;
 QString allArguments() const;
 QString makeCommand(const Utils::Environment &environment) const;

 QVariantMap toMap() const;
 void setMakeArguments(const QString val);

 QString outFileName() const;

protected:
 DLangMakeStep(ProjectExplorer::BuildStepList *parent, DLangMakeStep *bs);
 DLangMakeStep(ProjectExplorer::BuildStepList *parent, const Core::Id id);
 bool fromMap(const QVariantMap &map);

private:
 void ctor();

 TargetType m_targetType;
 QString m_makeArguments;
 QString m_makeCommand;
 QString m_targetName;
 QString m_targetDirName;
 QString m_objDirName;
 QList<ProjectExplorer::Task> m_tasks;
};

class DLangMakeStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
 Q_OBJECT

public:
 DLangMakeStepConfigWidget(DLangMakeStep *makeStep);
 ~DLangMakeStepConfigWidget();
 QString displayName() const;
 QString summaryText() const;

private slots:
 void targetTypeComboBoxSelectItem(int index);
 void makeLineEditTextEdited();
 void makeArgumentsLineEditTextEdited();
 void targetNameLineEditTextEdited();
 void targetDirNameLineEditTextEdited();
 void objDirLineEditTextEdited();
 void updateMakeOverrrideLabel();
 void updateDetails();

private:
 Ui::DLangMakeStep *m_ui;
 DLangMakeStep *m_makeStep;
 QString m_summaryText;
};

class DLangMakeStepFactory : public ProjectExplorer::IBuildStepFactory
{
 Q_OBJECT

public:
 explicit DLangMakeStepFactory(QObject *parent = 0);

 bool canCreate(ProjectExplorer::BuildStepList *parent, const Core::Id id) const;
 ProjectExplorer::BuildStep *create(ProjectExplorer::BuildStepList *parent, const Core::Id id);
 bool canClone(ProjectExplorer::BuildStepList *parent,
               ProjectExplorer::BuildStep *source) const;
 ProjectExplorer::BuildStep *clone(ProjectExplorer::BuildStepList *parent,
                                   ProjectExplorer::BuildStep *source);
 bool canRestore(ProjectExplorer::BuildStepList *parent, const QVariantMap &map) const;
 ProjectExplorer::BuildStep *restore(ProjectExplorer::BuildStepList *parent,
                                     const QVariantMap &map);

 QList<Core::Id> availableCreationIds(ProjectExplorer::BuildStepList *bc) const;
 QString displayNameForId(const Core::Id id) const;
};

} // namespace Internal
} // namespace DLangProjectManager

#endif // DLangMAKESTEP_H
