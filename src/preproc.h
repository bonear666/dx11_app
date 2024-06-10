#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3d10_1")
#pragma comment(lib, "d3d10")
#pragma comment(lib,"d3dcompiler")

#include <windows.h>
#include <D3D11.h>
#include <D3D10.h>
#include <DXGI.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <iostream>
#include <cmath>
#include <hidusage.h>

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
#define DYNAMIC_HIT_BOX_MOVEVECTOR_LENGTH 0.001f
#define ROTATION_ANGLE 0.00007f
#define W_COORD 0.01f

#define SHADERS_MODEL(FEATURE_LEVEL) if (FEATURE_LEVEL >= D3D_FEATURE_LEVEL_11_0) {\
											shadersModel = { "vs_5_0", "ps_5_0" };\
										}\
										else\
											shadersModel = { "vs_4_0", "ps_4_0" };