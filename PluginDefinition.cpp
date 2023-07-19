#include "PluginDefinition.h"
#include "menuCmdID.h"

#include <stdlib.h>
#include <time.h>
#include <shlwapi.h>
#include <shlobj.h>




#define FIND_BEFORE	0x0
#define FIND_AFTER	0x1
#define CLOSING		0x0
#define OPENING		0x2

#define MAX_LANG_LINE_SIZE		512
#define MAX_COMMENT_TAG_SIZE	32




struct Selection
{
	ui64 beg;
	ui64 end;
	ui64 idx;
};

struct Lang
{
	const char *n;
	char c[MAX_COMMENT_TAG_SIZE];
	char bcb[MAX_COMMENT_TAG_SIZE];
	char bce[MAX_COMMENT_TAG_SIZE];
};

struct UserLang
{
	char *n;
	char c[MAX_COMMENT_TAG_SIZE];
	char bcb[MAX_COMMENT_TAG_SIZE];
	char bce[MAX_COMMENT_TAG_SIZE];
};


ui64 ulangs_s;
UserLang *user_langs;

ui64 langs_s = 87;
Lang langs[] = {
	{"normal", ">>> ", "[", "]"} ,	// Coz, why not?
	{"php", "", "", ""} ,
	{"c", "", "", ""} ,
	{"cpp", "", "", ""} ,
	{"cs", "", "", ""} ,
	{"objc", "", "", ""} ,
	{"java", "", "", ""} ,
	{"rc", "", "", ""} ,
	{"html", "", "", ""} ,
	{"xml", "", "", ""} ,
	{"makefile", "", "", ""} ,
	{"pascal", "", "", ""} ,
	{"batch", "", "", ""} ,
	{"ini", "", "", ""} ,
	{"nfo", "", "", ""} ,			// MSDOS Style/ASCII Art
	{"L_USER", "", "", ""} ,		// User Defined language file
	{"asp", "", "", ""} ,
	{"sql", "", "", ""} ,
	{"vb", "", "", ""} ,
	{"L_JS", "", "", ""} ,			// Deprecated
	{"css", "", "", ""} ,
	{"perl", "", "", ""} ,
	{"python", "", "", ""} ,
	{"lua", "", "", ""} ,
	{"tex", "", "", ""} ,
	{"fortran", "", "", ""} ,
	{"bash", "", "", ""} ,
	{"actionscript", "", "", ""} ,	// Flash ActionScript file
	{"nsis", "", "", ""} ,
	{"tcl", "", "", ""} ,
	{"lisp", "", "", ""} ,
	{"scheme", "", "", ""} ,
	{"asm", "", "", ""} ,
	{"diff", "", "", ""} ,
	{"props", "", "", ""} ,
	{"postscript", "", "", ""} ,
	{"ruby", "", "", ""} ,
	{"smalltalk", "", "", ""} ,
	{"vhdl", "", "", ""} ,
	{"kix", "", "", ""} ,
	{"autoit", "", "", ""} ,
	{"caml", "", "", ""} ,
	{"ada", "", "", ""} ,
	{"verilog", "", "", ""} ,
	{"matlab", "", "", ""} ,
	{"haskell", "", "", ""} ,
	{"inno", "", "", ""} ,
	{"searchResult", "", "", ""} ,
	{"cmake", "", "", ""} ,
	{"yaml", "", "", ""} ,
	{"cobol", "", "", ""} ,
	{"gui4cli", "", "", ""} ,
	{"d", "", "", ""} ,
	{"powershell", "", "", ""} ,
	{"r", "", "", ""} ,
	{"jsp", "", "", ""} ,
	{"coffeescript", "", "", ""} ,
	{"json", "", "", ""} ,
	{"javascript", "", "", ""} ,
	{"fortran77", "", "", ""} ,
	{"baanc", "", "", ""} ,
	{"srec", "", "", ""} ,
	{"ihex", "", "", ""} ,
	{"tehex", "", "", ""} ,
	{"swift", "", "", ""} ,
	{"asn1", "", "", ""} ,
	{"avs", "", "", ""} ,
	{"blitzbasic", "", "", ""} ,
	{"purebasic", "", "", ""} ,
	{"freebasic", "", "", ""} ,
	{"csound", "", "", ""} ,
	{"erlang", "", "", ""} ,
	{"escript", "", "", ""} ,
	{"forth", "", "", ""} ,
	{"latex", "", "", ""} ,
	{"mmixal", "", "", ""} ,
	{"nim", "", "", ""} ,
	{"nncrontab", "", "", ""} ,
	{"oscript", "", "", ""} ,
	{"rebol", "", "", ""} ,
	{"registry", "", "", ""} ,
	{"rust", "", "", ""} ,
	{"spice", "", "", ""} ,
	{"txt2tags", "", "", ""} ,
	{"visualprolog", "", "", ""} ,
	{"typescript", "", "", ""} ,
	{"L_EXTERNAL", "", "", ""} ,	// End of language enum
};


