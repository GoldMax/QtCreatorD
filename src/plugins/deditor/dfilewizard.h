#ifndef DFILEWIZARD_H
#define DFILEWIZARD_H

#include <coreplugin/basefilewizard.h>
#include <coreplugin/basefilewizardfactory.h>
#include <utils/wizard.h>

namespace Utils { class FileWizardPage; }

namespace DEditor {

class DFileWizardDialog : public Core::BaseFileWizard
{
	Q_OBJECT

public:
	explicit DFileWizardDialog(const Core::BaseFileWizardFactory *factory,
																												const QVariantMap &extraValues,
																												QWidget *parent = 0);

	QString path() const;
	void setPath(const QString &path);
	QString fileName() const;
	QString projFile;

private:
	Utils::FileWizardPage* m_firstPage;
};

class DFileWizardFactory : public Core::BaseFileWizardFactory
{
	Q_OBJECT

public:
	enum FileType { Source, Header };

public:
	explicit DFileWizardFactory(FileType fileType);

protected:
	Core::BaseFileWizard* create(QWidget *parent,
																													const Core::WizardDialogParameters &params) const;
	Core::GeneratedFiles generateFiles(const QWizard* w, QString* errorMessage) const override;
	bool postGenerateFiles(const QWizard *w, const Core::GeneratedFiles &l,
																								QString *errorMessage) const override;

	QString preferredSuffix() const;

private:
	FileType m_fileType;
};

} // namespace DEditor

#endif // DFILEWIZARD_H
