#include "dbuildconfiguration.h"
#include "dprojectmanagerconstants.h"
#include "dproject.h"

#include <projectexplorer/buildinfo.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/project.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <utils/pathchooser.h>
#include <utils/qtcassert.h>

#include <QFormLayout>
#include <QLabel>
#include <QInputDialog>
#include <QSettings>
#include <QPlainTextEdit>

using namespace ProjectExplorer;

namespace DProjectManager {

DBuildConfiguration::DBuildConfiguration(Target *parent, Core::Id id)
	: BuildConfiguration(parent, id) //Core::Id(Constants::D_BC_ID))
{
	setConfigWidgetDisplayName(tr("Build settings"));
	setBuildDirectoryHistoryCompleter("D.BuildDir.History");

	updateCacheAndEmitEnvironmentChanged();
}

void DBuildConfiguration::initialize()
{
	BuildConfiguration::initialize();

	BuildStepList *buildSteps = stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
	buildSteps->appendStep(Constants::D_MS_ID);

	// TODO : configure clean step
//	BuildStepList *cleanSteps = stepList(ProjectExplorer::Constants::BUILDSTEPS_CLEAN);
//	cleanSteps->appendStep(Constants::D_MS_ID);

	updateCacheAndEmitEnvironmentChanged();
}
NamedWidget* DBuildConfiguration::createConfigWidget()
{
	return new DBuildSettingsWidget(this);
}

bool DBuildConfiguration::fromMap(const QVariantMap &map)
{
	if(BuildConfiguration::fromMap(map) == false)
		return false;
	DProject* prj = static_cast<DProject*>(target()->project());
	QTC_CHECK(prj);
	Utils::FilePath fn = 	Utils::FilePath::fromString(prj->buildDirectory().path());
	setBuildDirectory(fn);
	return true;
}

bool DBuildConfiguration::isEnabled() const
{
	DProject* prj = static_cast<DProject*>(project());
	QTC_CHECK(prj);
	return prj->files().count() > 0;
}

//------------------------------------------------------------------------------------------------
//  class DBuildConfigurationFactory
//------------------------------------------------------------------------------------------------

DBuildConfigurationFactory::DBuildConfigurationFactory() :
		BuildConfigurationFactory()
{
	registerBuildConfiguration<DBuildConfiguration>(Constants::D_BC_ID);

	setSupportedProjectType(Constants::DPROJECT_ID);
	setSupportedProjectMimeTypeName(Constants::DPROJECT_MIMETYPE);

}

QList<BuildInfo> DBuildConfigurationFactory::availableBuilds
				(const Kit *k, const Utils::FilePath &projectPath, bool forSetup) const
{
	BuildInfo info(this);
	info.typeName = tr("D Build");
	info.displayName = tr("Debug");
	info.buildDirectory = forSetup ? Project::projectDirectory(projectPath) : projectPath;
	info.kitId = k->id();

	return {info};
}

//------------------------------------------------------------------------------------------------
//  class DBuildSettingsWidget
//------------------------------------------------------------------------------------------------
DBuildSettingsWidget::DBuildSettingsWidget(DBuildConfiguration *bc)
	: m_buildConfiguration(bc)
{
	QFormLayout *fl = new QFormLayout(this);
	fl->setContentsMargins(0, -1, 0, -1);
	fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

	setDisplayName(tr("Project settings"));

	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);
		QString projectDir = proj->projectDirectory().toString();
	fl->addRow(tr("Project directory:"), new QLabel(projectDir));

	// build directory
	m_pathChooser = new Utils::PathChooser(this);
	m_pathChooser->setEnabled(true);
	m_pathChooser->lineEdit()->setReadOnly(true);
	m_pathChooser->setExpectedKind(Utils::PathChooser::ExistingDirectory);
	m_pathChooser->setBaseDirectory(projectDir);
	m_pathChooser->setEnvironment(bc->environment());
	QString srcRoot = proj->sourcesDirectory();
	m_pathChooser->setPath(srcRoot.length() ? srcRoot : QLatin1String("."));
	connect(m_pathChooser, SIGNAL(pathChanged(QString)), this, SLOT(buildDirectoryChanged()));
	fl->addRow(tr("Source directory:"), m_pathChooser);

	// Includes
	editIncludes = new QLineEdit(this);
	editIncludes->setText(proj->includes());
	connect(editIncludes, SIGNAL(textEdited(QString)),	this, SLOT(editsTextChanged()));
	connect(editIncludes, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Include paths:"), editIncludes);

	// Libs
	editLibs = new QLineEdit(this);
	editLibs->setText(proj->libraries());
	connect(editLibs, SIGNAL(textEdited(QString)),	this, SLOT(editsTextChanged()));
	connect(editLibs, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Libraries:"), editLibs);

	// ExtraArgs
	editExtra = new QLineEdit(this);
	editExtra->setText(proj->extraArgs());
	connect(editExtra, SIGNAL(textEdited(QString)),	this, SLOT(editsTextChanged()));
	connect(editExtra, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Extra args:"), editExtra);


}
void DBuildSettingsWidget::buildDirectoryChanged()
{
	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);

	QDir d(proj->projectDirectory().toString());
	QString rel = d.relativeFilePath(m_pathChooser->rawPath());
	proj->setSourcesDirectory(rel);

	m_pathChooser->setPath(rel);
	QSettings sets(proj->projectFilePath().toString(),	QSettings::IniFormat);
	sets.setValue(QLatin1String(Constants::INI_SOURCE_ROOT_KEY), rel);
	sets.sync();
	emit m_buildConfiguration->configurationChanged();
	proj->refresh(DProject::Project);
}

void DBuildSettingsWidget::editsTextChanged()
{
	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);
	proj->setIncludes(editIncludes->text());
	proj->setLibraries(editLibs->text());
	proj->setExtraArgs(editExtra->text());
	emit m_buildConfiguration->configurationChanged();
}
void DBuildSettingsWidget::editsEditingFinished()
{
	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);
	QSettings sets(proj->projectFilePath().toString(),	QSettings::IniFormat);
	sets.setValue(QLatin1String(Constants::INI_INCLUDES_KEY), editIncludes->text());
	sets.setValue(QLatin1String(Constants::INI_LIBRARIES_KEY), editLibs->text());
	sets.setValue(QLatin1String(Constants::INI_EXTRA_ARGS_KEY), editExtra->text());
	sets.sync();
	emit m_buildConfiguration->configurationChanged();
	proj->refresh(DProject::Project);
}

} // namespace DProjectManager