const TCHAR sectionName[] = TEXT("Insert Extension");
const TCHAR keyName[] = TEXT("doCloseTag");
const TCHAR configFileName[] = TEXT("pluginDemo.ini");

FuncItem funcItem[nbFunc];
NppData nppData; // The data of Notepad++ that you can use in your plugin commands

TCHAR iniFilePath[MAX_PATH];
bool doCloseTag = false;

HWND scintilla;


const char *line_comm;
const char *block_beg;
const char *block_end;
ui64 line_comm_s;
ui64 block_beg_s;
ui64 block_end_s;

Selection *selects;

ShortcutKey toggle_comm_sh_key;




i64 m2scintilla(UINT id, ui64 wp, ui64 lp);
i64 m2scintilla(UINT id, ui64 wp);
i64 m2scintilla(UINT id);
void updateScintillaHWND();

char *strrep(char *s, ui64 pos, ui64 n, char rep);
bool strCmpRng(const char *s0, const char *s1_beg, const char *s1_end);
bool fndValRng(const char *fnd, const char *start_range, const char **beg, const char **end);
bool fndValRngUser(const char *fnd, const char *start_range, const char **beg, const char **end);
void strCpyRng(char *dest, const char *s_beg, const char *s_end);
void htmlEntiti2chars(char *ent_s);
void extractUserLangInfo(const wchar_t *fn);
void extractULIdir(wchar_t *dir);

DWORD __declspec(nothrow) commDbLoadingThread(LPVOID lp);

ui64 fnd1stNonSp(ui64 line);
ui64 fnd1stNonSp(ui64 line, ui64 *h_sp);
bool isComment(ui64 pos);
bool isLineBeg(ui64 pos);
bool isLineEnd(ui64 pos);
bool isPosBegLine(ui64 pos, ui64 line);									   
bool isBlockComm(ui64 pos, ui64 mode);
void insertTabs(ui64 amount, ui64 pos);
bool fndInSpaceSepArr(const char *fnd, const char *arr);
void wcs2str(char *dest, const wchar_t *src);

i64 doToggleComment(ui64 sel_s, ui64 sel_e, ui64 sel_i);
i64 togSingSelLineComm(ui64 sel_s, ui64 sel_e, ui64 sel_i);
i64 togMultiSelLineComm(ui64 sel_s, ui64 sel_e, ui64 sel_i);
i64 togBlockComm(ui64 sel_s, ui64 sel_e, ui64 sel_i);




#pragma warning( suppress : 4100 )
void pluginInit(HANDLE hModule)
{
	CreateThread(NULL, 0, commDbLoadingThread, NULL, 0, NULL);
}

void pluginCleanUp()
{
	for(ui64 i = 0; i < ulangs_s; ++i)
	{
		VirtualFree(user_langs[i].n, 0, MEM_RELEASE);
	}

	if(user_langs != NULL)
	{
		VirtualFree(user_langs, 0, MEM_RELEASE);
	}
}

void commandMenuInit()
{
	toggle_comm_sh_key._isAlt = false;
	toggle_comm_sh_key._isCtrl = true;
	toggle_comm_sh_key._isShift = false;
	toggle_comm_sh_key._key = 0x51; // VK_Q
	
	setCommand(0, TEXT("Toggle comment"), toggleComment, &toggle_comm_sh_key, false);
	setCommand(1, TEXT("Plug-in homepage"), visitHomepage, NULL, false);
}

bool setCommand(size_t index, const TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit)
{
	if(index >= nbFunc)
		return false;

	if(!pFunc)
		return false;

	lstrcpy(funcItem[index]._itemName, cmdName);
	funcItem[index]._pFunc = pFunc;
	funcItem[index]._init2Check = check0nInit;
	funcItem[index]._pShKey = sk;

	return true;
}

void commandMenuCleanUp()
{
	
}




