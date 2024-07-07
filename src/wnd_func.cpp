#define _WND_FUNC_CPP

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _PREPROC_FUNC_H
#include "preproc_func.h"
#endif

// функция-обработчик сообщений, поступающих окну
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	// структура, содержащая необходимую информацию для рисования в клентской части окна
	PAINTSTRUCT ps;
	// указатель на дескриптор(или просто дескриптор) Device Context
	HDC hdc;

	switch (message) {
	// обработка сообщений от мыши или клавиатуры
	case(WM_INPUT): {
		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpRawInput[sizeof(RAWINPUT)];

		UINT resData = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpRawInput, &dwSize, sizeof(RAWINPUTHEADER));
		DWORD resData2 = ((RAWINPUT*)lpRawInput)->header.dwType;
		if (resData2 == RIM_TYPEKEYBOARD) { // если сообщение поступило от клавиатуры
			USHORT pressedKey = ((RAWINPUT*)lpRawInput)->data.keyboard.VKey;

			switch (pressedKey) {
			case(0x57): {// W
				// можно было бы просто прибавить moveAheadVector к последней строчке view matrix, но эта матрица уже транспонирована, поэтому так сделать не получится 
				matricesWVP.mView = XMMatrixTranspose(XMMatrixTranslationFromVector(moveAheadVector)) * matricesWVP.mView;
				//бляяя зачем я решил хранить позицию камеры в xmfloat3, нужно было в xmvector
				// меняем позицию камеры относительно глобальных координат
				sseProxyRegister0 = XMLoadFloat3(&currentCameraPos); 
				sseProxyRegister0 += moveAheadVectorInGlobalCoord;
				XMStoreFloat3(&currentCameraPos, sseProxyRegister0);
				break;
			}
			
			case(0x53): { // S
				matricesWVP.mView = XMMatrixTranspose(XMMatrixTranslationFromVector(moveBackVector)) * matricesWVP.mView;

				sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);
				sseProxyRegister0 -= moveAheadVectorInGlobalCoord;
				XMStoreFloat3(&currentCameraPos, sseProxyRegister0);
				break;
			}

			case(0x44): { // D
				matricesWVP.mView.r[0] = matricesWVP.mView.r[0] + moveRightVector;

				sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);
				sseProxyRegister0 += moveRightVectorInGlobalCoord;
				XMStoreFloat3(&currentCameraPos, sseProxyRegister0);
				break;
			}

			case(0x41): { // A
				matricesWVP.mView.r[0] = matricesWVP.mView.r[0] + moveLeftVector;

				sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);
				sseProxyRegister0 -= moveRightVectorInGlobalCoord;
				XMStoreFloat3(&currentCameraPos, sseProxyRegister0);
				break;
			}

			default:
				break;
			}
		}
		else // если сообщение поступило от мыши
		{
			LONG mouseX = ((RAWINPUT*)lpRawInput)->data.mouse.lLastX;
			LONG mouseY = ((RAWINPUT*)lpRawInput)->data.mouse.lLastY;
			static XMVECTOR newYAxisRotation = g_XMIdentityR1;

			XMMATRIX matrixRotationNewX = XMMatrixRotationX(-XM_PI * 0.0005 * mouseY); //матрица поворота вокруг оси X
			XMMATRIX matrixRotationNewY = XMMatrixRotationNormal(newYAxisRotation, -XM_PI * 0.0005 * mouseX); //матрица поворота вокруг оси Y
			newYAxisRotation = XMVector3Transform(newYAxisRotation, matrixRotationNewX); //статичная ось Y, вокруг которой происходит поворот
			moveAheadVector = XMVector3Transform(moveAheadVector, matrixRotationNewX);
			moveBackVector = (-1.0f) * moveAheadVector;

			matricesWVP.mView = XMMatrixTranspose(matrixRotationNewX * matrixRotationNewY) * matricesWVP.mView;

			//то же самое, только реализовано через построение новой матрицы вида
			/*
			static XMVECTOR newXAxisRotation = g_XMIdentityR0;
			static XMVECTOR cameraZCoordinateInWorldSpace = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
			static XMVECTOR cameraYCoordinateInWorldSpace = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
			static XMVECTOR cameraPositionInWorldSpace = XMVectorSet(0.0f, 0.3f, -3.0f, 1.0f);

			XMMATRIX matrixRotationNewY = XMMatrixRotationY(XM_PI * 0.0005 * mouseX);
			XMMATRIX matrixRotationNewX = XMMatrixRotationNormal(newXAxisRotation, XM_PI * 0.0005 * mouseY);
			newXAxisRotation = XMVector3Transform(newXAxisRotation, matrixRotationNewY);

			XMMATRIX transformMatrix = matrixRotationNewX * matrixRotationNewY;
			cameraZCoordinateInWorldSpace = XMVector3Transform(cameraZCoordinateInWorldSpace, transformMatrix);
			cameraYCoordinateInWorldSpace = XMVector3Transform(cameraYCoordinateInWorldSpace, transformMatrix);

			matricesWVP.mView = XMMatrixLookToLH(cameraPositionInWorldSpace, cameraZCoordinateInWorldSpace, cameraYCoordinateInWorldSpace);
			matricesWVP.mView = XMMatrixTranspose(matricesWVP.mView);
			*/

			//изменение moveAheadVector в глобальной системе координат
			sseProxyRegister0_Matrix = XMMatrixRotationY(XM_PI * 0.0005 * mouseX);
			moveAheadVectorInGlobalCoord = XMVector3Transform(moveAheadVectorInGlobalCoord, sseProxyRegister0_Matrix);
			//изменение moveRightVector в глобальной системе координат
			moveRightVectorInGlobalCoord = XMVector3Transform(moveRightVectorInGlobalCoord, sseProxyRegister0_Matrix);
		}
		break; 
	}

	case(WM_PAINT): {
		// заполнение структуры ps, и очистка Update Region
		hdc = BeginPaint(hWnd, &ps);

		// очистка Update Region, и освобождение Device Context
		EndPaint(hWnd, &ps);
		break;
	} 
	case(WM_DESTROY): {
		//отправка сообщения WM_QUIT
		PostQuitMessage(0);
		break;
	}
	default:
		DefWindowProc(hWnd, message, wParam, lParam);
	}

	UpdateScene();
	DrawScene(objectsPositions);
	// проверка на столкновение камеры с статическими и динамическими хитбоксами
	// если камера перешла в другую область статических хитбоксов
	if (ChangesOfStaticHtBoxesArea) {
		DefineCurrentStaticHtBoxesArea();
	}
	//StaticHitBoxesCollisionDetection(); 
	DynamicHitBoxesCollisionDetection();

	return 0;
};

