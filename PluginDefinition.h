#pragma once

#include "PluginInterface.h"

#define NPOS 0xFFFFFFFFFFFFFFFF

typedef unsigned long long int ui64;
typedef signed long long int i64;
typedef unsigned int ui32;
typedef signed int i32;
typedef unsigned short int ui16;
typedef signed short int i16;
typedef unsigned char ui8;
typedef signed char i8;

// Notepad++ data
const TCHAR NPP_PLUGIN_NAME[] = TEXT("Comment Toggler");
const int nbFunc = 2;

// Utility functions
void pluginInit(HANDLE hModule);
void pluginCleanUp();
void commandMenuInit();
void commandMenuCleanUp();
bool setCommand(size_t index, const TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false);

// Command functions
void toggleComment();
void visitHomepage();