void toggleComment()
{
	updateScintillaHWND();

	ui32 lang_t = L_TEXT;
	SendMessage(nppData._nppHandle, NPPM_GETCURRENTLANGTYPE, 0, (LPARAM)&lang_t);

	if(lang_t == L_USER)
	{


		static wchar_t fname[MAX_PATH];
		SendMessage(nppData._nppHandle, NPPM_GETFILENAME, MAX_PATH, (LPARAM)fname);

		wchar_t *ext = wcsrchr(fname, '.') + 1;
		if(ext == nullptr)
		{
			return;
		}

		static char ext_str[MAX_PATH];
		wcs2str(ext_str, ext);

		bool ulang_not_found = true;
		for(ui64 i = 0; i < ulangs_s; ++i)
		{
			if(fndInSpaceSepArr(ext_str, user_langs[i].n))
			{
				line_comm = user_langs[i].c;
				block_beg = user_langs[i].bcb;
				block_end = user_langs[i].bce;
				ulang_not_found = false;
				break;
			}
		}

		if(ulang_not_found)
		{
			return;
		}
	}
	else
	{
		line_comm = langs[lang_t].c;
		block_beg = langs[lang_t].bcb;
		block_end = langs[lang_t].bce;
	}

	line_comm_s = strlen(line_comm);
	block_beg_s = strlen(block_beg);
	block_end_s = strlen(block_end);

	ui64 sel_n = m2scintilla(SCI_GETSELECTIONS); // There is always at least one selection

	selects = (Selection *)VirtualAlloc(0, sel_n * sizeof(Selection), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	for(ui64 i = 0; i < sel_n; ++i)
	{
		selects[i].beg = m2scintilla(SCI_GETSELECTIONNSTART, i);
		selects[i].end = m2scintilla(SCI_GETSELECTIONNEND, i);
		selects[i].idx = i;
	}

	for(ui64 i = 0; i < sel_n; ++i) // Insertion sort
	{
		for(ui64 j = i+1; j < sel_n; ++j)
		{
			if(selects[j].beg < selects[i].beg)
			{
				Selection tmp = selects[i];
				selects[i] = selects[j];
				selects[j] = tmp;
			}
		}
	}

	m2scintilla(SCI_BEGINUNDOACTION); // UNDO =========================

	i64 ch_ins = 0;
	for(ui64 i = 0; i < sel_n; ++i)
	{
		ui64 ss = selects[i].beg + ch_ins;
		ui64 se = selects[i].end + ch_ins;

		ch_ins += doToggleComment(ss, se, i);
	}

	if(selects[0].end < selects[0].beg)
	{
		ui64 tmp = selects[0].end;
		selects[0].end = selects[0].beg;
		selects[0].beg = tmp;
	}

	m2scintilla(SCI_SETSELECTION, selects[0].end, selects[0].beg); // Good for 1st selection

	for(ui64 i = 1; i < sel_n; ++i)
	{
		if(selects[i].end < selects[i].beg)
		{
			ui64 tmp = selects[i].end;
			selects[i].end = selects[i].beg;
			selects[i].beg = tmp;
		}

		m2scintilla(SCI_ADDSELECTION, selects[i].end, selects[i].beg);
	}

	m2scintilla(SCI_ENDUNDOACTION);   // ==============================

	VirtualFree(selects, 0, MEM_RELEASE);
}

void visitHomepage()
{
	static const char *link = "https://github.com/ScienceDiscoverer/CommentToggler";

	ShellExecuteA( // =============================================================================
		NULL,				//  [I|O]  Handle to parent window that will show error or UI messages
		"open",				//  [I|O]  Verb string -> open|edit|explore|find|open|print|runas|NULL(def)
		link,				//    [I]  File or object that the verb is beeing executed on
		NULL,				//  [I|O]  Cmd arguments to pass to the file, if it is exe file
		NULL,				//  [I|O]  Default working directory of the action NULL -> current dir
		SW_SHOWNORMAL);		//    [I]  Parameter that sets default nCmdShow status of the window
	// ============================================================================================
}




i64 doToggleComment(ui64 sel_s, ui64 sel_e, ui64 sel_i)
{
	if(line_comm_s != 0)
	{
		if(sel_s == sel_e) // No selection, single line mode
		{
			return togSingSelLineComm(sel_s, sel_e, sel_i);
		}

		if(block_beg_s != 0 && block_end_s != 0)
		{
			if(!isLineBeg(sel_s) || !isLineEnd(sel_e)) /* ~~~ Block comment mode ~~~ */
			{
				return togBlockComm(sel_s, sel_e, sel_i);
			}
		}

		return togMultiSelLineComm(sel_s, sel_e, sel_i); // Multiline line comment mode
	}

	if(block_beg_s != 0 && block_end_s != 0) // No line comment tag found, try to use block mode
	{
		return togBlockComm(sel_s, sel_e, sel_i);
	}

	return 0;
}

i64 togSingSelLineComm(ui64 sel_s, ui64 sel_e, ui64 sel_i)
{
	ui64 line = m2scintilla(SCI_LINEFROMPOSITION, sel_s);

	ui64 non_sp = fnd1stNonSp(line);
	ui64 abs_pos = NPOS; // Absolute position
	if(non_sp == NPOS)
	{
		non_sp = sel_s;
	}
	else
	{
		ui64 line_beg = m2scintilla(SCI_POSITIONFROMLINE, line);
		abs_pos = non_sp + line_beg;
	}

	if(isComment(abs_pos))
	{
		m2scintilla(SCI_DELETERANGE, abs_pos, line_comm_s);
		sel_s -= line_comm_s;
		sel_e -= line_comm_s;

		selects[sel_i].beg = sel_s;
		selects[sel_i].end = sel_e;
		return line_comm_s * -1;
	}

	m2scintilla(SCI_INSERTTEXT, abs_pos, (ui64)line_comm);
	sel_s += line_comm_s;
	sel_e += line_comm_s;

	selects[sel_i].beg = sel_s;
	selects[sel_i].end = sel_e;
	return line_comm_s;
}

i64 togMultiSelLineComm(ui64 sel_s, ui64 sel_e, ui64 sel_i)
{
	i64 ch_changed = 0; // Total amount of characters inserted/removed

	ui64 line0 = m2scintilla(SCI_LINEFROMPOSITION, sel_s);
	ui64 line1 = m2scintilla(SCI_LINEFROMPOSITION, sel_e);

	if(fnd1stNonSp(line0) == NPOS)
	{
		++line0;
	}
	if(isPosBegLine(sel_e, line1))
	{
		--line1;
	}

	ui64 min_indent = NPOS;
	for(ui64 i = line0; i <= line1; ++i)
	{
		ui64 indent = fnd1stNonSp(i);
		if(indent < min_indent)
		{
			min_indent = indent;
		}
	}

	ui64 h_sp = 0;
	ui64 line0_beg = m2scintilla(SCI_POSITIONFROMLINE, line0);
	ui64 line0_abs_indent = fnd1stNonSp(line0, &h_sp) + line0_beg;
	ui64 line0_abs_pos = line0_beg + min_indent;
	bool to_uncomment = isComment(line0_abs_indent);
	ui64 tot_l_com = 0, tot_l_uncom = 0; // Total commented/uncommented lines counters

	if(to_uncomment)
	{
		m2scintilla(SCI_DELETERANGE, line0_abs_indent, line_comm_s);
		ch_changed -= line_comm_s;
		++tot_l_uncom;
	}
	else
	{
		m2scintilla(SCI_INSERTTEXT, line0_abs_pos, (ui64)line_comm);
		ch_changed += line_comm_s;
		++tot_l_com;
	}

	if(h_sp > 0 && sel_s > line0_abs_pos)
	{
		sel_s = to_uncomment ? line0_abs_indent : line0_abs_pos;
	}

	ui64 addt_ins = 0; // Additional inserts
	for(ui64 i = line0+1; i <= line1; ++i)
	{
		ui64 line_beg = m2scintilla(SCI_POSITIONFROMLINE, i);
		ui64 abs_pos = min_indent + line_beg; // Absolute position

		ui64 non_sp = fnd1stNonSp(i, &h_sp);
		if(non_sp == NPOS) // Line contains no non-space characters!
		{
			if(min_indent > h_sp)
			{
				ui64 tabs_need = min_indent - h_sp;
				addt_ins += tabs_need;
				insertTabs(tabs_need, line_beg);
				ch_changed += tabs_need;
			}
		}

		ui64 abs_indent = non_sp + line_beg;

		if(to_uncomment)
		{
			if(!isComment(abs_indent))
			{
				continue;
			}

			m2scintilla(SCI_DELETERANGE, abs_indent, line_comm_s);
			ch_changed -= line_comm_s;
			++tot_l_uncom;
		}
		else
		{
			m2scintilla(SCI_INSERTTEXT, abs_pos, (ui64)line_comm);
			ch_changed += line_comm_s;
			++tot_l_com;
		}
	}

	// Account for characters inserted/removed to maintain the same initial selection area
	sel_e = to_uncomment ? sel_e - tot_l_uncom * line_comm_s : sel_e + tot_l_com * line_comm_s;
	sel_e += addt_ins;

	selects[sel_i].beg = sel_s;
	selects[sel_i].end = sel_e;
	return ch_changed;
}

i64 togBlockComm(ui64 sel_s, ui64 sel_e, ui64 sel_i)
{
	i64 ch_changed = 0; // Total amount of characters inserted/removed

	bool to_uncomment_beg = false, to_uncomment_end = false;
	ui64 obc_bpos = sel_s, cbc_bpos = sel_e; // Block comment strings beginning positions

	if(isBlockComm(sel_s, FIND_AFTER | OPENING))
	{
		to_uncomment_beg = true;
	}
	else
	{
		if(isBlockComm(sel_s, FIND_BEFORE | OPENING))
		{
			obc_bpos -= block_beg_s;
			to_uncomment_beg = true;
		}
	}

	if(isBlockComm(sel_e, FIND_BEFORE | CLOSING))
	{
		cbc_bpos -= block_end_s;
		to_uncomment_end = true;
	}
	else
	{
		if(isBlockComm(sel_e, FIND_AFTER | CLOSING))
		{
			to_uncomment_end = true;
		}
	}

	if(to_uncomment_beg && to_uncomment_end)
	{
		m2scintilla(SCI_DELETERANGE, obc_bpos, block_beg_s);
		ch_changed -= block_beg_s;
		cbc_bpos -= block_beg_s;
		m2scintilla(SCI_DELETERANGE, cbc_bpos, block_end_s);
		ch_changed -= block_end_s;
		sel_s -= sel_s - obc_bpos;
		sel_e -= block_beg_s;
		sel_e -= sel_e - cbc_bpos;
	}
	else
	{
		m2scintilla(SCI_INSERTTEXT, obc_bpos, (ui64)block_beg);
		ch_changed += block_beg_s;
		cbc_bpos += block_beg_s;
		m2scintilla(SCI_INSERTTEXT, cbc_bpos, (ui64)block_end);
		ch_changed += block_end_s;
		sel_s += block_beg_s;
		sel_e += block_beg_s;
	}

	selects[sel_i].beg = sel_s;
	selects[sel_i].end = sel_e;
	return ch_changed;
}




i64 m2scintilla(UINT id, ui64 wp, ui64 lp)
{
	return SendMessage(scintilla, id, (WPARAM)wp, (LPARAM)lp);
}

i64 m2scintilla(UINT id, ui64 wp)
{
	return SendMessage(scintilla, id, (WPARAM)wp, NULL);
}

i64 m2scintilla(UINT id)
{
	return SendMessage(scintilla, id, NULL, NULL);
}

void updateScintillaHWND() // Update the current scintilla
{
	int scintilla_used = -1;
	SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&scintilla_used);
	if(scintilla_used == -1) // Scintilla 0 -> Main View in multivew (Left) | 1 -> Second view (Right)
	{
		return;
	}
	scintilla = (scintilla_used == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
}




char *strrep(char *s, ui64 pos, ui64 n, char rep) // String Replace with Char
{
	ui64 sz = strlen(s);
	ui64 ns = sz - n + 1;

	char *ipos = s + pos;
	memcpy(ipos + 1, s + pos + n, sz - pos - n);
	*ipos = rep;
	s[ns] = 0;

	return s;
}

bool strCmpRng(const char *s0, const char *s1_beg, const char *s1_end)
{
	++s1_end;
	while(*s0 != 0 && s1_beg != s1_end)
	{
		if(*s0 != *s1_beg)
		{
			return false;
		}
		++s0;
		++s1_beg;
	}

	return *s0 == 0 && s1_beg == s1_end;
}

bool fndValRng(const char *fnd, const char *start_range, const char **beg, const char **end)
{	
	ui64 s = strlen(fnd);
	const char *val_tag = strstr(start_range, fnd);
	if(val_tag == nullptr)
	{
		return false;
	}

	*beg = val_tag + s;
	*end = strchr(*beg, '"') - 1;

	if(*end == val_tag + s - 1) // Tag is empty
	{
		return false;
	}

	return true;
}

bool fndValRngUser(const char *fnd, const char *start_range, const char **beg, const char **end)
{
	ui64 s = strlen(fnd);
	const char *val_tag = strstr(start_range, fnd);
	if(val_tag == nullptr)
	{
		return false;
	}

	*beg = val_tag + s;
	*end = strpbrk(*beg, " <") - 1;

	if(*end == val_tag + s - 1) // Tag is empty
	{
		return false;
	}

	return true;
}

void strCpyRng(char *dest, const char *s_beg, const char *s_end)
{
	ui64 s = s_end - s_beg + 1;
	strncpy(dest, s_beg, s);
	dest[s] = 0;
}

void htmlEntiti2chars(char *ent_s)
{
	--ent_s;
	while(*(++ent_s) != 0)
	{
		if(*ent_s == '&')
		{
			char *ent_end = strchr(ent_s, ';');

			if(strCmpRng("&amp;", ent_s, ent_end) || strCmpRng("&#38;", ent_s, ent_end))
			{
				strrep(ent_s, 0, ent_end - ent_s + 1, '&');
			}
			if(strCmpRng("&apos;", ent_s, ent_end) || strCmpRng("&#39;", ent_s, ent_end))
			{
				strrep(ent_s, 0, ent_end - ent_s + 1, '\'');
			}
			if(strCmpRng("&quot;", ent_s, ent_end) || strCmpRng("&#34;", ent_s, ent_end))
			{
				strrep(ent_s, 0, ent_end - ent_s + 1, '"');
			}
			if(strCmpRng("&lt;", ent_s, ent_end) || strCmpRng("&#60;", ent_s, ent_end))
			{
				strrep(ent_s, 0, ent_end - ent_s + 1, '<');
			}
			if(strCmpRng("&gt;", ent_s, ent_end) || strCmpRng("&#62;", ent_s, ent_end))
			{
				strrep(ent_s, 0, ent_end - ent_s + 1, '>');
			}
			if(strCmpRng("&nbsp;", ent_s, ent_end) || strCmpRng("&#160;", ent_s, ent_end))
			{
				strrep(ent_s, 0, ent_end - ent_s + 1, ' ');
			}
		}
	}
}

void extractUserLangInfo(const wchar_t *fn)
{
	// Create or open File or Device =================================================================
	HANDLE f = CreateFile(
		fn,							//   [I]  Name of the file or device to create/open
		GENERIC_READ,				//   [I]  Requested access GENERIC_READ|GENERIC_WRITE|0
		FILE_SHARE_READ,			//   [I]  Sharing mode FILE_SHARE_READ|WRITE|DELETE|0
		NULL,						// [I|O]  SECURITY_ATTRIBUTES for file, handle inheritability
		OPEN_EXISTING,				//   [I]  Action to take if file/device exist or not
		FILE_FLAG_SEQUENTIAL_SCAN,	//   [I]  Attributes and flags for file/device
		NULL);						// [I|O]  Handle to template file to copy attributes from
	// ===============================================================================================

	if(f == INVALID_HANDLE_VALUE)
	{

		return;
	}

	char line[MAX_LANG_LINE_SIZE];
	line[MAX_LANG_LINE_SIZE-1] = 0;
	ui64 li = 0;
	bool ignore_cur_line = false;
	DWORD br;
	char ch = 0;

	while(ReadFile(f, &ch, 1, &br, NULL) && br)
	{
		if(ignore_cur_line)
		{
			if(ch != '\n')
			{
				continue;
			}
			ignore_cur_line = false;
			li = 0;
			continue;
		}

		if(ch == '\n')
		{
			line[li] = '\n';
			line[li+1] = 0;
			li = 0;

			char *lang_p = strstr(line, "<UserLang ");
			if(lang_p == nullptr)
			{
				char *comm_p = strstr(line, "name=\"Comments\"");
				if(comm_p == nullptr)
				{
					continue;
				}

				const char *beg = 0, *end = 0;
				if(fndValRngUser("00", comm_p, &beg, &end))
				{
					strCpyRng(user_langs[ulangs_s].c, beg, end);
					htmlEntiti2chars(user_langs[ulangs_s].c);
				}
				if(fndValRngUser("03", comm_p, &beg, &end))
				{
					strCpyRng(user_langs[ulangs_s].bcb, beg, end);
					htmlEntiti2chars(user_langs[ulangs_s].bcb);
				}
				if(fndValRngUser("04", comm_p, &beg, &end))
				{
					strCpyRng(user_langs[ulangs_s].bce, beg, end);
					htmlEntiti2chars(user_langs[ulangs_s].bce);
				}

				++ulangs_s;
				continue;
			}

			// Skip User Langs without extentions defined
			const char *ext_beg = 0, *ext_end = 0;
			if(!fndValRng("ext=\"", lang_p, &ext_beg, &ext_end))
			{
				CloseHandle(f);
				return;
			}

			ui64 ext_s = ext_end - ext_beg + 1;
			user_langs[ulangs_s].n = (char *)VirtualAlloc(0, ext_s+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			memcpy(user_langs[ulangs_s].n, ext_beg, ext_s);
			user_langs[ulangs_s].n[ext_s] = 0;
			continue;
		}
		if(li == MAX_LANG_LINE_SIZE-1)
		{
			ignore_cur_line = true;
			continue;
		}

		line[li] = ch;
		++li;
	}

	CloseHandle(f);
}

void extractULIdir(wchar_t *dir)
{
	static wchar_t find_file_querry[MAX_PATH];
	wcscpy(find_file_querry, dir);

	wcscat(find_file_querry, L"*.xml");
	ui64 usr_dl_s = wcslen(dir);

	WIN32_FIND_DATA fd;
	HANDLE f = FindFirstFile(find_file_querry, &fd);
	if(f == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if(wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0)
		{
			continue;
		}

		wcscpy(dir + usr_dl_s, fd.cFileName);
		extractUserLangInfo(dir);
	} while(FindNextFile(f, &fd));

	FindClose(f);
}




#pragma warning( suppress : 4100 )
DWORD __declspec(nothrow) commDbLoadingThread(LPVOID lp)
{
	wchar_t *app_data = NULL;

	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &app_data);

	wchar_t npp_appdata_lang[MAX_PATH];
	wcscpy(npp_appdata_lang, app_data);
	wcscat(npp_appdata_lang, L"\\Notepad++\\langs.xml");

	// Create or open File or Device =================================================================
	HANDLE f = CreateFile(
		npp_appdata_lang,			//   [I]  Name of the file or device to create/open
		GENERIC_READ,				//   [I]  Requested access GENERIC_READ|GENERIC_WRITE|0
		FILE_SHARE_READ,			//   [I]  Sharing mode FILE_SHARE_READ|WRITE|DELETE|0
		NULL,						// [I|O]  SECURITY_ATTRIBUTES for file, handle inheritability
		OPEN_EXISTING,				//   [I]  Action to take if file/device exist or not
		FILE_FLAG_SEQUENTIAL_SCAN,	//   [I]  Attributes and flags for file/device
		NULL);						// [I|O]  Handle to template file to copy attributes from
	// ===============================================================================================

	if(f == INVALID_HANDLE_VALUE)
	{
		
		return 1;
	}

	char line[MAX_LANG_LINE_SIZE];
	line[MAX_LANG_LINE_SIZE-1] = 0;
	ui64 li = 0;
	bool ignore_cur_line = false;
	DWORD br;
	char ch = 0;

	while(ReadFile(f, &ch, 1, &br, NULL) && br)
	{
		if(ignore_cur_line)
		{
			if(ch != '\n')
			{
				continue;
			}
			ignore_cur_line = false;
			li = 0;
			continue;
		}

		if(ch == '\n')
		{
			line[li] = '\n';
			line[li+1] = 0;
			li = 0;

			char *lang_p = strstr(line, "<Language ");
			if(lang_p == nullptr)
			{
				continue;
			}

			const char *name_beg = 0, *name_end = 0;
			fndValRng("name=\"", lang_p, &name_beg, &name_end);

			for(ui64 i = 0; i < langs_s; ++i)
			{
				if(strCmpRng(langs[i].n, name_beg, name_end))
				{
					const char *beg = 0, *end = 0;
					if(fndValRng("commentLine=\"", lang_p, &beg, &end))
					{
						strCpyRng(langs[i].c, beg, end);
						htmlEntiti2chars(langs[i].c);
					}
					if(fndValRng("commentStart=\"", lang_p, &beg, &end))
					{
						strCpyRng(langs[i].bcb, beg, end);
						htmlEntiti2chars(langs[i].bcb);
					}
					if(fndValRng("commentEnd=\"", lang_p, &beg, &end))
					{
						strCpyRng(langs[i].bce, beg, end);
						htmlEntiti2chars(langs[i].bce);
					}

					break;
				}
			}

			continue;
		}
		if(li == MAX_LANG_LINE_SIZE-1)
		{
			ignore_cur_line = true;
			continue;
		}

		line[li] = ch;
		++li;
	}

	CloseHandle(f);

	if(strcmp(langs[L_BATCH].c, "REM") == 0) // Fix command line comment
	{
		strcat(langs[L_BATCH].c, " ");
	}

	// Load up all user defined languages available

	ui64 ul_max_s = SendMessage(nppData._nppHandle, NPPM_GETNBUSERLANG, 0, 0);
	user_langs = (UserLang *)VirtualAlloc(0, ul_max_s * sizeof(UserLang), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	wcscpy(npp_appdata_lang, app_data);
	wcscat(npp_appdata_lang, L"\\Notepad++\\userDefineLang.xml");
	
	extractUserLangInfo(npp_appdata_lang);

	if(ulangs_s == ul_max_s)
	{
		return 0;
	}

	wcscpy(npp_appdata_lang, app_data);
	wcscat(npp_appdata_lang, L"\\Notepad++\\userDefineLangs\\");

	extractULIdir(npp_appdata_lang);

	if(ulangs_s == ul_max_s)
	{
		return 0;
	}

	SendMessage(nppData._nppHandle, NPPM_GETNPPDIRECTORY, MAX_PATH, (LPARAM)npp_appdata_lang);
	wcscat(npp_appdata_lang, L"\\userDefineLangs\\");

	extractULIdir(npp_appdata_lang);

	CoTaskMemFree(app_data);

	return 0;
}




ui64 fnd1stNonSp(ui64 line) // Returns first non-horisontal space character in line
{
	ui64 line_l = m2scintilla(SCI_LINELENGTH, line); // Including any line end characters
	char *line_b = (char *)VirtualAlloc(0, line_l, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	m2scintilla(SCI_GETLINE, line, (ui64)line_b); // The buffer is not terminated by a NUL(0) character

	for(ui64 i = 0; i < line_l; ++i)
	{
		if(line_b[i] != '\t' && line_b[i] != ' ')
		{
			if(line_b[i] == '\r' || line_b[i] == '\n')
			{
				break;
			}
			
			VirtualFree(line_b, 0, MEM_RELEASE);
			return i;
		}
	}

	VirtualFree(line_b, 0, MEM_RELEASE);
	return NPOS;
}

ui64 fnd1stNonSp(ui64 line, ui64 *h_sp) // Also returns number of horisonal spaces found
{
	ui64 line_l = m2scintilla(SCI_LINELENGTH, line); // Including any line end characters
	char *line_b = (char *)VirtualAlloc(0, line_l, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	m2scintilla(SCI_GETLINE, line, (ui64)line_b); // The buffer is not terminated by a NUL(0) character

	*h_sp = 0;
	for(ui64 i = 0; i < line_l; ++i)
	{
		if(line_b[i] != '\t' && line_b[i] != ' ')
		{
			if(line_b[i] == '\r' || line_b[i] == '\n')
			{
				break;
			}

			VirtualFree(line_b, 0, MEM_RELEASE);
			return i;
		}
		++(*h_sp);
	}

	VirtualFree(line_b, 0, MEM_RELEASE);
	return NPOS;
}

bool isComment(ui64 pos) // Checks if text at pos is comment start
{
	Sci_CharacterRangeFull cr; // Including cpMin, excluding cpMax
	cr.cpMin = pos;
	cr.cpMax = pos + line_comm_s;

	static char ctag_probe[MAX_COMMENT_TAG_SIZE];
	Sci_TextRangeFull tr;
	tr.chrg = cr;
	tr.lpstrText = ctag_probe;

	m2scintilla(SCI_GETTEXTRANGEFULL, 0, (ui64)&tr); // NULL terminated
	
	return strcmp(line_comm, ctag_probe) == 0;
}

bool isLineBeg(ui64 pos)
{
	ui64 line = m2scintilla(SCI_LINEFROMPOSITION, pos);
	ui64 line_beg = m2scintilla(SCI_POSITIONFROMLINE, line);

	if(pos == line_beg) // Position is in the start of the line
	{
		return true;
	}

	ui64 beg_seg_s = pos - line_beg;

	Sci_CharacterRangeFull cr; // Including cpMin, excluding cpMax
	cr.cpMin = line_beg;
	cr.cpMax = pos;

	char *seg_b = (char *)VirtualAlloc(0, beg_seg_s+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	Sci_TextRangeFull tr;
	tr.chrg = cr;
	tr.lpstrText = seg_b;

	m2scintilla(SCI_GETTEXTRANGEFULL, 0, (ui64)&tr);

	for(ui64 i = beg_seg_s-1; i != NPOS; --i)
	{
		if(seg_b[i] != '\t' && seg_b[i] != ' ')
		{
			VirtualFree(seg_b, 0, MEM_RELEASE);
			return false;
		}
	}

	VirtualFree(seg_b, 0, MEM_RELEASE);
	return true;
}

bool isLineEnd(ui64 pos)
{
	ui64 line = m2scintilla(SCI_LINEFROMPOSITION, pos);
	ui64 line_end = m2scintilla(SCI_GETLINEENDPOSITION, line);
	ui64 line_beg = m2scintilla(SCI_POSITIONFROMLINE, line);													 

	if(line_end == (ui64)m2scintilla(SCI_GETTEXTLENGTH))
	{
		return true;
	}

	if(pos == line_end || pos == line_beg) // Position is in the end of the line
	{
		return true;
	}

	ui64 end_seg_s = line_end - pos;

	Sci_CharacterRangeFull cr; // Including cpMin, excluding cpMax
	cr.cpMin = pos;
	cr.cpMax = line_end;

	char *seg_e = (char *)VirtualAlloc(0, end_seg_s+1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	Sci_TextRangeFull tr;
	tr.chrg = cr;
	tr.lpstrText = seg_e;

	m2scintilla(SCI_GETTEXTRANGEFULL, 0, (ui64)&tr);

	for(ui64 i = 0; i < end_seg_s; ++i)
	{
		if(seg_e[i] != '\t' && seg_e[i] != ' ')
		{
			VirtualFree(seg_e, 0, MEM_RELEASE);
			return false;
		}
	}

	VirtualFree(seg_e, 0, MEM_RELEASE);
	return true;
}

bool isPosBegLine(ui64 pos, ui64 line)
{
	return pos == (ui64)m2scintilla(SCI_POSITIONFROMLINE, line);
}

bool isBlockComm(ui64 pos, ui64 mode)
{
	Sci_CharacterRangeFull cr; // Including cpMin, excluding cpMax


	const char *bcomm;
	ui64 bcomm_s;

	if(mode & OPENING)
	{
		bcomm = block_beg;
		bcomm_s = block_beg_s;
	}
	else
	{
		bcomm = block_end;
		bcomm_s = block_end_s;
	}

	if(mode & FIND_AFTER)
	{
		cr.cpMin = pos;
		cr.cpMax = pos + bcomm_s;
	}
	else
	{
		cr.cpMin = pos - bcomm_s;
		cr.cpMax = pos;
	}

	static char ctag_probe[MAX_COMMENT_TAG_SIZE];
	Sci_TextRangeFull tr;
	tr.chrg = cr;
	tr.lpstrText = ctag_probe;

	m2scintilla(SCI_GETTEXTRANGEFULL, 0, (ui64)&tr); // NULL terminated

	return strcmp(bcomm, ctag_probe) == 0;
}

void insertTabs(ui64 amount, ui64 pos)
{
	for(ui64 i = 0; i < amount; ++i)
	{
		m2scintilla(SCI_INSERTTEXT, pos, (ui64)"\t");
	}
}

bool fndInSpaceSepArr(const char *fnd, const char *arr) // Find in space (0x20) separated array of strings
{
	const char *fnd_b = fnd;
	const char *arr_cmp = arr;
	bool skip = false;
	--arr;
	while(*(++arr) != 0)
	{
		if(*arr == ' ')
		{
			skip = false;
			++arr;
			fnd = fnd_b;
			arr_cmp = arr;
		}
		if(skip)
		{
			continue;
		}

		bool match = true;
		while(*arr_cmp != ' ' && *arr != 0 && *fnd != 0)
		{
			if(*arr_cmp != *fnd)
			{
				skip = true;
				match = false;
				break;
			}
			++arr_cmp, ++fnd;
		}

		if(match)
		{
			if((*arr_cmp == ' ' || *arr_cmp == 0) && *fnd == 0)
			{
				return true;
			}

			arr = arr_cmp - 1;
		}
	}

	return false;
}

void wcs2str(char *dest, const wchar_t *src)
{
	while(*src != 0)
	{
		*dest = (char)(*src);
		++dest, ++src;
	}
	*dest = 0;
}