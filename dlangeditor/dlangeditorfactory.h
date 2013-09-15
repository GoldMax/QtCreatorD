#ifndef DLangEDITORFACTORY_H
#define DLangEDITORFACTORY_H

#include <coreplugin/editormanager/ieditorfactory.h>

namespace DLangEditor {
namespace Internal {

class DLangEditorFactory : public Core::IEditorFactory
{
    Q_OBJECT

public:
				DLangEditorFactory(QObject *parent);

    Core::IEditor *createEditor(QWidget *parent);

private slots:
    void updateEditorInfoBar(Core::IEditor *editor);
};

} // namespace Internal
} // namespace DLangEditor

#endif // DLangEDITORFACTORY_H
