#ifndef DEDITOR_GLOBAL_H
#define DEDITOR_GLOBAL_H

#include <QtGlobal>

#if defined(DEDITOR_LIBRARY)
#  define DEDITORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DEDITORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DEDITOR_GLOBAL_H

