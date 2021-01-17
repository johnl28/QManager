#pragma once
#include "framework.h"
#include "Resource.h"
//#include "resource.h"


extern NOTIFYICONDATA nidApp;
extern HINSTANCE hInst;
extern HMENU hPopMenu;
extern bool endThread;

#define MAX_LOADSTRING 100


typedef struct button_struct
{
	uint32_t id;
	WCHAR name[MAX_LOADSTRING];
	uint32_t x, y;
	uint32_t width, height;
} SButton;




