#include "reuse.h"

#include <QLatin1String>

#include <glsl/glsllexer.h>

//using namespace DLang;

namespace DLangEditor {
namespace Internal {

int languageVariant(const QString &mimeType)
{
    int variant = 0;
//    bool isVertex = false;
//    bool isFragment = false;
//    bool isDesktop = false;
//    if (mimeType.isEmpty()) {
//        // ### Before file has been opened, so don't know the mime type.
//        isVertex = true;
//        isFragment = true;
//    } else if (mimeType == QLatin1String("text/x-glsl") ||
//               mimeType == QLatin1String("application/x-glsl")) {
//        isVertex = true;
//        isFragment = true;
//        isDesktop = true;
//    } else if (mimeType == QLatin1String("text/x-glsl-vert")) {
//        isVertex = true;
//        isDesktop = true;
//    } else if (mimeType == QLatin1String("text/x-glsl-frag")) {
//        isFragment = true;
//        isDesktop = true;
//    } else if (mimeType == QLatin1String("text/x-glsl-es-vert")) {
//        isVertex = true;
//    } else if (mimeType == QLatin1String("text/x-glsl-es-frag")) {
//        isFragment = true;
//    }
//    if (isDesktop)
//								variant |= Lexer::Variant_DLang_120;
//    else
//								variant |= Lexer::Variant_DLang_ES_100;
//    if (isVertex)
//        variant |= Lexer::Variant_VertexShader;
//    if (isFragment)
//        variant |= Lexer::Variant_FragmentShader;
    return variant;
}

} // Internal
} // DLangEditor

