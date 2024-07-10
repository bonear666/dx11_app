#define _DIRECTX_FUNC_CPP

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _STRUCT_H
#include "struct.h"
#endif

#ifndef _GLOBAL_EXTERN_H
#include "global_extern.h"
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

HRESULT CreateDirect3DComponents(UINT widthParam, UINT heightParam) {
	HRESULT hr;

	//ИНИЦИАЛИЗАЦИЯ DirectX КОМПОНЕНТОВ	

	// Струкутра, описывающая back buffer SwapChain-a
	DXGI_MODE_DESC backBuffer;
	ZeroMemory(&backBuffer, sizeof(DXGI_MODE_DESC));
	// Ширина
	backBuffer.Width = widthParam;
	// Высота
	backBuffer.Height = heightParam;
	// Частота обновления монитора
	backBuffer.RefreshRate.Numerator = 60;
	backBuffer.RefreshRate.Denominator = 1;
	// Формат пикселей
	backBuffer.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// Порядок создания изображения(Surface)
	backBuffer.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// Флаг, обозначающий то, как будет растягиваться изображение для соответствия резрешению заданного монитора
	backBuffer.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Структура, описывающая цепь обмена (Swap Chain)
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	// Описание backBuffer-a, который входит в состав Swap Chain-a
	sd.BufferDesc = backBuffer;
	// Описание мультисемплинга(не использую)
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	// Цель использования буферов
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// Количество буферов в Swap Chain-e (так как приложение работает в оконном режиме, то для двойной буферизации достаточно ввести кол-во буферов без учета дополнительного Front Buffer-a
	//(Front Buffer-ом является рабочий стол)), но для full-screen mode приложения необходимо ввести 2 буфера для двойной буферизации
	sd.BufferCount = 2;
	// Дескриптор окна приложения
	sd.OutputWindow = g_hWnd;
	// Вывод происходит в windowed mode или full-screen mode
	sd.Windowed = TRUE;
	// Что должен делать драйвер после того, как front buffer стал back buffer-ом
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// Дополнительные параметры Swap Chain-а
	sd.Flags = NULL;

	// Этап Создания Device, Device Contex, Swap Chain, Render Target View, View Port

	// Определим feature level, который поддерживается видеокартой, и определим используемый тип драйвера
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0 };

	// Количество элементов в массиве featureLevels
	UINT numFeatureLevels = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);

	// Хотя я буду использовать только hardware type
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
		D3D_DRIVER_TYPE_SOFTWARE
	};

	// Количество элементов в массиве dreiverTypes
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(D3D_DRIVER_TYPE);

	// Собственно создание Direct3D Device, Device Context, Swap Chain, View Port

	// Результат вызова CreateDeviceAndSwapChain
	HRESULT createDeviceDeviceContextSwapChainResult(S_OK);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex) {
		createDeviceDeviceContextSwapChainResult = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[driverTypeIndex], NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(createDeviceDeviceContextSwapChainResult)) {
			goto createDeviceDeviceContextSwapChainLoopExit;
		}
	}
	// Неуспешный выход из цикла
	return createDeviceDeviceContextSwapChainResult;
	// Успешный выход из цикла
