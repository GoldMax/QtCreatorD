#pragma once

#include <projectexplorer/abstractprocessstep.h>

namespace DProjectManager {

namespace Ui { class DMakeStepUi; }

class DMakeStep : public ProjectExplorer::AbstractProcessStep
{
	Q_OBJECT

	friend class DMakeStepConfigWidget;

public:
	enum TargetType
	{
		Executable = 0,
		StaticLibrary = 1,
		SharedLibrary = 2
	};
	enum BuildPreset
	{
		Debug = 0,
		Release = 1,
		Unittest = 2,
		None = 3
	};

	explicit DMakeStep(ProjectExplorer::BuildStepList *parent, Utils::Id id);

public:
	QVariantMap toMap() const override;
	bool fromMap(const QVariantMap &map) override;
	ProjectExplorer::BuildStepConfigWidget *createConfigWidget() override;
	void stdError(const QString &line) override;
	void stdOutput(const QString &line) override;

	QString allArguments() const;
	QString outFileName() const;
	QString targetDirName() const { return m_targetDirName; }
	Utils::FilePath makeCommand() const;
	void setMakeArguments(const QString val) { m_makeArguments = val; }
	void setBuildPreset(BuildPreset pres) { m_buildPreset = pres; }

	bool init() override;
	void doRun() override;

private:
	TargetType m_targetType;
	BuildPreset m_buildPreset;
	QString m_targetName;
	QString m_targetDirName;
	QString m_objDirName;
	QString m_makeArguments;
};

class DMakeStepFactory : public ProjectExplorer::BuildStepFactory
{
public:
	DMakeStepFactory();
};

class DMakeStepConfigWidget : public ProjectExplorer::BuildStepConfigWidget
{
	Q_OBJECT

public:
	DMakeStepConfigWidget(DMakeStep *makeStep);
	~DMakeStepConfigWidget();

public slots:
	void updateDetails();

private slots:
	void targetTypeComboBoxSelectItem(int index);
	void buildPresetComboBoxSelectItem(int index);
	void makeArgumentsLineEditTextEdited();
	void targetNameLineEditTextEdited();
	void targetDirNameLineEditTextEdited();
	void objDirLineEditTextEdited();

private:
	Ui::DMakeStepUi *m_ui;
	DMakeStep *m_makeStep;
};

} // namespace DProjectManager
