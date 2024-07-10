#define _WND_FUNC_H

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _STRUCT_H
#include "struct.h"
#endif

#if !defined(_GLOBAL_EXTERN_H) && !defined(_MAIN_CPP)
#include "global_extern.h"
#endif

// Функция окна
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 
// Создание окна
HRESULT MyCreateWindow(CONST WCHAR* wndClassNameParam, CONST WCHAR* wndNameParam, int widthParam, int heightParam, HINSTANCE hInstanceParam, int nShowCmdParam);
// оконная процедура, которая обрабатывает все сообщения по-дефолту
LRESULT CALLBACK StartUpWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