createDeviceDeviceContextSwapChainLoopExit:

	// Получение доступа к back buffer
	// Указатель на back buffer
	ID3D11Texture2D* pBackBuffer(NULL); 
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	// Создание Render Target View
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);

	// Освобождение интерфейса back buffer, back buffer больше не нужен 
	pBackBuffer->Release();
	pBackBuffer = NULL; // хотя занулять не обязательно, в данном случае

	// Создание View Port, области поверхности RTV, которая и будет отображаться на дисплей. Также view port переводит RTV из пиксельных координат к числовым координатам 
	D3D11_VIEWPORT viewPort;
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = widthParam;
	viewPort.Height = heightParam;
	// Уровень удаленности объектов, которые будут отображаться в view port
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	// Связывание view port с графическим конвейером
	g_pImmediateContext->RSSetViewports(1, &viewPort);

	// Описание depth stencil буфера
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = widthParam;
	depthStencilDesc.Height = heightParam;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_D32_FLOAT_S8X24_UINT DXGI_FORMAT_D24_UNORM_S8_UINT DXGI_FORMAT_D16_UNORM
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	g_pd3dDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilTexture);

	// описание того, как будет выполнятся z-test и stencil-test
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace = { D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_ALWAYS };
	dsDesc.BackFace = { D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_NEVER };

	// создание состояния depth-stencil теста
	g_pd3dDevice->CreateDepthStencilState(&dsDesc, &pDSState);

	// связывание настроек depth-stencil теста с OM stage
	g_pImmediateContext->OMSetDepthStencilState(pDSState, 1);

	// описание depth-stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = NULL;
	descDSV.Texture2D.MipSlice = 0;

	// создание depth-stencil view
	g_pd3dDevice->CreateDepthStencilView(depthStencilTexture, &descDSV, &g_pDepthStencilView);

	// Привязка RTV и DSV к Output-Merger Stage
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// описание состояния растеризатора
	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.DepthClipEnable = TRUE;

	// создание состояния растеризатора
	g_pd3dDevice->CreateRasterizerState(&rasterizerState, &pRasterizerState);

	// связывание состояния растеризатора с стадией растеризации
	g_pImmediateContext->RSSetState(pRasterizerState);

	return S_OK;
};

HRESULT InitGeometry(Vertex* vertexArray, LPCWSTR vertexShaderName, LPCWSTR pixelShaderName, LPCSTR vsShaderEntryPoint, LPCSTR psShaderEntryPoint) {
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
	vertexBufferSubresourceInitData.pSysMem = vertexArray;
	// смещение
	vertexBufferSubresourceInitData.SysMemPitch = 0;
	// смещение
	vertexBufferSubresourceInitData.SysMemSlicePitch = 0;

	// создание vertex buffer
	hr = g_pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferSubresourceInitData, &pPyramidVertexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	// привязка vertex buffer к IA stage
	// колчиство байт конкретного вершинного буфера, которое будет использоваться дальше
	UINT stride[] = { sizeof(Vertex) };
	// смещение до первого элемента конкретного вершинного буфера, который будет использован
	UINT offset[] = { 0 };
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pPyramidVertexBuffer, stride, offset);

	// указание какие примитивы собирать из вершинного буфера
	//g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// компиляция шейдеров
	//SHADERS_MODEL(g_featureLevel)

	// компиляция вершинного шейдера
	hr = CompileShader(vertexShaderName, vsShaderEntryPoint, shadersModel.vertexShaderModel, &VS_Buffer);
	if (FAILED(hr)) {
		return hr;
	}
	// компиляция пиксельного шейдера
	hr = CompileShader(pixelShaderName, psShaderEntryPoint, shadersModel.pixelShaderModel, &PS_Buffer);
	if (FAILED(hr)) {
		return hr;
	}

	// создание объекта вершинного шейдера
	hr = g_pd3dDevice->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &g_pVertexShader);
	// создание объекта пиксельного шейдера
	hr = g_pd3dDevice->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &g_pPixelShader);
	
	// привязка вершинного шейдера к конвейеру
	//g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, NULL);
	// привязка пиксельного шейдера к конвейеру
	//g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, NULL);
	
	// Описание Input-Layout Object
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	// Количество элементов в Input-Layout Object
	UINT numInputLayoutObject = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	// Создание Input-Layout Object
	hr = g_pd3dDevice->CreateInputLayout(layout, numInputLayoutObject, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &g_pInputLayoutObjectPyramid);

	// Связывание Input-layout object с конвейером
	//g_pImmediateContext->IASetInputLayout(g_pInputLayoutObjectPyramid);

	return S_OK;
};

