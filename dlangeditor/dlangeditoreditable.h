#ifndef DLangEDITOREDITABLE_H
#define DLangEDITOREDITABLE_H

#include <texteditor/basetexteditor.h>

namespace DLangEditor {
namespace Internal {

class DLangTextEditorWidget;

class DLangEditorEditable : public TextEditor::BaseTextEditor
{
    Q_OBJECT

public:
				explicit DLangEditorEditable(DLangTextEditorWidget *);

    bool duplicateSupported() const { return true; }
    Core::IEditor *duplicate(QWidget *parent);
    Core::Id id() const;
    bool open(QString *errorString, const QString &fileName, const QString &realFileName);
};

} // namespace Internal
} // namespace DLangEditor

#endif // DLangEDITOREDITABLE_H
