#include "dbuildconfiguration.h"
#include "dprojectmanagerconstants.h"
#include "dproject.h"

#include <projectexplorer/buildinfo.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/project.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <coreplugin/navigationwidget.h>
#include <utils/pathchooser.h>
#include <utils/qtcassert.h>
#include <utils/fileutils.h>

#include <QFormLayout>
#include <QLabel>
#include <QInputDialog>
#include <QSettings>
#include <QPlainTextEdit>
#include <QSpinBox>

using namespace ProjectExplorer;
using namespace Utils;

namespace DProjectManager {

DBuildConfiguration::DBuildConfiguration(Target *parent, Utils::Id id)
	: BuildConfiguration(parent, id) //Utils::Id(Constants::D_BC_ID))
{
	setConfigWidgetDisplayName(tr("Build settings"));
	setBuildDirectoryHistoryCompleter("D.BuildDir.History");

	m_buildSystem = new DBuildSystem(this);

	setInitializer([this](const BuildInfo &) {
					buildSteps()->appendStep(Constants::D_MS_ID);
					//cleanSteps()->appendStep(Constants::D_MS_ID);
					updateCacheAndEmitEnvironmentChanged();
	});

	updateCacheAndEmitEnvironmentChanged();
}

BuildSystem* DBuildConfiguration::buildSystem() const
{
				return m_buildSystem;
}
//void DBuildConfiguration::initialize()
//{
//	BuildConfiguration::initialize();

//	BuildStepList *buildSteps = stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
//	buildSteps->appendStep(Constants::D_MS_ID);

//	// TODO : configure clean step
////	BuildStepList *cleanSteps = stepList(ProjectExplorer::Constants::BUILDSTEPS_CLEAN);
////	cleanSteps->appendStep(Constants::D_MS_ID);

//	updateCacheAndEmitEnvironmentChanged();
//}
NamedWidget* DBuildConfiguration::createConfigWidget()
{
	return new DBuildSettingsWidget(this);
}

bool DBuildConfiguration::isEnabled() const
{
	DProject* prj = static_cast<DProject*>(project());
	QTC_CHECK(prj);
	return prj->files().count() > 0;
}

void DBuildConfiguration::emitConfigurationChanged(bool rebuildProjectTree)
{
	if(rebuildProjectTree)
	{
		// TODO : Это не работает, окно Projects не обновляется
		//auto navWidget = Core::NavigationWidget::instance(Core::Side::Left);
		//navWidget->closeSubWidgets();
		//QWidget* widget = navWidget->activateSubWidget(ProjectExplorer::Constants::PROJECTTREE_ID, Core::Side::Left);
		//widget = nullptr;
	}
	emit configurationChanged();
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


	setBuildGenerator([](const Kit *, const FilePath &projectPath, bool forSetup) {
					BuildInfo info;
					info.typeName = BuildConfiguration::tr("D Build");
					info.buildDirectory = forSetup ? Project::projectDirectory(projectPath) : projectPath;

					if (forSetup)  {
									//: The name of the build configuration created by default for a generic project.
									info.displayName = BuildConfiguration::tr("Debug");
					}

					return QList<BuildInfo>{info};
	});
}

//QList<BuildInfo> DBuildConfigurationFactory::availableBuilds
//				(const Kit *k, const Utils::FilePath &projectPath, bool /*forSetup*/) const
//{
//	BuildInfo info(this);
//	info.typeName = tr("D Build");
//	info.displayName = tr("Debug");
//	info.buildDirectory = Project::projectDirectory(projectPath);
//	info.kitId = k->id();

//	return {info};
//}

//------------------------------------------------------------------------------------------------
//  class DBuildSettingsWidget
//------------------------------------------------------------------------------------------------
DBuildSettingsWidget::DBuildSettingsWidget(DBuildConfiguration *bc)
	: NamedWidget(tr("Project settings")),
			m_buildConfiguration(bc)
{
	QFormLayout *fl = new QFormLayout(this);
	fl->setContentsMargins(0, -1, 0, -1);
	fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

	//setDisplayName(tr("Project settings"));

	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);
		Utils::FilePath projectDir = proj->projectDirectory();
	fl->addRow(tr("Project directory:"), new QLabel(projectDir.toString()));

	// source directory
	m_pathChooser = new Utils::PathChooser(this);
	m_pathChooser->setEnabled(true);
	m_pathChooser->lineEdit()->setReadOnly(true);
	m_pathChooser->setExpectedKind(Utils::PathChooser::ExistingDirectory);
	m_pathChooser->setBaseDirectory(projectDir);
	m_pathChooser->setEnvironment(bc->environment());
	QString srcRoot = proj->sourcesDirectory();
	m_pathChooser->setPath(srcRoot.length() ? srcRoot : QLatin1String("."));
	connect(m_pathChooser, SIGNAL(pathChanged(QString)), this, SLOT(sourceDirectoryChanged()));
	fl->addRow(tr("Source directory:"), m_pathChooser);

