/*****************************************************************************
** File			: yDataStorageDll.h
** CopyRight(c) : Yorktal
** Writer		: ZhangLei
** Time			: 2010.11.3
** Description	: dll export and import declaration
** Vesion		: 1.0
** Info			: None
*****************************************************************************/

#ifndef yDataStorageDll_h__
#define yDataStorageDll_h__

#if defined(WIN32) && !defined(VTKSTATIC)

#define Y_DATA_STORAGE_EXPORTS

#if defined(Y_DATA_STORAGE_EXPORTS)
#define Y_DATA_STORAGE_EXPORT __declspec(dllexport)
#else
#define Y_DATA_STORAGE_EXPORT __declspec(dllimport)
#endif

#else
#define Y_DATA_STORAGE_EXPORT
#endif

#endif // yDataStorageDll_h__
