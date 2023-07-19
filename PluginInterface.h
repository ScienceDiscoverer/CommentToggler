#pragma once

#include "Scintilla.h"
#include "Notepad_plus_msgs.h"

const int nbChar = 64;

typedef const TCHAR * (__cdecl * PFUNCGETNAME)();

struct NppData
{
	HWND _nppHandle = nullptr;
	HWND _scintillaMainHandle = nullptr;
	HWND _scintillaSecondHandle = nullptr;
};

typedef void (__cdecl * PFUNCSETINFO)(NppData);
typedef void (__cdecl * PFUNCPLUGINCMD)();
typedef void (__cdecl * PBENOTIFIED)(SCNotification *);
typedef LRESULT(__cdecl *PMESSAGEPROC)(UINT Message, WPARAM wParam, LPARAM lParam);

struct ShortcutKey
{
	bool _isCtrl = false;
	bool _isAlt = false;
	bool _isShift = false;
	UCHAR _key = 0;
};

struct FuncItem
{
	TCHAR _itemName[nbChar] = { '\0' };
	PFUNCPLUGINCMD _pFunc = nullptr;
	int _cmdID = 0;
	bool _init2Check = false;
	ShortcutKey *_pShKey = nullptr;
};

typedef FuncItem * (__cdecl * PFUNCGETFUNCSARRAY)(int *);

// Functions called by Notepad++ plugin manager
extern "C" __declspec(dllexport) void setInfo(NppData);
extern "C" __declspec(dllexport) const TCHAR * getName();
extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *);
extern "C" __declspec(dllexport) void beNotified(SCNotification *);
extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam);

// This API return always true now, since Notepad++ isn't compiled in ANSI mode anymore
extern "C" __declspec(dllexport) BOOL isUnicode();