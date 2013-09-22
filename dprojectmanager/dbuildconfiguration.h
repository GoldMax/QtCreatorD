#ifndef DBUILDCONFIGURATION_H
#define DBUILDCONFIGURATION_H

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/namedwidget.h>

namespace Utils { class PathChooser; }

namespace DProjectManager {
namespace Internal {

class DTarget;
class DBuildConfigurationFactory;
class DBuildSettingsWidget;

class DBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
 Q_OBJECT
 friend class DBuildConfigurationFactory;

public:
 explicit DBuildConfiguration(ProjectExplorer::Target *parent);

 ProjectExplorer::NamedWidget *createConfigWidget();

 BuildType buildType() const;

protected:
 DBuildConfiguration(ProjectExplorer::Target *parent, DBuildConfiguration *source);
 DBuildConfiguration(ProjectExplorer::Target *parent, const Core::Id id);

 friend class DBuildSettingsWidget;
};

class DBuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
 Q_OBJECT

public:
 explicit DBuildConfigurationFactory(QObject *parent = 0);
 ~DBuildConfigurationFactory();

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

class DBuildSettingsWidget : public ProjectExplorer::NamedWidget
{
 Q_OBJECT

public:
 DBuildSettingsWidget(DBuildConfiguration *bc);

private slots:
 void buildDirectoryChanged();
 void environmentHasChanged();

private:
 Utils::PathChooser *m_pathChooser;
 DBuildConfiguration *m_buildConfiguration;
};

} // namespace Internal
} // namespace DProjectManager

#endif // DBUILDCONFIGURATION_H