	// Includes
	editIncludes = new QLineEdit(this);
	editIncludes->setText(proj->includes());
	//connect(editIncludes, SIGNAL(textEdited(QString)),	this, SLOT(editsTextChanged()));
	connect(editIncludes, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Include paths:"), editIncludes);

	// Libs
	editLibs = new QLineEdit(this);
	editLibs->setText(proj->libraries());
	//connect(editLibs, SIGNAL(textEdited(QString)),	this, SLOT(editsTextChanged()));
	connect(editLibs, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Libraries:"), editLibs);

	// ExtraArgs
	editExtra = new QLineEdit(this);
	editExtra->setText(proj->extraArgs());
	//connect(editExtra, SIGNAL(textEdited(QString)),	this, SLOT(editsTextChanged()));
	connect(editExtra, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Extra args:"), editExtra);

	// CompilePriority
	editPriority = new QSpinBox(this);
	editPriority->setMinimum(0);
	editPriority->setMaximum(1000);
	editPriority->setValue(proj->compilePriority());
	//connect(editPriority, SIGNAL(valueChanged(int)),	this, SLOT(priorityValueChanged(int)));
	connect(editPriority, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Compile priority:"), editPriority);

}
void DBuildSettingsWidget::sourceDirectoryChanged()
{
	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);

	QDir d(proj->projectDirectory().toString());
	QString rel = d.relativeFilePath(m_pathChooser->rawPath());
	m_pathChooser->setPath(rel);
	proj->setSourcesDirectory(rel);

	proj->saveSettings();
	proj->refresh(DProject::Project);
	m_buildConfiguration->emitConfigurationChanged(false);
}
/*void DBuildSettingsWidget::priorityValueChanged(int val)
{
	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);

	proj->setCompilePriority(val);
	emit m_buildConfiguration->configurationChanged();
}
void DBuildSettingsWidget::editsTextChanged()
{
	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);
	proj->setIncludes(editIncludes->text());
	proj->setLibraries(editLibs->text());
	proj->setExtraArgs(editExtra->text());
	emit m_buildConfiguration->configurationChanged();
}*/
void DBuildSettingsWidget::editsEditingFinished()
{
	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);

	proj->setIncludes(editIncludes->text());
	proj->setLibraries(editLibs->text());
	proj->setExtraArgs(editExtra->text());
	proj->setCompilePriority(editPriority->value());

	proj->saveSettings();
	proj->refresh(DProject::Project);
	m_buildConfiguration->emitConfigurationChanged(QObject::sender() == editPriority);
}

//------------------------------------------------------------------------------------------------
//  class DBuildSystem
//------------------------------------------------------------------------------------------------
DBuildSystem::DBuildSystem(DBuildConfiguration *bc)
				: BuildSystem(bc)
{
}
DBuildSystem::~DBuildSystem()
{
}
bool DBuildSystem::supportsAction(Node *context, ProjectAction action, const Node *node) const
{
	Q_UNUSED(context)
	Q_UNUSED(node)
	return action == AddNewFile
			|| action == AddExistingFile
			|| action == RemoveFile
			|| action == Rename;
}

void DBuildSystem::triggerParsing()
{
	DProject* proj = dynamic_cast<DProject*>(this->project());
	Q_ASSERT(proj);
	proj->refresh(DProject::RefreshOptions::Everything);
}
bool DBuildSystem::addFiles(Node* context, const QStringList &filePaths, QStringList *notAdded)
{
	Q_UNUSED(context)
	Q_UNUSED(notAdded)
	DProject* proj = dynamic_cast<DProject*>(this->project());
	Q_ASSERT(proj);
	return proj->addFiles(filePaths);
}
RemovedFilesFromProject DBuildSystem::removeFiles(Node* context, const QStringList &filePaths, QStringList *notRemoved)
{
	Q_UNUSED(context)
	Q_UNUSED(notRemoved)
	DProject* proj = dynamic_cast<DProject*>(this->project());
	Q_ASSERT(proj);
	proj->removeFiles(filePaths);
	return RemovedFilesFromProject::Ok;
}
bool DBuildSystem::renameFile(Node* context, const QString &filePath, const QString &newFilePath)
{
	Q_UNUSED(context)
	DProject* proj = dynamic_cast<DProject*>(this->project());
	Q_ASSERT(proj);
	return proj->renameFile(filePath, newFilePath);
}

} // namespace DProjectManager
