#include "PluginDefinition.h"
#include <shlwapi.h>
#include "menuCmdID.h"
#include "resource.h"

extern FuncItem funcItem[nbFunc];
extern NppData nppData;
extern bool doCloseTag;

HINSTANCE g_inst;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD reasonForCall, LPVOID /*lpReserved*/)
{
	try {
		switch (reasonForCall)
		{
			case DLL_PROCESS_ATTACH:
				pluginInit(hModule);
				g_inst = HINSTANCE(hModule);
				break;

			case DLL_PROCESS_DETACH:
				pluginCleanUp();
				break;

			case DLL_THREAD_ATTACH:
				break;

			case DLL_THREAD_DETACH:
				break;
		}
	}
	catch (...) { return FALSE; }

    return TRUE;
}


extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	nppData = notpadPlusData;
	commandMenuInit();
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return NPP_PLUGIN_NAME;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = nbFunc;
	return funcItem;
}

void addToolbarButton(ui64 comm_id, ui64 ico_id)
{
	toolbarIconsWithDarkMode tbIcons;
	tbIcons.hToolbarIcon = ::LoadIcon(g_inst, MAKEINTRESOURCE(ico_id));
	tbIcons.hToolbarIconDarkMode = ::LoadIcon(g_inst, MAKEINTRESOURCE(ico_id+1));
	tbIcons.hToolbarBmp = ::LoadBitmap(g_inst, MAKEINTRESOURCE(ico_id+2));
	::SendMessage(nppData._nppHandle, NPPM_ADDTOOLBARICON_FORDARKMODE, funcItem[comm_id]._cmdID, (LPARAM)&tbIcons);
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	switch (notifyCode->nmhdr.code) 
	{
	case NPPN_TBMODIFICATION:
		addToolbarButton(0, IDI_COMMENT_ICON);
		break;
	case NPPN_SHUTDOWN:
		commandMenuCleanUp();
		break;
	default:
		return;
	}
}

#pragma warning( suppress : 4100 )
extern "C" __declspec(dllexport) LRESULT messageProc(UINT msg, WPARAM wp, LPARAM lp)
{

	return TRUE;
}

#ifdef UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
    return TRUE;
}
#endif // UNICODE
