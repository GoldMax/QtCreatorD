#ifndef DLANGBUILDCONFIGURATION_H
#define DLANGBUILDCONFIGURATION_H

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/namedwidget.h>

namespace Utils { class PathChooser; }

namespace DLangProjectManager {
namespace Internal {

class DLangTarget;
class DLangBuildConfigurationFactory;
class DLangBuildSettingsWidget;

class DLangBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
 Q_OBJECT
 friend class DLangBuildConfigurationFactory;

public:
 explicit DLangBuildConfiguration(ProjectExplorer::Target *parent);

 ProjectExplorer::NamedWidget *createConfigWidget();

 BuildType buildType() const;

protected:
 DLangBuildConfiguration(ProjectExplorer::Target *parent, DLangBuildConfiguration *source);
 DLangBuildConfiguration(ProjectExplorer::Target *parent, const Core::Id id);

 friend class DLangBuildSettingsWidget;
};

class DLangBuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
 Q_OBJECT

public:
 explicit DLangBuildConfigurationFactory(QObject *parent = 0);
 ~DLangBuildConfigurationFactory();

 QList<Core::Id> availableCreationIds(const ProjectExplorer::Target *parent) const;
 QString displayNameForId(const Core::Id id) const;

 bool canCreate(const ProjectExplorer::Target *parent, const Core::Id id) const;
 ProjectExplorer::BuildConfiguration *create(ProjectExplorer::Target *parent, const Core::Id id, const QString &name = QString());
 bool canClone(const ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source) const;
 ProjectExplorer::BuildConfiguration *clone(ProjectExplorer::Target *parent, ProjectExplorer::BuildConfiguration *source);
 bool canRestore(const ProjectExplorer::Target *parent, const QVariantMap &map) const;
 ProjectExplorer::BuildConfiguration *restore(ProjectExplorer::Target *parent, const QVariantMap &map);

private:
 bool canHandle(const ProjectExplorer::Target *t) const;
};

class DLangBuildSettingsWidget : public ProjectExplorer::NamedWidget
{
 Q_OBJECT

public:
 DLangBuildSettingsWidget(DLangBuildConfiguration *bc);

private slots:
 void buildDirectoryChanged();
 void environmentHasChanged();

private:
 Utils::PathChooser *m_pathChooser;
 DLangBuildConfiguration *m_buildConfiguration;
};

} // namespace Internal
} // namespace DLangProjectManager

#endif // DLANGBUILDCONFIGURATION_H
