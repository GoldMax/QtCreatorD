#include "deditorconstants.h"
#include "deditorplugin.h"
#include "dtexteditor.h"
//#include "dautocompleter.h"
//#include "dindenter.h"
#include "dcompletionassist.h"
#include "deditorhighlighter.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>

#include <texteditor/basetextdocument.h>
#include <texteditor/normalindenter.h>
#include <texteditor/generichighlighter/highlighterutils.h>
//#include <texteditor/generichighlighter/highlighter.h>
//#include <cppeditor/cpphighlighter.h>

#include <cpptools/cppqtstyleindenter.h>

#include <QFileInfo>

using namespace Core;
using namespace DEditor::Internal;
using namespace TextEditor;
using namespace TextEditor::Internal;

DTextEditor::DTextEditor(DTextEditorWidget* editor)
 : BaseTextEditor(editor)
{
 setContext(Core::Context(DEditor::Constants::C_DEDITOR_ID,
                          TextEditor::Constants::C_TEXTEDITOR));
}

Core::IEditor* DTextEditor::duplicate(QWidget *parent)
{
 DTextEditorWidget *newWidget = new DTextEditorWidget(parent);
 newWidget->duplicateFrom(editorWidget());
 DEditorPlugin::instance()->initializeEditor(newWidget);
 return newWidget->editor();
}

Core::Id DTextEditor::id() const
{
 //return Core::Constants::K_DEFAULT_TEXT_EDITOR_ID;
 return DEditor::Constants::C_DEDITOR_ID;
}

bool DTextEditor::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
 editorWidget()->setMimeType(Core::MimeDatabase::findByFile(QFileInfo(fileName)).type());
 bool b = TextEditor::BaseTextEditor::open(errorString, fileName, realFileName);
 return b;
}


//-----------------------------
//- DTextEditorWidget
//-----------------------------

DTextEditorWidget::DTextEditorWidget(QWidget *parent)
  : BaseTextEditorWidget(parent) // PlainTextEditorWidget(parent)
{
 setRevisionsVisible(true);
 setMarksVisible(true);
 setLineSeparatorsAllowed(true);
 setParenthesesMatchingEnabled(true);
 setCodeFoldingSupported(true);

 setIndenter(new CppTools::CppQtStyleIndenter);
 // Indenter вызывает исключения в стандартном hightlighter
 //setIndenter(new DIndenter());
 //setIndenter(new NormalIndenter);

 // Понадобится для подсветки пользовательских типов
 //new DEditorHighlighter(baseTextDocument().data());

 setMimeType(QLatin1String(DEditor::Constants::D_MIMETYPE_SRC));
 connect(editorDocument(), SIGNAL(changed()), this, SLOT(configure()));

}

TextEditor::IAssistInterface* DTextEditorWidget::createAssistInterface(
  TextEditor::AssistKind kind, TextEditor::AssistReason reason) const
{
 if (kind == TextEditor::Completion)
  return new DCompletionAssistInterface(document(),
                                            position(),
                                            editor()->document()->filePath(),
                                            reason);
 return BaseTextEditorWidget::createAssistInterface(kind, reason);
}

TextEditor::BaseTextEditor* DTextEditorWidget::createEditor()
{
 DTextEditor* edit = new DTextEditor(this);
 //createToolBar(edit);
 return edit;
}

void DTextEditorWidget::unCommentSelection()
{
 Utils::unCommentSelection(this);
}

void DTextEditorWidget::configure()
{
 MimeType mimeType;
 if (editorDocument())
  mimeType = MimeDatabase::findByFile(editorDocument()->filePath());
 configure(mimeType);
}

void DTextEditorWidget::configure(const QString &mimeType)
{
 configure(MimeDatabase::findByType(mimeType));
}

void DTextEditorWidget::configure(const MimeType &mimeType)
{
 DEditorHighlighter *highlighter = new DEditorHighlighter();
 baseTextDocument()->setSyntaxHighlighter(highlighter);

 if(isParenthesesMatchingEnabled() == false)
  setParenthesesMatchingEnabled(true);

 if (!mimeType.isNull())
 {
  //setMimeTypeForHighlighter(highlighter, mimeType);
  const QString &type = mimeType.type();
  setMimeType(type);
 }

 setFontSettings(TextEditorSettings::instance()->fontSettings());

 emit configured(editor());
}

