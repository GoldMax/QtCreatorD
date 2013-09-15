#ifndef DLangEDITORPLUGIN_H
#define DLangEDITORPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <coreplugin/icontext.h>
#include <coreplugin/id.h>

#include <QPointer>

QT_FORWARD_DECLARE_CLASS(QAction)

namespace TextEditor {
class TextEditorActionHandler;
class ITextEditor;
} // namespace TextEditor

namespace Core {
class Command;
class ActionContainer;
class ActionManager;
} // namespace Core

namespace DLangEditor {
namespace Internal {

class DLangEditorFactory;
class DLangTextEditorWidget;

class DLangEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
				Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DLangEditor.json")

public:
				DLangEditorPlugin();
				~DLangEditorPlugin();

    // IPlugin
    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

				static DLangEditorPlugin *instance() { return m_instance; }

				void initializeEditor(DLangTextEditorWidget *editor);

//    struct InitFile
//    {
//								InitFile(DLang::Engine *engine = 0, DLang::TranslationUnitAST *ast = 0)
//            : engine(engine), ast(ast)
//        {}

//        ~InitFile();

//								DLang::Engine *engine;
//								DLang::TranslationUnitAST *ast;
//    };

//    const InitFile *fragmentShaderInit(int variant) const;
//    const InitFile *vertexShaderInit(int variant) const;
//    const InitFile *shaderInit(int variant) const;

private:
//    QByteArray dlangFile(const QString &fileName) const;
//    InitFile *getInitFile(const QString &fileName, InitFile **initFile) const;
//    void parseGlslFile(const QString &fileName, InitFile *initFile) const;

				static DLangEditorPlugin *m_instance;

				DLangEditorFactory *m_editor;
    TextEditor::TextEditorActionHandler *m_actionHandler;

    QPointer<TextEditor::ITextEditor> m_currentTextEditable;

//    mutable InitFile *m_dlang_120_frag;
//    mutable InitFile *m_dlang_120_vert;
//    mutable InitFile *m_dlang_120_common;
//    mutable InitFile *m_dlang_es_100_frag;
//    mutable InitFile *m_dlang_es_100_vert;
//    mutable InitFile *m_dlang_es_100_common;
};

} // namespace Internal
} // namespace DLangEditor

#endif // DLangEDITORPLUGIN_H
