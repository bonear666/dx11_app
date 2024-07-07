#define _WND_FUNC_H

// Функция окна
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 
// Создание окна
HRESULT MyCreateWindow(CONST WCHAR* wndClassNameParam, CONST WCHAR* wndNameParam, int widthParam, int heightParam, HINSTANCE hInstanceParam, int nShowCmdParam);
// оконная процедура, которая обрабатывает все сообщения по-дефолту
LRESULT CALLBACK StartUpWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