void UpdateScene() {
	// обновление угла поворота пирамиды
	if (angleCBufferData.angle0 >= XM_2PI) {
		angleCBufferData.angle0 = angleCBufferData.angle0 - XM_2PI + ROTATION_ANGLE;
	}
	angleCBufferData.angle0 += ROTATION_ANGLE;

	/*
	D3D11_MAPPED_SUBRESOURCE angleCBufferUpdateData;
	g_pImmediateContext->Map(pAngleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &angleCBufferUpdateData);
	memcpy(angleCBufferUpdateData.pData, &angleCBufferData, sizeof(float));
	g_pImmediateContext->Unmap(pAngleBuffer, 0);
	*/

	// обновление значений угла
	g_pImmediateContext->UpdateSubresource(constantBufferArray[1], 0, NULL, &angleCBufferData, 0, 0); 
	
	// обновление матрицы поворота вокруг оси
	// проверяю то, что сам написал
	//RotationAroundAxis(g_XMIdentityR1, g_XMZero, angleCBufferData.angle0, &matricesWVP.mRotationAroundAxis);
	//matricesWVP.mRotationAroundAxis = XMMatrixTranspose(matricesWVP.mRotationAroundAxis);

	// обновление матрицы поворота вокруг оси
	matricesWVP.mRotationAroundAxis = XMMatrixTranspose(XMMatrixRotationAxis(g_XMIdentityR1, angleCBufferData.angle0));
	/**/
	//изменение динамических хитбоксов
	for (int i = 0; i < DYNAMIC_HIT_BOX_AMOUNT; ++i) {
		// если хитбокс вышел за пределы карты
		if (DynamicHitBoxesArray[i].activeCyclesAmount == 0.0f) {
			//возвращаем хитбокс в точку его спавна
			DynamicHitBoxesArray[i].position = DynamicHitBoxesArray[i].startPosition;
			//новый вектор движения
			CreateNewMoveVectorForDynamicHitBox(&DynamicHitBoxesArray[i]);
			//через сколько циклов хитбокс снова покинет карту
			DynamicHitBoxesArray[i].activeCyclesAmount = DynamicHitBoxCyclesAmount(&DynamicHitBoxesArray[i]);
			// поворот хитбокса
			if (DynamicHitBoxesArray[i].angle >= XM_2PI) {
				DynamicHitBoxesArray[i].activeCyclesAmount += -XM_2PI + ROTATION_ANGLE;
			}
			DynamicHitBoxesArray[i].activeCyclesAmount += ROTATION_ANGLE;
			//измеяем позицию пирамиды
			objectsPositions[i] = XMLoadFloat3(&DynamicHitBoxesArray[i].startPosition);
		}
		else {
			MoveAndRotationDynamicHitBox(*DynamicHitBoxesArray[i].moveVectorPtr, ROTATION_ANGLE, &DynamicHitBoxesArray[i]);
			--DynamicHitBoxesArray[i].activeCyclesAmount;
			//измеяем позицию пирамиды
			objectsPositions[i] = XMLoadFloat3(&DynamicHitBoxesArray[i].position);
		}
	}
};

void DrawScene(XMVECTOR* objectsPositionsArray) {	
	// Цвет пикселя
	const FLOAT backgroundColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };

	// Устанавливает цвет всеч пикселей поверхности RTV к единому значению
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, backgroundColor);

	// оччистка depth-stencil буфера
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// отрисовка пирамид
	// привязка вершинного шейдера к конвейеру
	g_pImmediateContext->VSSetShader(vertexShadersObj[0], NULL, NULL);
	// привязка пиксельного шейдера к конвейеру
	g_pImmediateContext->PSSetShader(pixelShadersObj[0], NULL, NULL);
	// привязка ILO 
	g_pImmediateContext->IASetInputLayout(g_pInputLayoutObjectPyramid);
	// привязка индекс-буфера
	g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//отрисовка каждой пирамиды
	for (int i = 0; i < 3; ++i) {
		SetWorldMatrix(objectsPositionsArray[i], XMVECTORF32{ 1.0f, 1.0f, 1.0f, 0.0f }, & matricesWVP.mWorld);
		g_pImmediateContext->UpdateSubresource(constantBufferArray[0], 0, NULL, &matricesWVP, 0, 0);

		g_pImmediateContext->DrawIndexed(12, 0, 0);
	}
	
	// отрисовка стен
	// привязка вершинного шейдера к конвейеру
	g_pImmediateContext->VSSetShader(vertexShadersObj[1], NULL, NULL);
	// привязка пиксельного шейдера к конвейеру не нужна, используется тот же шейдер
	// привязка ILO 
	g_pImmediateContext->IASetInputLayout(g_pInputLayoutObjectWalls);
	// привязка индекс-буфера
	g_pImmediateContext->IASetIndexBuffer(pWallsIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// отрисовка
	g_pImmediateContext->DrawIndexed(6 * 4, 0, 0);

	// Вывод на дисплей поверхности Back Buffer
	g_pSwapChain->Present(0, 0);

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
};