LRESULT CALLBACK StartUpWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hWnd, message, wParam, lParam);
};

HRESULT MyCreateWindow(CONST WCHAR* wndClassNameParam, CONST WCHAR* wndNameParam, int widthParam, int heightParam, HINSTANCE hInstanceParam, int nShowCmdParam) {
	// Структура, описывающая класс Окна
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
	// Размер структуры wndClass
	wndClass.cbSize = sizeof(WNDCLASSEX);
	// Стиль класса окна
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	// Указатель на функцию, обрабатывающую сообщения, поступающие окну
	wndClass.lpfnWndProc = StartUpWndProc;
	// Количество дополнительных байтов для структуры wndClass
	wndClass.cbClsExtra = NULL;
	// Количество дополнительных байтов для экземпляра окна
	wndClass.cbWndExtra = NULL;
	// Дескриптор приложения
	wndClass.hInstance = hInstanceParam;
	// Дексриптор иконки окна
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// Дескриптор курсора окна
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// Дескриптор кисти
	wndClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	// Указатель на строку-имя меню класса 
	wndClass.lpszMenuName = NULL;
	// Имя класса окна
	wndClass.lpszClassName = wndClassNameParam;
	// Дескриптор иконки окна, которая отображается на панели задач
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Регистрация класса окна
	if (RegisterClassEx(&wndClass) == 0) {
		return E_FAIL;
	}

	// Создание окна
	g_hWnd = CreateWindowEx(NULL, wndClassNameParam, wndNameParam, WS_OVERLAPPEDWINDOW, 0, 0, widthParam, heightParam, NULL, NULL, hInstanceParam, NULL);
	if (g_hWnd == 0) {
		return E_FAIL;
	}

	// регистрация устройств клавиатуры и мыши
	RAWINPUTDEVICE rid[2];
	// клавиатура
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	rid[0].dwFlags = RIDEV_NOLEGACY;
	rid[0].hwndTarget = g_hWnd;
	// мышь
	rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[1].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[1].dwFlags = RIDEV_NOLEGACY;
	rid[1].hwndTarget = g_hWnd;
	RegisterRawInputDevices(rid, 2, sizeof(rid[0]));

	// Вывод окна на дисплей 
	ShowWindow(g_hWnd, nShowCmdParam);

	return S_OK;
};
