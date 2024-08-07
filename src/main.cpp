#define _MAIN_CPP

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _STRUCT_H
#include "struct.h"
#endif

#ifndef _GLOBAL_H
#include "global.h"
#endif

#ifndef _DIRECTX_FUNC_H
#include "directx_func.h"
#endif

#ifndef _HITBOX_FUNC_H
#include "hitbox_func.h"
#endif

#ifndef _MATH_FUNC_H
#include "math_func.h"
#endif

#ifndef _SHADERFILES_FUNC_H
#include "shaderfiles_func.h"
#endif

#ifndef _WND_FUNC_H
#include "wnd_func.h"
#endif

// Главная функция, точка входа
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// Переменная для хранения кода, возвращаемого вызванным методом интерфейса
	HRESULT hr;

	// изменение рабочей директории
	SetCurrentDirectoryToModulePath();

	// СОЗДАНИЕ ОКНА
	hr = MyCreateWindow(L"DX11GraphicApp", L"GraphicApp", 1280, 720, hInstance, nShowCmd);
	if (FAILED(hr)) {
		return hr;
	}

	// инициализация массива неподвижных хитбоксов
	InitPageSizeAndAllocGranularityVariables();

	// инициализация хитбоксов
	InitHitBoxes();

	//ИНИЦИАЛИЗАЦИЯ DirectX КОМПОНЕНТОВ	
	hr = CreateDirect3DComponents(1280, 720);
	if (FAILED(hr)) {
		return hr;
	}

	// массив вершин (пирамида)
	Vertex* vertexArray = new Vertex[4]{
		Vertex{XMFLOAT4{-2.0f, -2.0f, 2.0f, 1.0f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}}, // a 0
		Vertex{XMFLOAT4{2.0f, -2.0f, 2.0f, 1.0f}, XMFLOAT4{1.0f, 1.0f, 0.0f, 1.0f}}, //b 1
		Vertex{XMFLOAT4{0.0f, -2.0f, -2.0f, 1.0f}, XMFLOAT4{0.0f, 0.0f, 0.0f, 1.0f}}, //c 2
		Vertex{XMFLOAT4{0.0f, 4.0f, 0.0f, 1.0f}, XMFLOAT4{1.0f, 0.0f, 1.0f, 1.0f}} //d 3 вершина пирамиды
	};
	
	// массив вершин стен
	Vertex* wallsVertices = new Vertex[8]{
		Vertex{XMFLOAT4{-50.0f, -2.0f, 50.0f, 1.0f}, XMFLOAT4{CONVERT_COLOR_CODE(0.0f), CONVERT_COLOR_CODE(50.0f), CONVERT_COLOR_CODE(90.0f), 1.0f}},
		Vertex{XMFLOAT4{-50.0f, 6.0f, 50.0f, 1.0f}, XMFLOAT4{CONVERT_COLOR_CODE(0.0f), CONVERT_COLOR_CODE(255.0f), CONVERT_COLOR_CODE(160.0f), 1.0f}},
		Vertex{XMFLOAT4{50.0f, -2.0f, 50.0f, 1.0f}, XMFLOAT4{CONVERT_COLOR_CODE(160.0f), CONVERT_COLOR_CODE(190.0f), CONVERT_COLOR_CODE(80.0f), 1.0f}},
		Vertex{XMFLOAT4{50.0f, 6.0f, 50.0f, 1.0f}, XMFLOAT4{CONVERT_COLOR_CODE(0.0f), CONVERT_COLOR_CODE(255.0f), CONVERT_COLOR_CODE(160.0f), 1.0f}},
		Vertex{XMFLOAT4{-50.0f, -2.0f, -50.0f, 1.0f}, XMFLOAT4{CONVERT_COLOR_CODE(160.0f), CONVERT_COLOR_CODE(190.0f), CONVERT_COLOR_CODE(80.0f), 1.0f}},
		Vertex{XMFLOAT4{-50.0f, 6.0f, -50.0f, 1.0f}, XMFLOAT4{CONVERT_COLOR_CODE(0.0f), CONVERT_COLOR_CODE(255.0f), CONVERT_COLOR_CODE(160.0f), 1.0f}},
		Vertex{XMFLOAT4{50.0f, -2.0f, -50.0f, 1.0f}, XMFLOAT4{CONVERT_COLOR_CODE(0.0f), CONVERT_COLOR_CODE(150.0f), CONVERT_COLOR_CODE(90.0f), 1.0f}},
		Vertex{XMFLOAT4{50.0f, 6.0f, -50.0f, 1.0f}, XMFLOAT4{CONVERT_COLOR_CODE(0.0f), CONVERT_COLOR_CODE(255.0f), CONVERT_COLOR_CODE(160.0f), 1.0f}}
	};
	
	// определение модели шейдеров
	SHADERS_MODEL(g_featureLevel)
	
	// инициализация шейдеров
	InitShaders();
	
	// указание какие примитивы собирать из вершинного буфера
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// создание буфера вершин, создание ILO 
	InitPyramidVertices(vertexArray);
	InitWallsVertices(wallsVertices);

	// создание буфера вершин, компиляция шейдеров, связывание шейдеров и буфера вершин с конвейером
	/*
	hr = InitGeometry(vertexArray, L"TriangleVertexShader.hlsl", L"TrianglePixelShader.hlsl", "main", "main");
	if (FAILED(hr)) {
		return hr;
	}*/

	// освобождение памяти, занятой массивом вершин
	delete[] vertexArray;
	vertexArray = NULL;
	delete[] wallsVertices;
	wallsVertices = NULL;

	// индексы вершин пирамиды
	// обход по часовой стрелеке относительно нормали к поверхности, которую нужно показать
	WORD indices[] = { 
		3, 0, 1, //abс видимая грань (обход по часовой стрелке)
		3, 1, 2, //aсd видимая грань (обход по часовой стрелке)
		3, 2, 0, //dcb невидимая грань (обход против часовой стрелки)
		2, 1, 0 //adb  невидимая грань (обход против часовой стрелки)
	};

	//InvertIndices(indices, 12);
	
	WORD wallsIndices[] = { 
		0, 1, 3,
		0, 3, 2,
		4, 7, 5,
		4, 6, 7,
		4, 5, 1,
		4, 1, 0,
		6, 3, 7,
		6, 2, 3,
		0, 2, 4, // земля
		2, 6, 4
	};
	//InvertIndices(wallsIndices, 6 * 4);

	// Создание константного буфера матриц, константного буфера угла, буфера индексов вершин пирамиды
	hr = InitMatrices(indices);
	if (FAILED(hr)) {
		return hr;
	}
	
	// инициализация буфера индексов вершин стен
	hr = InitWallsIndexBuffer(wallsIndices);
	if (FAILED(hr)) {
		return hr;
	}

	// инициализация матриц
	float vecAngle = -XM_PIDIV4;
	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, 0.0f, 100.0f); // откуда смотрим
	// Лучше использовать XMScalarSinExt, XMScalarCosExt вместо XMScalarSin, XMScalarCos, чтобы получать хорошое округление чисел
	//XMVECTOR zAxis = XMVectorSet(0.0f, XMScalarSinEst(vecAngle), XMScalarCosEst(vecAngle), 1.0f); // куда смотрим. 
	//XMVECTOR yAxis = XMVectorSet(0.0f, XMScalarCosEst(XM_PIDIV4), XMScalarSinEst(XM_PIDIV4), 1.0f); // нормаль к тому, куда смотрим

	XMVECTOR zAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 100.0f); // куда смотрим. 
	XMVECTOR yAxis = XMVectorSet(0.0f, 1.0f, 0.0f, 100.0f); // нормаль к тому, куда смотрим

	//NewCoordinateSystemMatrix(eye, zAxis, yAxis, &matricesWVP.mView);
	matricesWVP.mView = XMMatrixLookToLH(eye, zAxis, yAxis);
	//matricesWVP.mView.r[3] = _mm_mul_ps(eye, XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f)); // приходится заменять последнюю строку в матрице вида, так как XMMatrixLookToLH как-то странно считает значение последней строки
	matricesWVP.mView = XMMatrixTranspose(matricesWVP.mView); 

	// инициализация матрицы проекции
	float fovAngle = XM_PI / 2.5f;
	float ratio = 1280.0F / 720.0F;
	matricesWVP.mProjection = XMMatrixPerspectiveFovLH(fovAngle, ratio, 0.1F, 200.0F);
	matricesWVP.mProjection.r[0] *= XMVECTORF32{W_COORD, 1.0f, 1.0f, 1.0f};
	matricesWVP.mProjection.r[1] *= XMVECTORF32{1.0f, W_COORD, 1.0f, 1.0f};
	matricesWVP.mProjection.r[2] *= XMVECTORF32{1.0f, 1.0f, W_COORD, W_COORD};
	matricesWVP.mProjection.r[3] *= XMVECTORF32{1.0f, 1.0f, W_COORD, 1.0f};
	matricesWVP.mProjection = XMMatrixTranspose(matricesWVP.mProjection);
	/*
	float sin1;
	float cos1;
	XMScalarSinCos(&sin1, &cos1, 0.5f * fovAngle);
	float myCoeff = 1.0f / (1.0f - (sin1 / cos1));
	
	matricesWVP.mProjection = XMMatrixPerspectiveFovLH(fovAngle, ratio, 0.1F, 200.0F);
	matricesWVP.mProjection.r[0] *= XMVECTORF32{0.0f, 1.0f, 1.0f, 1.0f};
	matricesWVP.mProjection.r[1] *= XMVECTORF32{1.0f, 0.0f, 1.0f, 1.0f};
	matricesWVP.mProjection.r[0] += XMVECTORF32{1.0f * myCoeff, 0.0f, 0.0f, 0.0f};
	matricesWVP.mProjection.r[1] += XMVECTORF32{0.0f, ratio * myCoeff, 0.0f, 0.0f};
	
	matricesWVP.mProjection.r[0] *= XMVECTORF32{W_COORD, 1.0f, 1.0f, 1.0f};
	matricesWVP.mProjection.r[1] *= XMVECTORF32{1.0f, W_COORD, 1.0f, 1.0f};
	matricesWVP.mProjection.r[2] *= XMVECTORF32{1.0f, 1.0f, W_COORD, W_COORD};
	matricesWVP.mProjection.r[3] *= XMVECTORF32{1.0f, 1.0f, W_COORD, 1.0f};
	matricesWVP.mProjection = XMMatrixTranspose(matricesWVP.mProjection); */
	//SetProjectionMatrix(&matricesWVP, 0, XM_PI / 100.0f, 0.001f, 7.35f, true);
	//SetProjectionMatrixWithCameraDistance(&matricesWVP, XM_PI / 5.0f, XM_PI / 25.0f, 0.5f, 2.2f, 0.0001f, true);

	// инициализация массива неподвижных хитбоксов
	//InitPageSizeAndAllocGranularityVariables();
	
	// инициализация хитбоксов
	//InitHitBoxes();
	
	// прикрепляем к окну процедуру, которая будет обрабатывать нажатия клавиш и т.д.
	StartUpWndProcPtr = SetWindowLong(g_hWnd, GWLP_WNDPROC, (LONG)WndProc);
	
	MSG msg;// структура, описывающая сообщение
	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT) { // цикл, чтобы не выйти из приложения раньше необходимого времени
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		UpdateScene();
		DrawScene(objectsPositions);

		// проверка на столкновение камеры с статическими и динамическими хитбоксами
		// если камера перешла в другую область статических хитбоксов
		if (ChangesOfStaticHtBoxesArea) {
			DefineCurrentStaticHtBoxesArea();
		}
		#ifdef _STATIC_HITBOX_COLLISION
		StaticHitBoxesCollisionDetection();
		#endif
		#ifdef _DYNAMIC_HITBOX_COLLISION
		DynamicHitBoxesCollisionDetection();
		#endif
	}
	// окночание работы приложения
	ReleaseObjects();
	return 0;
};