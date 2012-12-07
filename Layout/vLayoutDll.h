#ifndef vLayoutDll_h__
#define vLayoutDll_h__

#include <QtCore/qglobal.h>

#if defined(V_LAYOUT_EXPORTS)
#define V_LAYOUT_EXPORT Q_DECL_EXPORT
#else
#define V_LAYOUT_EXPORT Q_DECL_IMPORT
#endif

#endif // vLayoutDll_h__
