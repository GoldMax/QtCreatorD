#ifndef DINDENTER_H
#define DINDENTER_H

#include <texteditor/textindenter.h>

namespace TextEditor
{
class ICodeStylePreferences;
}

namespace CppTools {
class CppCodeStyleSettings;
class CppCodeStylePreferences;
}

namespace DEditor {

class DIndenter : public TextEditor::TextIndenter
{
public:
	DIndenter(QTextDocument *doc);
	~DIndenter() override;

	bool isElectricCharacter(const QChar &ch) const override;
	void indentBlock(const QTextBlock &block,
																		const QChar &typedChar,
																		const TextEditor::TabSettings &tabSettings,
																		int cursorPositionInEditor = -1) override;

	void indent(const QTextCursor &cursor,
													const QChar &typedChar,
													const TextEditor::TabSettings &tabSettings,
													int cursorPositionInEditor = -1) override;
	void setCodeStylePreferences(TextEditor::ICodeStylePreferences *preferences) override;

private:
	CppTools::CppCodeStyleSettings codeStyleSettings() const;
	CppTools::CppCodeStylePreferences *m_cppCodeStylePreferences = nullptr;
};

} // namespace DEditor

#endif // DINDENTER_H