void ReleaseObjects() {
	if (dynamicMemory != NULL) {
		VirtualFree(dynamicMemory, 0, MEM_RELEASE);
	}
	if (pWallsIndexBuffer != NULL) {
		pWallsIndexBuffer->Release();
		pWallsIndexBuffer = NULL;
	}
	if (pIndexBuffer != NULL) {
		pIndexBuffer->Release();
		pIndexBuffer = NULL;
	}
	for (int i = 0; i < SHADERS_CONST_BUFFERS_NUM; ++i) {
		if (constantBufferArray[i] != NULL) {
			constantBufferArray[i]->Release();
			constantBufferArray[i] = NULL;
		}
	}
	if (g_pInputLayoutObjectWalls != NULL) {
		g_pInputLayoutObjectWalls->Release();
		g_pInputLayoutObjectWalls = NULL;
	}
	if (pWallsVertexBuffer != NULL) {
		pWallsVertexBuffer->Release();
		pWallsVertexBuffer = NULL;
	}
	if (g_pInputLayoutObjectPyramid != NULL) {
		g_pInputLayoutObjectPyramid->Release();
		g_pInputLayoutObjectPyramid = NULL;
	}
	if (pPyramidVertexBuffer != NULL) {
		pPyramidVertexBuffer->Release();
		pPyramidVertexBuffer = NULL;
	}
	for(int i = 0; i < PIXEL_SHADERS_NUM; ++i){
		if(pixelShadersObj[i] != NULL){
			pixelShadersObj[i]->Release();
			pixelShadersObj[i] = NULL;
		}
	}
	for(int i = 0; i < VERTEX_SHADERS_NUM; ++i){
		if(vertexShadersObj[i] != NULL){
			vertexShadersObj[i]->Release();
			vertexShadersObj[i] = NULL;
		}
	}
	for (int i = 0; i < SHADERS_TOTAL_NUM; ++i) {
		if(shadersBufferArray[i] != NULL){
			shadersBufferArray[i]->Release();
			shadersBufferArray[i] = NULL;
		}
	}
	if(pRasterizerState != NULL) {
		pRasterizerState->Release();
		pRasterizerState = NULL;
	}
	if (g_pDepthStencilView != NULL) {
		g_pDepthStencilView->Release();
		g_pDepthStencilView = NULL;
	}
	if (pDSState != NULL) {
		pDSState->Release();
		pDSState = NULL;
	}
	if (depthStencilTexture != NULL) {
		depthStencilTexture->Release();
		depthStencilTexture = NULL;
	}
	if (g_pRenderTargetView != NULL) {
		g_pRenderTargetView->Release();
		g_pRenderTargetView = NULL;
	}
	if (g_pSwapChain != NULL) {
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if (g_pImmediateContext != NULL) {
		g_pImmediateContext->Release();
		g_pImmediateContext = NULL;
	}
	if (g_pd3dDevice != NULL) {
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}
};

HRESULT CompileShader(LPCWSTR srcName, LPCSTR entryPoint, LPCSTR target, ID3DBlob** buffer) {
	HRESULT hr;
	ID3DBlob* errorsBuffer = NULL;
	HANDLE errorsFileHandle = NULL;
	DWORD bytesWritten = NULL;

	hr = D3DCompileFromFile(srcName, NULL, NULL, entryPoint, target, D3DCOMPILE_DEBUG, NULL, buffer, &errorsBuffer);
	// вывод ошибок компиляции, если они есть
	if (FAILED(hr)) {
		if (errorsBuffer != NULL) {
			// создаем или открываем файл, где будет находится текст с ошибками компиляции шейдера
			errorsFileHandle = CreateFile(L"res\\shaderErrors.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			// записываем ошибки компиляции в файл
			WriteFile(errorsFileHandle, errorsBuffer->GetBufferPointer(), errorsBuffer->GetBufferSize(), &bytesWritten, NULL);
			CloseHandle(errorsFileHandle);
			errorsBuffer->Release();
		}
		if ((*buffer) != NULL) {
			(*buffer)->Release();
			*buffer = NULL;
		}
	}
	
	if (errorsBuffer != NULL){
		errorsBuffer->Release();
	}
	return hr;
};

HRESULT InitMatrices(WORD* indices) {
	HRESULT hr;

	// описание константного буфера матриц
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.ByteWidth = sizeof(MatricesBuffer);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// создание константного буфера матриц
	hr = g_pd3dDevice->CreateBuffer(&constantBufferDesc, NULL, &constantBufferArray[0]);
	if (FAILED(hr)) {
		return hr;
	}

	// описание буфера угла
	D3D11_BUFFER_DESC angleBufferDesc;
	angleBufferDesc.ByteWidth = sizeof(AngleConstantBuffer);
	angleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	angleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	angleBufferDesc.CPUAccessFlags = 0;
	angleBufferDesc.MiscFlags = 0;
	angleBufferDesc.StructureByteStride = 0;

	// описание инициализирующей информации буфера угла
	D3D11_SUBRESOURCE_DATA angleBufferInitData;
	angleBufferInitData.pSysMem = &angleCBufferData;
	angleBufferInitData.SysMemPitch = 0;
	angleBufferInitData.SysMemSlicePitch = 0;

	// создание буфера угла
	hr = g_pd3dDevice->CreateBuffer(&angleBufferDesc, &angleBufferInitData, &constantBufferArray[1]);
	if (FAILED(hr)) {
		return hr;
	}

	// связывание констнатного буфера с шейдером вершин
	g_pImmediateContext->VSSetConstantBuffers(0, 2, constantBufferArray);
	/*
	//описание ресурса вершинного шейдера
	D3D11_SHADER_RESOURCE_VIEW_DESC angleBufferVSResorceDesc;
	angleBufferVSResorceDesc.Format = DXGI_FORMAT_R32_FLOAT;
	angleBufferVSResorceDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	angleBufferVSResorceDesc.Buffer.FirstElement = 0;
	angleBufferVSResorceDesc.Buffer.NumElements = 1;

	//создание ресурса вершинного шейдера
	hr = g_pd3dDevice->CreateShaderResourceView(pAngleBuffer, &angleBufferVSResorceDesc, &pAngleBufferVSResource);
	if (FAILED(hr)) {
		return hr;
	}

	// привязка ресурса с углом к шейдеру вершин
	g_pImmediateContext->VSSetShaderResources(0, 1, &pAngleBufferVSResource);
	*/
	// описание индекс буфера
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(WORD) * 4 * 3;
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
	hr = g_pd3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferInitData, &pIndexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
};
/*
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
	hr = g_pd3dDevice->CreateInputLayout(layout, numInputLayoutObject, shadersBufferArray[1]->GetBufferPointer(), shadersBufferArray[1]->GetBufferSize(), &g_pInputLayoutObjectWalls);
	
};

inline void InitPyramidVertices(Vertex* pyramidVertexArray){
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

inline void InitShaders(){
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
	
	switch(CheckCompiledShadersFromLogFile(fileDirBuffer, fileDirBufLength, (WCHAR*)L"\\res\\log.txt", WCHAR_NUM_OF_STRING(L"\\res\\log.txt"))){
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

inline HRESULT InitWallsIndexBuffer(WORD* indices){
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
*/



