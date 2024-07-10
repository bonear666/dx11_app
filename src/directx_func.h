#define _DIRECTX_FUNC_H

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _STRUCT_H
#include "struct.h"
#endif

#if !defined(_GLOBAL_EXTERN_H) && !defined(_MAIN_CPP)
#include "global_extern.h"
#endif

#ifndef _SHADERFILES_FUNC_H
#include "shaderfiles_func.h"
#endif

// Создание DirectX компонентов
HRESULT CreateDirect3DComponents(UINT widthParam, UINT heightParam);
// Создание буфера вершин, компиляция шейдеров, связывание шейдеров с конвейером
HRESULT InitGeometry(Vertex* vertexArray, LPCWSTR vertexShaderName, LPCWSTR pixelShaderName, LPCSTR vsShaderEntryPoint, LPCSTR psShaderEntryPoint);
// Обновление сцены
void UpdateScene();
// Рендеринг сцены
void DrawScene(XMVECTOR* objectsPositionsArray);
// Освобождение COM-интерфейсов
void ReleaseObjects();
// компиляция шейдера
HRESULT CompileShader(LPCWSTR srcName, LPCSTR entryPoint, LPCSTR target, ID3DBlob** buffer);
// Создание константного буфера матриц, константного буфера угла, индекс-буфера вершин пирамиды
HRESULT InitMatrices(WORD* indices);
// инициализация буфера вершин стен
inline void InitWallsVertices(Vertex* wallsVertexArray);
inline void InitWallsVertices(Vertex* wallsVertexArray) {
	HRESULT hr;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 8;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferSubresourceInitData;
	vertexBufferSubresourceInitData.pSysMem = wallsVertexArray;
	vertexBufferSubresourceInitData.SysMemPitch = 0;
	vertexBufferSubresourceInitData.SysMemSlicePitch = 0;

	hr = g_pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferSubresourceInitData, &pWallsVertexBuffer);

	UINT stride[] = { sizeof(Vertex) };
	UINT offset[] = { 0 };
	g_pImmediateContext->IASetVertexBuffers(1, 1, &pWallsVertexBuffer, stride, offset);

	// Создание Input-Layout Object
	// Описание Input-Layout Object
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Количество элементов в Input-Layout Object
	UINT numInputLayoutObject = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	// Создание Input-Layout Object
	hr = g_pd3dDevice->CreateInputLayout(layout, numInputLayoutObject, shadersBufferArray[2]->GetBufferPointer(), shadersBufferArray[2]->GetBufferSize(), &g_pInputLayoutObjectWalls);

};
// инициализация буфера вершин пирамиды
inline void InitPyramidVertices(Vertex* pyramidVertexArray);
inline void InitPyramidVertices(Vertex* pyramidVertexArray) {
	HRESULT hr;

	// описание vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	// размер буфера
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	// уровень доступа CPU и GPU к буферу 
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// к какой стадии конвейера привязывать буфер
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// уровень доступа CPU к буферу
	vertexBufferDesc.CPUAccessFlags = 0;
	// дополнительные параметры буфера
	vertexBufferDesc.MiscFlags = 0;
	// размер каждого элемента буфера, если данный буфер является structured buffer. (не использую)
	vertexBufferDesc.StructureByteStride = 0;

	// информация для инициализации подресурса
	D3D11_SUBRESOURCE_DATA vertexBufferSubresourceInitData;
	// указатель на инициализируемые данные
	vertexBufferSubresourceInitData.pSysMem = pyramidVertexArray;
	// смещение
	vertexBufferSubresourceInitData.SysMemPitch = 0;
	// смещение
	vertexBufferSubresourceInitData.SysMemSlicePitch = 0;

	// создание vertex buffer
	hr = g_pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferSubresourceInitData, &pPyramidVertexBuffer);

	// привязка vertex buffer к IA stage
	// колчиство байт конкретного вершинного буфера, которое будет использоваться дальше
	UINT stride[] = { sizeof(Vertex) };
	// смещение до первого элемента конкретного вершинного буфера, который будет использован
	UINT offset[] = { 0 };
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pPyramidVertexBuffer, stride, offset);

	// Описание Input-Layout Object
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Количество элементов в Input-Layout Object
	UINT numInputLayoutObject = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	// Создание Input-Layout Object
	hr = g_pd3dDevice->CreateInputLayout(layout, numInputLayoutObject, shadersBufferArray[0]->GetBufferPointer(), shadersBufferArray[0]->GetBufferSize(), &g_pInputLayoutObjectPyramid);
};
// инициализация шейдерных объектов
inline void InitShaders();
inline void InitShaders() {
	// буфер хранения строки пути файла
	//WCHAR* fileDirBuffer = new WCHAR[MAX_PATH];
	// длина пути файла с учетом null(все длины строк с учетом null), без учета имени искомого файла
	//DWORD fileDirBufLength = GetCurrentDirectory(MAX_PATH, fileDirBuffer) + 1;

	// буфер хранения строки пути файла
	WCHAR* fileDirBuffer = new WCHAR[MAX_PATH];
	// длина пути файла с учетом null(все длины строк с учетом null), без учета имени искомого файла
	DWORD fileDirBufLength = GetModuleFileName(NULL, fileDirBuffer, MAX_PATH) + 1;
	size_t backslashPos = FindBackslashInPath(fileDirBuffer, fileDirBufLength);
	fileDirBuffer[backslashPos] = NULL;
	fileDirBufLength = backslashPos + 1;
#ifdef _DEBUG
	backslashPos = FindBackslashInPath(fileDirBuffer, fileDirBufLength);
	fileDirBuffer[backslashPos] = NULL;
	fileDirBufLength = backslashPos + 1;
#endif

	switch (CheckCompiledShadersFromLogFile(fileDirBuffer, fileDirBufLength, (WCHAR*)L"\\res\\log.txt", WCHAR_NUM_OF_STRING(L"\\res\\log.txt"))) {
	case true:
	{
		CreateShadersObjFromCompiledShaders((WCHAR*)L"\\res\\compiled_shaders_list.txt", WCHAR_NUM_OF_STRING(L"\\res\\compiled_shaders_list.txt"), fileDirBuffer, fileDirBufLength);
		break;
	}

	case false:
	{
		CompileAndSaveShadersFromList((WCHAR*)L"\\res\\shaders_list.txt", WCHAR_NUM_OF_STRING(L"\\res\\shaders_list.txt"), fileDirBuffer, fileDirBufLength);
		break;
	}
	}

	delete[] fileDirBuffer;
};
// инициализация буфера вершин стен
inline HRESULT InitWallsIndexBuffer(WORD* indices);
inline HRESULT InitWallsIndexBuffer(WORD* indices) {
	HRESULT hr;

	// описание индекс буфера
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(WORD) * 6 * 4;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// значения индекс буфера
	D3D11_SUBRESOURCE_DATA indexBufferInitData;
	indexBufferInitData.pSysMem = indices;
	indexBufferInitData.SysMemPitch = 0;
	indexBufferInitData.SysMemSlicePitch = 0;

	//создание индекс буфера
	hr = g_pd3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferInitData, &pWallsIndexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
};
