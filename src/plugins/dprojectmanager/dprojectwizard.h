#ifndef DPROJECTWIZARD_H
#define DPROJECTWIZARD_H

#include <coreplugin/basefilewizard.h>
#include <coreplugin/basefilewizardfactory.h>
#include <utils/wizard.h>

namespace Utils { class FileWizardPage; }

namespace DProjectManager {

class DProjectWizardDialog : public Core::BaseFileWizard
{
	Q_OBJECT

public:
	explicit DProjectWizardDialog(const Core::BaseFileWizardFactory *factory,
																															QWidget *parent = nullptr);

	QString path() const;
	void setPath(const QString &path);

	QString projectName() const;

	Utils::FileWizardPage *m_firstPage;
};

class DProjectWizard : public Core::BaseFileWizardFactory
{
	Q_OBJECT

public:
	DProjectWizard();

protected:
	Core::BaseFileWizard* create(QWidget *parent,
																													const Core::WizardDialogParameters &params) const override;
	Core::GeneratedFiles generateFiles(const QWizard* w, QString* errorMessage) const override;
	bool postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l,
																								QString *errorMessage) const override;
};

} // namespace DProjectManager

#endif // DPROJECTWIZARD_H
