#define _PREPROC_H

#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3d10_1")
#pragma comment(lib, "d3d10")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

#include <windows.h>
#include <D3D11.h>
#include <D3D10.h>
#include <DXGI.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <iostream>
#include <cmath>
#include <hidusage.h>

//#define _STATIC_HITBOX_COLLISION // включает столкновения с хитбоксами
#define _DYNAMIC_HITBOX_COLLISION // включает столкновения с хитбоксами
#define _DEBUG

// начальные количества структур 
#define STATIC_HIT_BOX_AMOUNT 20
/*
#define HIT_BOX_WIDTH_AND_HEIGHT_AMOUNT 20
#define HIT_BOX_CENTER_AMOUNT 20
#define HIT_BOX_ANGLE_AMOUNT 20
#define STATIC_HIT_BOX_DESC_AMOUNT 20
*/
#define STATIC_HIT_BOX_AREA_AMOUNT 6
#define STATIC_HIT_BOX_AREA_LEAF_AMOUNT 4
#define STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_0 2
#define STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_1 2
#define STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_2 2
#define STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_3 2
#define DYNAMIC_HIT_BOX_AMOUNT 3

// изменяемые параметры
#define ROTATION_ANGLE 0.0005f // угол поворота пирамид
#define W_COORD 0.01f
#define CAMERA_MOVEVECTOR_LENGTH 0.1f
#define DYNAMIC_HIT_BOX_MOVEVECTOR_LENGTH 0.001f

// макросы
#define SHADERS_MODEL(FEATURE_LEVEL) if(FEATURE_LEVEL >= D3D_FEATURE_LEVEL_11_0)\
											shadersModel = { "vs_5_0", "ps_5_0" };\
									 else\
											shadersModel = { "vs_4_0", "ps_4_0" };									
											
#define BUF_FILL_FROM_FILE(FILE_DIR, FILE_HANDLE, BUFFER_PTR, BYTES_NUM, BYTES_NUM_VAR)\
HANDLE FILE_HANDLE = CreateFile(FILE_DIR, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);\
ReadFile(FILE_HANDLE, (LPVOID*)BUFFER_PTR, BYTES_NUM, &BYTES_NUM_VAR, NULL);\
CloseHandle(FILE_HANDLE);

#define WCHAR_NUM_OF_STRING(STRING) \
sizeof(STRING) / sizeof(WCHAR)

#define CONVERT_COLOR_CODE(FLOAT_COLOR) \
FLOAT_COLOR * (1.0f / 255.0f)

// параметры res файлов
#define SHADERS_LIST_CHAR_NUM 55 // with BOM кол-во символов в списке шейдеров
#define COMPILED_SHADERS_LIST_CHAR_NUM 52 // with BOM кол-во символов в списке шейдеров
#define VERTEX_SHADERS_NUM 2 // кол-во шейдеров
#define PIXEL_SHADERS_NUM 1	
#define SHADERS_TOTAL_NUM 3
#define LOG_FILE_FLAGS_NUM 1 // кол-во флагов(символов) в лог файле								
#define LOG_FILE_COMPILED_SHADERS_FLAG_POS 1

// прочие параметры
#define SHADERS_CONST_BUFFERS_NUM 2