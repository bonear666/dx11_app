LRESULT CALLBACK StartUpWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hWnd, message, wParam, lParam);
};


// �������-���������� ���������, ����������� ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	// ���������, ���������� ����������� ���������� ��� ��������� � ��������� ����� ����
	PAINTSTRUCT ps;
	// ��������� �� ����������(��� ������ ����������) Device Context
	HDC hdc;

	switch (message) {
	// ��������� ��������� �� ���� ��� ����������
	case(WM_INPUT): {
		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpRawInput[sizeof(RAWINPUT)];

		UINT resData = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpRawInput, &dwSize, sizeof(RAWINPUTHEADER));
		DWORD resData2 = ((RAWINPUT*)lpRawInput)->header.dwType;
		if (resData2 == RIM_TYPEKEYBOARD) { // ���� ��������� ��������� �� ����������
			USHORT pressedKey = ((RAWINPUT*)lpRawInput)->data.keyboard.VKey;

			switch (pressedKey) {
			case(0x57): {// W
				// ����� ���� �� ������ ��������� moveAheadVector � ��������� ������� view matrix, �� ��� ������� ��� ���������������, ������� ��� ������� �� ��������� 
				matricesWVP.mView = XMMatrixTranspose(XMMatrixTranslationFromVector(moveAheadVector)) * matricesWVP.mView;
				//����� ����� � ����� ������� ������� ������ � xmfloat3, ����� ���� � xmvector
				// ������ ������� ������ ������������ ���������� ���������
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
		else // ���� ��������� ��������� �� ����
		{
			LONG mouseX = ((RAWINPUT*)lpRawInput)->data.mouse.lLastX;
			LONG mouseY = ((RAWINPUT*)lpRawInput)->data.mouse.lLastY;
			static XMVECTOR newYAxisRotation = g_XMIdentityR1;

			XMMATRIX matrixRotationNewX = XMMatrixRotationX(-XM_PI * 0.0005 * mouseY); //������� �������� ������ ��� X
			XMMATRIX matrixRotationNewY = XMMatrixRotationNormal(newYAxisRotation, -XM_PI * 0.0005 * mouseX); //������� �������� ������ ��� Y
			newYAxisRotation = XMVector3Transform(newYAxisRotation, matrixRotationNewX); //��������� ��� Y, ������ ������� ���������� �������
			moveAheadVector = XMVector3Transform(moveAheadVector, matrixRotationNewX);
			moveBackVector = (-1.0f) * moveAheadVector;

			matricesWVP.mView = XMMatrixTranspose(matrixRotationNewX * matrixRotationNewY) * matricesWVP.mView;

			//�� �� �����, ������ ����������� ����� ���������� ����� ������� ����
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

			//��������� moveAheadVector � ���������� ������� ���������
			sseProxyRegister0_Matrix = XMMatrixRotationY(XM_PI * 0.0005 * mouseX);
			moveAheadVectorInGlobalCoord = XMVector3Transform(moveAheadVectorInGlobalCoord, sseProxyRegister0_Matrix);
			//��������� moveRightVector � ���������� ������� ���������
			moveRightVectorInGlobalCoord = XMVector3Transform(moveRightVectorInGlobalCoord, sseProxyRegister0_Matrix);
		}
		break; 
	}

	case(WM_PAINT): {
		// ���������� ��������� ps, � ������� Update Region
		hdc = BeginPaint(hWnd, &ps);

		// ������� Update Region, � ������������ Device Context
		EndPaint(hWnd, &ps);
		break;
	} 
	case(WM_DESTROY): {
		//�������� ��������� WM_QUIT
		PostQuitMessage(0);
		break;
	}
	default:
		DefWindowProc(hWnd, message, wParam, lParam);
	}

	UpdateScene();
	DrawScene(objectsPositions);
	// �������� �� ������������ ������ � ������������ � ������������� ����������
	// ���� ������ ������� � ������ ������� ����������� ���������
	if (ChangesOfStaticHtBoxesArea) {
		DefineCurrentStaticHtBoxesArea();
	}
	//StaticHitBoxesCollisionDetection();
	DynamicHitBoxesCollisionDetection();

	return 0;
};

HRESULT CreateDirect3DComponents(UINT widthParam, UINT heightParam) {
	HRESULT hr;

	//������������� DirectX �����������	

	// ���������, ����������� back buffer SwapChain-a
	DXGI_MODE_DESC backBuffer;
	ZeroMemory(&backBuffer, sizeof(DXGI_MODE_DESC));
	// ������
	backBuffer.Width = widthParam;
	// ������
	backBuffer.Height = heightParam;
	// ������� ���������� ��������
	backBuffer.RefreshRate.Numerator = 60;
	backBuffer.RefreshRate.Denominator = 1;
	// ������ ��������
	backBuffer.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// ������� �������� �����������(Surface)
	backBuffer.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// ����, ������������ ��, ��� ����� ������������� ����������� ��� ������������ ���������� ��������� ��������
	backBuffer.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ���������, ����������� ���� ������ (Swap Chain)
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	// �������� backBuffer-a, ������� ������ � ������ Swap Chain-a
	sd.BufferDesc = backBuffer;
	// �������� ���������������(�� ���������)
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	// ���� ������������� �������
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// ���������� ������� � Swap Chain-e (��� ��� ���������� �������� � ������� ������, �� ��� ������� ����������� ���������� ������ ���-�� ������� ��� ����� ��������������� Front Buffer-a
	//(Front Buffer-�� �������� ������� ����)), �� ��� full-screen mode ���������� ���������� ������ 2 ������ ��� ������� �����������
	sd.BufferCount = 2;
	// ���������� ���� ����������
	sd.OutputWindow = g_hWnd;
	// ����� ���������� � windowed mode ��� full-screen mode
	sd.Windowed = TRUE;
	// ��� ������ ������ ������� ����� ����, ��� front buffer ���� back buffer-��
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// �������������� ��������� Swap Chain-�
	sd.Flags = NULL;

	// ���� �������� Device, Device Contex, Swap Chain, Render Target View, View Port

	// ��������� feature level, ������� �������������� �����������, � ��������� ������������ ��� ��������
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0 };

	// ���������� ��������� � ������� featureLevels
	UINT numFeatureLevels = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);

	// ���� � ���� ������������ ������ hardware type
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
		D3D_DRIVER_TYPE_SOFTWARE
	};

	// ���������� ��������� � ������� dreiverTypes
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(D3D_DRIVER_TYPE);

	// ���������� �������� Direct3D Device, Device Context, Swap Chain, View Port

	// ��������� ������ CreateDeviceAndSwapChain
	HRESULT createDeviceDeviceContextSwapChainResult(S_OK);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex) {
		createDeviceDeviceContextSwapChainResult = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[driverTypeIndex], NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(createDeviceDeviceContextSwapChainResult)) {
			goto createDeviceDeviceContextSwapChainLoopExit;
		}
	}
	// ���������� ����� �� �����
	return createDeviceDeviceContextSwapChainResult;
	// �������� ����� �� �����
createDeviceDeviceContextSwapChainLoopExit:

	// ��������� ������� � back buffer
	// ��������� �� back buffer
	ID3D11Texture2D* pBackBuffer(NULL); 
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	// �������� Render Target View
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);

	// ������������ ���������� back buffer, back buffer ������ �� ����� 
	pBackBuffer->Release();
	pBackBuffer = NULL; // ���� �������� �� �����������, � ������ ������

	// �������� View Port, ������� ����������� RTV, ������� � ����� ������������ �� �������. ����� view port ��������� RTV �� ���������� ��������� � �������� ����������� 
	D3D11_VIEWPORT viewPort;
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = widthParam;
	viewPort.Height = heightParam;
	// ������� ����������� ��������, ������� ����� ������������ � view port
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	// ���������� view port � ����������� ����������
	g_pImmediateContext->RSSetViewports(1, &viewPort);

	// �������� depth stencil ������
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

	// �������� ����, ��� ����� ���������� z-test � stencil-test
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace = { D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_ALWAYS };
	dsDesc.BackFace = { D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP , D3D11_STENCIL_OP_KEEP , D3D11_COMPARISON_NEVER };

	// �������� ��������� depth-stencil �����
	g_pd3dDevice->CreateDepthStencilState(&dsDesc, &pDSState);

	// ���������� �������� depth-stencil ����� � OM stage
	g_pImmediateContext->OMSetDepthStencilState(pDSState, 1);

	// �������� depth-stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = NULL;
	descDSV.Texture2D.MipSlice = 0;

	// �������� depth-stencil view
	g_pd3dDevice->CreateDepthStencilView(depthStencilTexture, &descDSV, &g_pDepthStencilView);

	// �������� RTV � DSV � Output-Merger Stage
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// �������� ��������� �������������
	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.DepthClipEnable = TRUE;

	// �������� ��������� �������������
	g_pd3dDevice->CreateRasterizerState(&rasterizerState, &pRasterizerState);

	// ���������� ��������� ������������� � ������� ������������
	g_pImmediateContext->RSSetState(pRasterizerState);

	return S_OK;
};

HRESULT MyCreateWindow(CONST WCHAR* wndClassNameParam, CONST WCHAR* wndNameParam, int widthParam, int heightParam, HINSTANCE hInstanceParam, int nShowCmdParam) {
	// ���������, ����������� ����� ����
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
	// ������ ��������� wndClass
	wndClass.cbSize = sizeof(WNDCLASSEX);
	// ����� ������ ����
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	// ��������� �� �������, �������������� ���������, ����������� ����
	wndClass.lpfnWndProc = StartUpWndProc;
	// ���������� �������������� ������ ��� ��������� wndClass
	wndClass.cbClsExtra = NULL;
	// ���������� �������������� ������ ��� ���������� ����
	wndClass.cbWndExtra = NULL;
	// ���������� ����������
	wndClass.hInstance = hInstanceParam;
	// ���������� ������ ����
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// ���������� ������� ����
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// ���������� �����
	wndClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	// ��������� �� ������-��� ���� ������ 
	wndClass.lpszMenuName = NULL;
	// ��� ������ ����
	wndClass.lpszClassName = wndClassNameParam;
	// ���������� ������ ����, ������� ������������ �� ������ �����
	wndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// ����������� ������ ����
	if (RegisterClassEx(&wndClass) == 0) {
		return E_FAIL;
	}

	// �������� ����
	g_hWnd = CreateWindowEx(NULL, wndClassNameParam, wndNameParam, WS_OVERLAPPEDWINDOW, 0, 0, widthParam, heightParam, NULL, NULL, hInstanceParam, NULL);
	if (g_hWnd == 0) {
		return E_FAIL;
	}

	// ����������� ��������� ���������� � ����
	RAWINPUTDEVICE rid[2];
	// ����������
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	rid[0].dwFlags = RIDEV_NOLEGACY;
	rid[0].hwndTarget = g_hWnd;
	// ����
	rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[1].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[1].dwFlags = RIDEV_NOLEGACY;
	rid[1].hwndTarget = g_hWnd;
	RegisterRawInputDevices(rid, 2, sizeof(rid[0]));

	// ����� ���� �� ������� 
	ShowWindow(g_hWnd, nShowCmdParam);

	return S_OK;
};

void UpdateScene() {
	// ���������� ���� �������� ��������
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

	// ���������� �������� ����
	g_pImmediateContext->UpdateSubresource(constantBufferArray[1], 0, NULL, &angleCBufferData, 0, 0); 
	
	// ���������� ������� �������� ������ ���
	// �������� ��, ��� ��� �������
	//RotationAroundAxis(g_XMIdentityR1, g_XMZero, angleCBufferData.angle0, &matricesWVP.mRotationAroundAxis);
	//matricesWVP.mRotationAroundAxis = XMMatrixTranspose(matricesWVP.mRotationAroundAxis);

	// ���������� ������� �������� ������ ���
	matricesWVP.mRotationAroundAxis = XMMatrixTranspose(XMMatrixRotationAxis(g_XMIdentityR1, angleCBufferData.angle0));
	/**/
	//��������� ������������ ���������
	for (int i = 0; i < DYNAMIC_HIT_BOX_AMOUNT; ++i) {
		// ���� ������� ����� �� ������� �����
		if (DynamicHitBoxesArray[i].activeCyclesAmount == 0.0f) {
			//���������� ������� � ����� ��� ������
			DynamicHitBoxesArray[i].position = DynamicHitBoxesArray[i].startPosition;
			//����� ������ ��������
			CreateNewMoveVectorForDynamicHitBox(&DynamicHitBoxesArray[i]);
			//����� ������� ������ ������� ����� ������� �����
			DynamicHitBoxesArray[i].activeCyclesAmount = DynamicHitBoxCyclesAmount(&DynamicHitBoxesArray[i]);
			// ������� ��������
			if (DynamicHitBoxesArray[i].angle >= XM_2PI) {
				DynamicHitBoxesArray[i].activeCyclesAmount += -XM_2PI + ROTATION_ANGLE;
			}
			DynamicHitBoxesArray[i].activeCyclesAmount += ROTATION_ANGLE;
			//������� ������� ��������
			objectsPositions[i] = XMLoadFloat3(&DynamicHitBoxesArray[i].startPosition);
		}
		else {
			MoveAndRotationDynamicHitBox(*DynamicHitBoxesArray[i].moveVectorPtr, ROTATION_ANGLE, &DynamicHitBoxesArray[i]);
			--DynamicHitBoxesArray[i].activeCyclesAmount;
			//������� ������� ��������
			objectsPositions[i] = XMLoadFloat3(&DynamicHitBoxesArray[i].position);
		}
	}
};

void DrawScene(XMVECTOR* objectsPositionsArray) {
	// ���� �������
	const FLOAT backgroundColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };

	// ������������� ���� ���� �������� ����������� RTV � ������� ��������
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, backgroundColor);

	// �������� depth-stencil ������
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//��������� ������ ��������
	for (int i = 0; i < 3; ++i) {
		SetWorldMatrix(objectsPositionsArray[i], XMVECTORF32{ 1.0f, 1.0f, 1.0f, 0.0f }, & matricesWVP.mWorld);
		g_pImmediateContext->UpdateSubresource(constantBufferArray[0], 0, NULL, &matricesWVP, 0, 0);

		g_pImmediateContext->DrawIndexed(12, 0, 0);
	}
	// ��������� ���������� 
	//g_pImmediateContext->DrawIndexed(12, 0, 0);

	// ����� �� ������� ����������� Back Buffer
	g_pSwapChain->Present(0, 0);

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
};

HRESULT CompileShader(LPCWSTR srcName, LPCSTR entryPoint, LPCSTR target, ID3DBlob** buffer) {
	HRESULT hr;
	ID3DBlob* errorsBuffer = NULL;
	HANDLE errorsFileHandle = NULL;
	DWORD bytesWritten = NULL;

	hr = D3DCompileFromFile(srcName, NULL, NULL, entryPoint, target, D3DCOMPILE_DEBUG, NULL, buffer, &errorsBuffer);
	// ����� ������ ����������, ���� ��� ����
	if (FAILED(hr)) {
		if (errorsBuffer != NULL) {
			// ������� ��� ��������� ����, ��� ����� ��������� ����� � �������� ���������� �������
			errorsFileHandle = CreateFile(L"shaderErrors.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			// ���������� ������ ���������� � ����
			WriteFile(errorsFileHandle, errorsBuffer->GetBufferPointer(), errorsBuffer->GetBufferSize(), &bytesWritten, NULL);
			CloseHandle(errorsFileHandle);
			errorsBuffer->Release();
		}
		if ((*buffer) != NULL) {
			(*buffer)->Release();
			*buffer = NULL;
		}
	}
	return hr;
};

HRESULT InitGeometry(Vertex* vertexArray, LPCWSTR vertexShaderName, LPCWSTR pixelShaderName, LPCSTR vsShaderEntryPoint, LPCSTR psShaderEntryPoint) {
	HRESULT hr;

	// �������� vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	// ������ ������
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	// ������� ������� CPU � GPU � ������ 
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// � ����� ������ ��������� ����������� �����
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// ������� ������� CPU � ������
	vertexBufferDesc.CPUAccessFlags = 0;
	// �������������� ��������� ������
	vertexBufferDesc.MiscFlags = 0;
	// ������ ������� �������� ������, ���� ������ ����� �������� structured buffer. (�� ���������)
	vertexBufferDesc.StructureByteStride = 0;

	// ���������� ��� ������������� ����������
	D3D11_SUBRESOURCE_DATA vertexBufferSubresourceInitData;
	// ��������� �� ���������������� ������
	vertexBufferSubresourceInitData.pSysMem = vertexArray;
	// ��������
	vertexBufferSubresourceInitData.SysMemPitch = 0;
	// ��������
	vertexBufferSubresourceInitData.SysMemSlicePitch = 0;

	// �������� vertex buffer
	hr = g_pd3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferSubresourceInitData, &pVertexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	// �������� vertex buffer � IA stage
	// ��������� ���� ����������� ���������� ������, ������� ����� �������������� ������
	UINT stride[] = { sizeof(Vertex) };
	// �������� �� ������� �������� ����������� ���������� ������, ������� ����� �����������
	UINT offset[] = { 0 };
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, stride, offset);

	// �������� ����� ��������� �������� �� ���������� ������
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���������� ��������
	SHADERS_MODEL(g_featureLevel)

	// ���������� ���������� �������
	hr = CompileShader(vertexShaderName, vsShaderEntryPoint, shadersModel.vertexShaderModel, &VS_Buffer);
	if (FAILED(hr)) {
		return hr;
	}
	// ���������� ����������� �������
	hr = CompileShader(pixelShaderName, psShaderEntryPoint, shadersModel.pixelShaderModel, &PS_Buffer);
	if (FAILED(hr)) {
		return hr;
	}

	// �������� ������� ���������� �������
	hr = g_pd3dDevice->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &g_pVertexShader);
	// �������� ������� ����������� �������
	hr = g_pd3dDevice->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &g_pPixelShader);
	
	// �������� ���������� ������� � ���������
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, NULL);
	// �������� ����������� ������� � ���������
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, NULL);
	
	// �������� Input-Layout Object
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	
	// ���������� ��������� � Input-Layout Object
	UINT numInputLayoutObject = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	// �������� Input-Layout Object
	hr = g_pd3dDevice->CreateInputLayout(layout, numInputLayoutObject, VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), &g_pInputLayoutObject);

	// ���������� Input-layout object � ����������
	g_pImmediateContext->IASetInputLayout(g_pInputLayoutObject);

	return S_OK;
}

HRESULT InitMatrices(WORD* indices) {
	HRESULT hr;

	// �������� ������������ ������ ������
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.ByteWidth = sizeof(MatricesBuffer);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// �������� ������������ ������ ������
	hr = g_pd3dDevice->CreateBuffer(&constantBufferDesc, NULL, &constantBufferArray[0]);
	if (FAILED(hr)) {
		return hr;
	}

	// �������� ������ ����
	D3D11_BUFFER_DESC angleBufferDesc;
	angleBufferDesc.ByteWidth = sizeof(AngleConstantBuffer);
	angleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	angleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	angleBufferDesc.CPUAccessFlags = 0;
	angleBufferDesc.MiscFlags = 0;
	angleBufferDesc.StructureByteStride = 0;

	// �������� ���������������� ���������� ������ ����
	D3D11_SUBRESOURCE_DATA angleBufferInitData;
	angleBufferInitData.pSysMem = &angleCBufferData;
	angleBufferInitData.SysMemPitch = 0;
	angleBufferInitData.SysMemSlicePitch = 0;

	// �������� ������ ����
	hr = g_pd3dDevice->CreateBuffer(&angleBufferDesc, &angleBufferInitData, &constantBufferArray[1]);
	if (FAILED(hr)) {
		return hr;
	}

	// ���������� ������������ ������ � �������� ������
	g_pImmediateContext->VSSetConstantBuffers(0, 2, constantBufferArray);
	/*
	//�������� ������� ���������� �������
	D3D11_SHADER_RESOURCE_VIEW_DESC angleBufferVSResorceDesc;
	angleBufferVSResorceDesc.Format = DXGI_FORMAT_R32_FLOAT;
	angleBufferVSResorceDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	angleBufferVSResorceDesc.Buffer.FirstElement = 0;
	angleBufferVSResorceDesc.Buffer.NumElements = 1;

	//�������� ������� ���������� �������
	hr = g_pd3dDevice->CreateShaderResourceView(pAngleBuffer, &angleBufferVSResorceDesc, &pAngleBufferVSResource);
	if (FAILED(hr)) {
		return hr;
	}

	// �������� ������� � ����� � ������� ������
	g_pImmediateContext->VSSetShaderResources(0, 1, &pAngleBufferVSResource);
	*/
	// �������� ������ ������
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(WORD) * 4 * 3;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �������� ������ ������
	D3D11_SUBRESOURCE_DATA indexBufferInitData;
	indexBufferInitData.pSysMem = indices;
	indexBufferInitData.SysMemPitch = 0;
	indexBufferInitData.SysMemSlicePitch = 0;

	//�������� ������ ������
	hr = g_pd3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferInitData, &pIndexBuffer);
	if (FAILED(hr)) {
		return hr;
	}

	//�������� ������ ������ � ���������
	g_pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	return S_OK;
};

void SetProjectionMatrix(MatricesBuffer* pMatricesBuffer, FLOAT angleHoriz, FLOAT angleVert, FLOAT nearZ, FLOAT farZ, BOOL saveProportionsFlag) {
	// (0 < angle < PI/2) => (0 < tg(angle) < +inf) => (0 < newCoeff < 1)

	FLOAT sinAngle;
	FLOAT cosAngle;
	FLOAT tangentAngle;
	FLOAT newCoeff;

	//FLOAT nearZ = 2.5f; // ������� �������� ���������
	//FLOAT farZ = 3.2f; // ������� ��������� ���������
	//FLOAT mulCoeff = (farZ - nearZ + 1.0f) / (farZ - nearZ);

	FLOAT zCoeff = farZ / (farZ - nearZ);
	//FLOAT wCoeff = farZ - farZ * zCoeff;
	FLOAT wCoeff = -nearZ * zCoeff;


	if (saveProportionsFlag == false) {
		XMScalarSinCos(&sinAngle, &cosAngle, angleHoriz);
		tangentAngle = sinAngle / cosAngle;
		newCoeff = 1.0f / (1.0f + tangentAngle);
		pMatricesBuffer->mProjection.r[0] = XMVectorSet(newCoeff, 0.0f, 0.0f, 0.0f);

		XMScalarSinCos(&sinAngle, &cosAngle, angleVert);
		tangentAngle = sinAngle / cosAngle;
		newCoeff = 1.0f / (1.0f + tangentAngle);
		pMatricesBuffer->mProjection.r[1] = XMVectorSet(0.0f, newCoeff, 0.0f, 0.0f);

		pMatricesBuffer->mProjection.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		pMatricesBuffer->mProjection.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		pMatricesBuffer->mProjection = XMMatrixTranspose(pMatricesBuffer->mProjection);
		g_pImmediateContext->UpdateSubresource(constantBufferArray[0], 0, 0, pMatricesBuffer, 0, 0);
	}
	else {
		XMScalarSinCos(&sinAngle, &cosAngle, MaxElement(angleHoriz, angleVert));
		tangentAngle = sinAngle / cosAngle;
		newCoeff = 1.0f / (1.0f + tangentAngle);
		pMatricesBuffer->mProjection.r[0] = XMVectorSet(newCoeff, 0.0f, 0.0f, 0.0f);
		pMatricesBuffer->mProjection.r[1] = XMVectorSet(0.0f, newCoeff, 0.0f, 0.0f);
		pMatricesBuffer->mProjection.r[2] = XMVectorSet(0.0f, 0.0f, zCoeff * 0.25f * newCoeff, 0.25f * newCoeff); // 0.25f * newCoeff
		pMatricesBuffer->mProjection.r[3] = XMVectorSet(0.0f, 0.0f, wCoeff, 0.0f);

		SaveProportions(pMatricesBuffer, g_hWnd);
	}
};

void SetProjectionMatrixWithCameraDistance(MatricesBuffer* pMatricesBuffer, FLOAT angleHoriz, FLOAT angleVert, FLOAT nearZ, FLOAT farZ, FLOAT cameraDistance, BOOL saveProportionsFlag) {
	// (0 < angle < PI/2) => (0 < tg(angle) < +inf) => (0 < newCoeff < 1)

	FLOAT sinAngle;
	FLOAT cosAngle;
	FLOAT tangentAngle;
	FLOAT newCoeff;

	//nearZ = 0.6f; // ������� �������� ���������, �������� �� W ����������
	//farZ = 4.2f; // ������� ��������� ���������, �������� �� W ����������

	FLOAT zCoeff = (farZ - nearZ + cameraDistance) / (farZ - nearZ);
	FLOAT wCoeff = farZ - nearZ + cameraDistance - farZ * zCoeff;

	if (saveProportionsFlag == false) {
		XMScalarSinCos(&sinAngle, &cosAngle, angleHoriz);
		tangentAngle = sinAngle / cosAngle;
		newCoeff = 1.0f / (1.0f + tangentAngle);
		pMatricesBuffer->mProjection.r[0] = XMVectorSet(newCoeff, 0.0f, 0.0f, 0.0f);

		XMScalarSinCos(&sinAngle, &cosAngle, angleVert);
		tangentAngle = sinAngle / cosAngle;
		newCoeff = 1.0f / (1.0f + tangentAngle);
		pMatricesBuffer->mProjection.r[1] = XMVectorSet(0.0f, newCoeff, 0.0f, 0.0f);

		pMatricesBuffer->mProjection.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		pMatricesBuffer->mProjection.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		pMatricesBuffer->mProjection = XMMatrixTranspose(pMatricesBuffer->mProjection);
		g_pImmediateContext->UpdateSubresource(constantBufferArray[0], 0, 0, pMatricesBuffer, 0, 0);
	}
	else {
		XMScalarSinCos(&sinAngle, &cosAngle, MaxElement(angleHoriz, angleVert));
		tangentAngle = sinAngle / cosAngle;
		newCoeff = 1.0f / (1.0f + tangentAngle);
		pMatricesBuffer->mProjection.r[0] = XMVectorSet(newCoeff, 0.0f, 0.0f, 0.0f);
		pMatricesBuffer->mProjection.r[1] = XMVectorSet(0.0f, newCoeff, 0.0f, 0.0f);
		pMatricesBuffer->mProjection.r[2] = XMVectorSet(0.0f, 0.0f, zCoeff * 0.25f * newCoeff, 0.25f * newCoeff); // 0.25f * newCoeff
		pMatricesBuffer->mProjection.r[3] = XMVectorSet(0.0f, 0.0f, wCoeff, cameraDistance - nearZ);

		SaveProportions(pMatricesBuffer, g_hWnd);
	}
};

void SaveProportions(MatricesBuffer* pMatricesBuffer, HWND hWnd) {
	RECT rectangle;
	FLOAT windowWidth;
	FLOAT windowHeight;
	FLOAT windowCoeff;

	GetClientRect(hWnd, &rectangle);
	windowWidth = rectangle.right - rectangle.left;
	windowHeight = rectangle.bottom - rectangle.top;

	windowCoeff = windowHeight / windowWidth;

	if (windowCoeff >= 1.0f) {
		//pMatricesBuffer->mProjection._22 *=  1 / windowCoeff;
		pMatricesBuffer->mProjection.r[1] = _mm_mul_ps(pMatricesBuffer->mProjection.r[1], XMVectorSet(1.0f, 1.0f / windowCoeff, 1.0f, 1.0f));
	}
	else
	{
		//pMatricesBuffer->mProjection._11 *= windowCoeff;
		pMatricesBuffer->mProjection.r[0] = _mm_mul_ps(pMatricesBuffer->mProjection.r[0], XMVectorSet(windowCoeff, 1.0f, 1.0f, 1.0f));
	}

	pMatricesBuffer->mProjection = XMMatrixTranspose(pMatricesBuffer->mProjection);
	g_pImmediateContext->UpdateSubresource(constantBufferArray[0], 0, 0, pMatricesBuffer, 0, 0);
};

FLOAT MaxElement(FLOAT arg0, FLOAT arg1) {
	if (arg0 >= arg1) {
		return arg0;
	}
	return arg1;
};

HRESULT InvertMatrix(XMVECTOR zAxis, XMVECTOR yAxis, XMMATRIX* invertibleMatrix) {
	// �������� �������� �� ��������������� 
	XMFLOAT4 dotProduct;
	XMStoreFloat4(&dotProduct, XMVector3Dot(zAxis, yAxis));
	if (dotProduct.x != 0.0f) {
		return E_FAIL;
	}
	
	// ���������� ����� ��� X
	XMVECTOR xAxis = XMVector3Cross(yAxis, zAxis); // ������ ������ ��������
	// ������� ������� ����������, ������ ����� ������ �������� �������

	// ������� �������� �������� �������, ������� �������
	XMVECTOR axisArray[] = { xAxis, yAxis, zAxis };

	// ������� ����� � �������� �������, ������� ��������� � ���������� ������ �������� �������
	int invertibleMatrixRowsOrder[3];

	// ��������� �������
	XMMATRIX invertibleMatrixOutput = {
	 XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
	 XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
	 XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
	 XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
	};

	//����� �������� ������� ������� ������
	for (size_t axisIndex = 0; axisIndex <= 1; ++axisIndex) { // ����� ���� �� ������� ������� �������
		for (size_t i = 0; i <= 2; ++i) { // ����� �� ����������� ������ ������� ������
			float axisElement = XMVectorGetByIndex(axisArray[axisIndex], i);
			if (axisElement != 0) {
				axisArray[axisIndex] /= XMVectorSet(axisElement, axisElement, axisElement, axisElement);
				invertibleMatrixOutput.r[axisIndex] /= XMVectorSet(axisElement, axisElement, axisElement, axisElement);
				invertibleMatrixRowsOrder[axisIndex] = i;

				for (size_t nextAxisIndex = axisIndex + 1; nextAxisIndex <= 2; ++nextAxisIndex) { // ����������� ������ � ������ �������
					float nextAxisElement = XMVectorGetByIndex(axisArray[nextAxisIndex], i);
					XMVECTOR mulVector = axisArray[axisIndex] * XMVectorSet(-nextAxisElement, -nextAxisElement, -nextAxisElement, -nextAxisElement); // ������ �������� ��������� axisArray[axisIndex] �� -nextAxisElement ���������� �������� �� ������, ��� ��� � ���� ������ ��������� ������� �����������
					XMVECTOR invertMatrixMulVector = invertibleMatrixOutput.r[axisIndex] * XMVectorSet(-nextAxisElement, -nextAxisElement, -nextAxisElement, -nextAxisElement);

					axisArray[nextAxisIndex] += mulVector;
					invertibleMatrixOutput.r[nextAxisIndex] += invertMatrixMulVector;
				}
				break;
			}
		}
	};
	for (size_t i = 0; i <= 2; ++i) { // ����� ��������� ������ ������� �������
		float axisElement = XMVectorGetByIndex(axisArray[2], i);
		if (axisElement != 0) {
			axisArray[2] /= XMVectorSet(axisElement, axisElement, axisElement, axisElement);
			invertibleMatrixOutput.r[2] /= XMVectorSet(axisElement, axisElement, axisElement, axisElement);
			invertibleMatrixRowsOrder[2] = i;

			break;
		}
	};
	for (size_t axisIndex = 2; axisIndex >= 1; --axisIndex) { // ����� ����� �� ������� ������� �������
		for (size_t i = 0; i <= 2; ++i) { // ����� �� ����������� ������ ������� ������
			float axisElement = XMVectorGetByIndex(axisArray[axisIndex], i);
			if (axisElement != 0) {
				for (size_t nextAxisIndex = axisIndex - 1; nextAxisIndex < UINT_MAX; --nextAxisIndex) { // ����������� ������ � ������ �������
					float nextAxisElement = XMVectorGetByIndex(axisArray[nextAxisIndex], i);
					XMVECTOR mulVector = axisArray[axisIndex] * XMVectorSet(-nextAxisElement, -nextAxisElement, -nextAxisElement, -nextAxisElement);
					XMVECTOR invertMatrixMulVector = invertibleMatrixOutput.r[axisIndex] * XMVectorSet(-nextAxisElement, -nextAxisElement, -nextAxisElement, -nextAxisElement);

					axisArray[nextAxisIndex] += mulVector;
					invertibleMatrixOutput.r[nextAxisIndex] += invertMatrixMulVector;
				}
				break;
			}
		}
	};

	//�������������� �������� ������� � ����������� ������� �����
	for (int i = 0; i <= 2; ++i) {
		invertibleMatrix->r[invertibleMatrixRowsOrder[i]] = invertibleMatrixOutput.r[i];
	};
	invertibleMatrix->r[3] = invertibleMatrixOutput.r[3];

	return S_OK;
};

HRESULT NewCoordinateSystemMatrix(XMVECTOR point, XMVECTOR zAxis, XMVECTOR yAxis, XMMATRIX* invertibleMatrix) {
	if (FAILED(InvertMatrix(zAxis, yAxis, invertibleMatrix))) {
		return E_FAIL;
	}
	// ��� ��������� ������� �� ������� �������� � ����� ������� ��������� �������� ������ ��������� ����� ������� ���������
	point *= XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f);
	*invertibleMatrix = XMMatrixTranslationFromVector(point) * (*invertibleMatrix);

	return S_OK;
};

void SetWorldMatrix(XMVECTOR point, XMVECTOR scale, XMMATRIX* worldMatrix) {
	*worldMatrix = XMMatrixTranspose(XMMatrixScalingFromVector(scale) * XMMatrixTranslationFromVector(point));
};

void InvertIndices(WORD* indicesArray, int size) {
	WORD triangleEnd;

	for (int i = 2; i < size; i = i + 3) {
		triangleEnd = indicesArray[i];

		indicesArray[i] = indicesArray[i-2];
		indicesArray[i - 2] = triangleEnd;
	}
};

XMVECTOR FindOrthogonalVector(XMVECTOR vector) { // ��������� ������������ ������������� �������� ����� ����, �� ������������� ����� ����� � �������� ����� �������������� �������
	XMFLOAT4 orthogonalVector;
	int zeroElementOrNot[3];

	for (int i = 0; i < 3; ++i) {
		if (XMVectorGetByIndex(vector, i) != 0.0f) {
			zeroElementOrNot[i] = 1;
		}
		zeroElementOrNot[i] = 0;
	}

	if ((zeroElementOrNot[1] != 0) || (zeroElementOrNot[2] != 0)) {
		orthogonalVector.x = 1.0f;

		if ((zeroElementOrNot[1] != 0) && (zeroElementOrNot[2] == 0)) {
			orthogonalVector.y = -XMVectorGetByIndex(vector, 0) / XMVectorGetByIndex(vector, 1);

			return XMVectorSet(orthogonalVector.x, orthogonalVector.y, 0.0f, 0.0f);
		}
		else
		{
			orthogonalVector.z = -XMVectorGetByIndex(vector, 0) / XMVectorGetByIndex(vector, 2);

			return XMVectorSet(orthogonalVector.x, 0.0f, orthogonalVector.z, 0.0f);
		}
	}
	else
	{
		return XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	}
};

XMVECTOR FindOrthogonalNormalizedVector(XMVECTOR vector) {
	XMFLOAT4 orthogonalVector;

	for (int i = 0; i < 3; ++i) {
		float coordinate = XMVectorGetByIndex(vector, i);
		if ((coordinate != 0.0f)) {
			// ���� ���������� x �� ����
			if (i == 0) {
				float zOfFirstVector = XMVectorGetByIndex(vector, 2);
				orthogonalVector.z = coordinate / (sqrt(coordinate * coordinate + zOfFirstVector * zOfFirstVector));
				orthogonalVector.x = (-orthogonalVector.z * zOfFirstVector) / coordinate;

				return XMVectorSet(orthogonalVector.x, 0.0f, orthogonalVector.z, 0.0f);
			}

			// ���� ���������� y �� ����
			if (i == 1) {
				float zOfFirstVector = XMVectorGetByIndex(vector, 2);
				orthogonalVector.z = coordinate / (sqrt(coordinate * coordinate + zOfFirstVector * zOfFirstVector));
				orthogonalVector.y = (-orthogonalVector.z * zOfFirstVector) / coordinate;

				return XMVectorSet(0.0f, orthogonalVector.y, orthogonalVector.z, 0.0f);
			}

			// ���� ���������� z �� ����
			if (i == 2) {
				float yOfFirstVector = XMVectorGetByIndex(vector, 1);
				orthogonalVector.y = coordinate / (sqrt(coordinate * coordinate + yOfFirstVector * yOfFirstVector));
				orthogonalVector.z = (-orthogonalVector.y * yOfFirstVector) / coordinate;

				return XMVectorSet(0.0f, orthogonalVector.y, orthogonalVector.z, 0.0f);
			}
		}
	}
	// ���� ��� ������� ������
	return XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
};

void RotationAroundAxis(XMVECTOR yAxis, XMVECTOR point, FLOAT angle, XMMATRIX* outputMatrix) {
	yAxis = XMVector3Normalize(yAxis);
	XMVECTOR zAxis = FindOrthogonalNormalizedVector(yAxis);
	XMMATRIX offsetMatrix = XMMatrixTranslationFromVector(_mm_mul_ps(point, XMVectorSet(-1.0f, -1.0f, -1.0f, 0.0f))); // ������� �������� �������, ������� ����� �������������� ������ ���, �� -point
	XMMATRIX backOffsetMatrix = XMMatrixTranslationFromVector(point); // ������� �������� ������������ �������, ������� ��� ��������� ������ ���, �� +point

	// ������� ��������� ������� �� ��������� � ������ ������ � ����������� � ����� ������, ��� �������� ��� �������� ���� Y
	XMMATRIX newCoordinates;
	NewCoordinateSystemMatrix(g_XMZero, zAxis, yAxis, &newCoordinates); // ������ xAxis, ������� ��������� � ���� ������� ����� ���������������, ��� ��� ������, ����������
																	 // � ���������� ���������� ������������ ��������������� � ������������� �������� ���� ����� ���������������  

	// ��� ��� �������� ��� �������� ���� Y, �� � �������� ����� ����������� ������ ��� Y
	XMMATRIX yAxisRotationMatrix = XMMatrixRotationY(angle);

	// ������� �������� � ������ �����������
	XMMATRIX transformationMatrix = {
		XMVector3Cross(yAxis, zAxis),
		yAxis,
		zAxis,
		g_XMIdentityR3 };

	// ����� ����������� ��� �������, ��� ��� ������������ ������ ������������, � �������� ���� �������. �� �������� � ������� �� ������ ������� ������ ��������� ������������ ������. 
	*outputMatrix = offsetMatrix * newCoordinates * yAxisRotationMatrix * transformationMatrix * backOffsetMatrix; 
}

void InitPageSizeAndAllocGranularityVariables() {
	SYSTEM_INFO systemInfo;
	GetNativeSystemInfo(&systemInfo);
	pageSize = systemInfo.dwPageSize;
	allocationGranularity = systemInfo.dwAllocationGranularity;
}

bool ChangesOfStaticHtBoxesArea() {
	// ���� ������� ������ ��������� ������ ������� ������� ����������� ���������
	if ((currentCameraPos.x >= currentHitBoxArea->lowX) and
		(currentCameraPos.x <= currentHitBoxArea->highX) and
		(currentCameraPos.z >= currentHitBoxArea->lowZ) and
		(currentCameraPos.z <= currentHitBoxArea->highZ))
	{
		return false;
	}
		return true;
};

void DefineCurrentStaticHtBoxesArea() {
	// ��� ��� ��� ����� � �������� ������, �� ���������� �������� ��������� � ����� �� ���� ������(0 � 1) �������� 
	// ��������� ������
	// ���� ������ ��������� ������ ������� ������� ����������� ���������
	if ((currentCameraPos.x >= StaticHitBoxAreaArray[0].lowX) and
		(currentCameraPos.x <= StaticHitBoxAreaArray[0].highX) and
		(currentCameraPos.z >= StaticHitBoxAreaArray[0].lowZ) and
		(currentCameraPos.z <= StaticHitBoxAreaArray[0].highZ)) 
	{
		currentHitBoxArea = &StaticHitBoxAreaArray[0];
	}
	else {
		currentHitBoxArea = &StaticHitBoxAreaArray[1];
	}

	// ���� �� ����� �� ����� ���������� ���������� � ����� ������� ��������� ������
	while (currentHitBoxArea->leftNode != NULL) {
		// ���� ������ ��������� ������ ������� ������� ����������� ���������
		if ((currentCameraPos.x >= currentHitBoxArea->leftNode->lowX) and
			(currentCameraPos.x <= currentHitBoxArea->leftNode->highX) and
			(currentCameraPos.z >= currentHitBoxArea->leftNode->lowZ) and
			(currentCameraPos.z <= currentHitBoxArea->leftNode->highZ)) 
		{
			currentHitBoxArea = currentHitBoxArea->leftNode;
		}
		else {
			currentHitBoxArea = currentHitBoxArea->rightNode;
		}
	}
};

void StaticHitBoxesCollisionDetection() {
	// ������ ������� ������� ������
	sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);

	// ���� ������� ������ ��������� ������ ��������(�.�. ��������� ������������), ������ ����� ������ ���������� �� ������� ��������
	// ������� ��� ������� �� ������� ������� ���������
	for (int i = 0; i < currentHitBoxArea->staticHitBoxesAmount; ++i) { 
		// ������� � ������������ ������� ������� ������
		sseProxyRegister0 = XMVector3Transform(sseProxyRegister0, *(currentHitBoxArea->staticHitBoxesArray[i]->angleMatrixRotation));
		
		// ������ ������ ������ ����� ��������
		sseProxyRegister1 = XMLoadFloat3(&currentHitBoxArea->staticHitBoxesArray[i]->widthHeightLength);
		// ���� 0 <= x <= width && 0 <= y <= height && 0 <= z <= length, ��� xyz ����������� ������� ������� ������, � width height length - ��������
		// �� ���� ���� ������� ������ ������ ��������
		if (XMVector3LessOrEqual(sseProxyRegister0, sseProxyRegister1) and
			XMVector3GreaterOrEqual(sseProxyRegister0, g_XMIdentityR3))
		{
			// ������� ���������� ������� ������ �� ������ ������� ������ ��������
			sseProxyRegister1 = sseProxyRegister1 - sseProxyRegister0;

			// ������� ������� ����� ����������� ������� ������ �� ������ ����� ������ ��������(��� ���������� ������� ������) � 
			// ����������� ������ ������ �� ������� ������ ������ ��������
			sseProxyRegister2 = XMVectorMin(sseProxyRegister0, sseProxyRegister1);

			float minX = XMVectorGetX(sseProxyRegister2);
			float minZ = XMVectorGetZ(sseProxyRegister2);

			// ���� ����������� ���������� �� ������ ��������� �� ��� X
			if (minX < minZ) {
				//���� ����������� ���������� ������� ����������� ����������� �� ����� �� ����� ����� ��������
				if (minX == XMVectorGetX(sseProxyRegister0)) {
					sseProxyRegister0 = XMVectorSetX(sseProxyRegister0, -0.005f); // 0.0005 ��� ���������� �� ������� ������������ ������ �� ����� ��������
				} 
				else { //���� ����������� ���������� ������� ����������� ����������� �� ����� �� ������ ����� ��������
					sseProxyRegister1 = XMVectorSetX(sseProxyRegister1, 0.005f);
					sseProxyRegister2 += sseProxyRegister1;
					sseProxyRegister0 += XMVectorAndInt(sseProxyRegister2, g_XMMaskX);
				}
			}
			else {// ���� ����������� ���������� �� ������ ��������� �� ��� Z
				//���� ����������� ���������� ������� ����������� ����������� �� ����� �� ������ ����� ��������
				if (minZ == XMVectorGetZ(sseProxyRegister0)) {
					sseProxyRegister0 = XMVectorSetZ(sseProxyRegister0, -0.005f);
				}
				else { //���� ����������� ���������� ������� ����������� ����������� �� ����� �� ������� ����� ��������
					sseProxyRegister1 = XMVectorSetZ(sseProxyRegister1, 0.005f);
					sseProxyRegister2 += sseProxyRegister1;
					sseProxyRegister0 += XMVectorAndInt(sseProxyRegister2, g_XMMaskZ);
				}
			}
			// �������� �� �������� �������
			sseProxyRegister0 = XMVector3Transform(sseProxyRegister0, *(currentHitBoxArea->staticHitBoxesArray[i]->invertAngleMatrixRotation));
			// ��������� ���������� ������� � currentCameraPosition
			XMStoreFloat3(&currentCameraPos, sseProxyRegister0);

			//������ view matrix, ��� ����� ��� ��������� ������� ��������� � �����, ��������� �����
			//������� ���������� ����� ������� � ������� ������� ��������� ������
			sseProxyRegister0 = XMVector3Transform(sseProxyRegister0, matricesWVP.mView);
			//��������� ������� ��������� ������ � ����� �����
			matricesWVP.mView.r[3] -= sseProxyRegister0;
		}
	}
};

void InitHitBoxes() {
	// �����������, ��� ������ �������� ������ �������� �������� ������
	// ����� ���-�� �������, ����������� ��� ���� ����� �������� ��� ��������, ����� ��������� ���:

	//���������� ���� �� ������������ �� 4 ����� � ������������� ��������� ������
	// ������� �� ������� ������������� �� 4 �����
	DWORD allocationGranularityAlignModulo = allocationGranularity & (4UL - 1UL);
	// �������������� ����� ����� ����� ���������� ������ ��� ��������
	SIZE_T extraBytes = 0;

	if (allocationGranularityAlignModulo != 0UL) { // ���� ����� ���������� ������ ���������� 
		extraBytes += (4UL - allocationGranularityAlignModulo);
	}
	// p.s. ���� ������� � windows ������ ������������� ��������� ������ ����� virtualalloc ����� ������ 4 ������

	// ������ ������, ��������� ����������
	SIZE_T hitBoxAmount = sizeof(extraBytes +
		STATIC_HIT_BOX_AMOUNT * sizeof(HitBox) +
		STATIC_HIT_BOX_AREA_AMOUNT * sizeof(StaticHitBoxArea) +
		STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_0 * sizeof(HitBox*) +
		STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_1 * sizeof(HitBox*) +
		STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_2 * sizeof(HitBox*) +
		STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_3 * sizeof(HitBox*) +
		DYNAMIC_HIT_BOX_AMOUNT * sizeof(DynamicHitBox));

	// ���������� �������, ����������� ��� ���� ����� �������� ��� ��������
	pageAmount = hitBoxAmount & (0xFFFFFFFF ^ (pageSize - 1)); // ������ ����� �������� ������ ����� ����� �� ������� hitBoxAmount �� pageSize
	//SIZE_T moduloPart = hitBoxAmount & (pageSize - 1);
	//SIZE_T integerPart = hitBoxAmount & (0xFFFFFFFF ^ (pageSize - 1));

	// ���� ������� �� ������� hitBoxAmount �� pageSize �� ����
	if ((hitBoxAmount & (pageSize - 1UL)) != 0UL) {
		++pageAmount;
	}
	// p.s. ������� ����� � �������� ���� �� ��������� ���������� ��������� ������� ������ � ������� div � mod

	// ��������� ������ ��� ��������
	// ����� ������ � ������ ������ ��� ������������
	dynamicMemory = VirtualAlloc(NULL, pageAmount * pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// ��������� �� ������ ����������� ���������, ����������� �� ������� 4 �����
	StaticHitBoxArray = (HitBox*)((BYTE*)dynamicMemory + extraBytes);
	// ��������� �� ������ StaticHitBoxArea ��������
	StaticHitBoxAreaArray = (StaticHitBoxArea*)(StaticHitBoxArray + STATIC_HIT_BOX_AMOUNT);
	// ��������� �� ������� ������� � StaticHitBoxDescArray, ������� ���������� ����� ����������� �������� ���� � ������� staticHitBoxArea
	// 0 ����
	Leaf0Array = (HitBox**)(StaticHitBoxAreaArray + STATIC_HIT_BOX_AREA_AMOUNT);
	// 1 ����
	Leaf1Array = Leaf0Array + STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_0;
	// 2 ����
	Leaf2Array = Leaf1Array + STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_1;
	// 3 ����
	Leaf3Array = Leaf2Array + STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_2;

	// ������ ������ � ������� ����������� ���������
	// 0 �������
	StaticHitBoxArray[0] = {
		0.0f,
		55.0f,
		{100.0f, 0.0f, 5.0f},
		&staticHitBoxesRotationMatricesArray[0],
		&invertStaticHitBoxesRotationMatricesArray[0]
	};
	// 1 �������
	StaticHitBoxArray[1] = {
		55.0f,
		0.0f,
		{5.0f, 0.0f, 100.0f},
		&staticHitBoxesRotationMatricesArray[1],
		&invertStaticHitBoxesRotationMatricesArray[1]
	};
	// 2 �������
	StaticHitBoxArray[2] = {
		0.0f,
		-55.0f,
		{100.0f, 0.0f, 5.0f},
		&staticHitBoxesRotationMatricesArray[2],
		&invertStaticHitBoxesRotationMatricesArray[2]
	};
	// 3 �������
	StaticHitBoxArray[3] = {
		-55.0f,
		0.0f,
		{5.0f, 0.0f, 100.0f},
		&staticHitBoxesRotationMatricesArray[3],
		&invertStaticHitBoxesRotationMatricesArray[3]
	};

	// ��������� ������� ������� ��� ����������� ���������
	// ������ 0 �����
	Leaf0Array[0] = &StaticHitBoxArray[0];
	Leaf0Array[1] = &StaticHitBoxArray[3];
	// ������ 1 �����
	Leaf1Array[0] = &StaticHitBoxArray[0];
	Leaf1Array[1] = &StaticHitBoxArray[1];
	// ������ 2 �����
	Leaf2Array[0] = &StaticHitBoxArray[2];
	Leaf2Array[1] = &StaticHitBoxArray[3];
	// ������ 3 �����
	Leaf3Array[0] = &StaticHitBoxArray[1];
	Leaf3Array[1] = &StaticHitBoxArray[2];

	// ������ ������ � ������ �������� ����������� ���������
	// 0 �������
	StaticHitBoxAreaArray[0] = {
		&StaticHitBoxAreaArray[2],
		&StaticHitBoxAreaArray[3],
		NULL,
		0.0f, // center
		25.0f,
		-50.0f, //lX hX
		50.0f,
		0.0f, // lZ hZ
		50.0f,
		2,
		0
	};
	// 1 �������
	StaticHitBoxAreaArray[1] = {
		&StaticHitBoxAreaArray[4],
		&StaticHitBoxAreaArray[5],
		NULL,
		0.0f, // center
		-25.0f,
		-50.0f, //lX hX
		50.0f,
		-50.0f, // lZ hZ
		0.0f,
		2,
		1
	};
	// 2 �������
	StaticHitBoxAreaArray[2] = {
		NULL,
		(StaticHitBoxArea*)Leaf0Array,
		&StaticHitBoxAreaArray[0],
		-25.0f, // center
		25.0f,
		-50.0f, //lX hX
		0.0f,
		0.0f, // lZ hZ
		50.0f,
		2,
		0
	};
	// 3 �������
	StaticHitBoxAreaArray[3] = {
		NULL,
		(StaticHitBoxArea*)Leaf1Array,
		&StaticHitBoxAreaArray[0],
		25.0f, // center
		25.0f,
		0.0f, //lX hX
		50.0f,
		0.0f, // lZ hZ
		50.0f,
		2,
		1
	};
	// 4 �������
	StaticHitBoxAreaArray[4] = {
		NULL,
		(StaticHitBoxArea*)Leaf2Array,
		&StaticHitBoxAreaArray[1],
		-25.0f, // center
		-25.0f,
		-50.0f, //lX hX
		0.0f,
		-50.0f, // lZ hZ
		0.0f,
		2,
		0
	};
	// 5 �������
	StaticHitBoxAreaArray[5] = {
		NULL,
		(StaticHitBoxArea*)Leaf3Array,
		&StaticHitBoxAreaArray[1],
		25.0f, // center
		-25.0f,
		0.0f, //lX hX
		50.0f,
		-50.0f, // lZ hZ
		0.0f,
		2,
		1
	};
    
	// ��������� �� ������ ����������� ���������
    DynamicHitBoxesArray = (DynamicHitBox*)(Leaf3Array + STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_3);
    
	//0 ����������� ������
    DynamicHitBoxesArray[0] = {
        3.0f,
        {-25.0f, 0.0f, 25.0f},
        {2.0f, 0.0f, 2.0f},
        0.0f,
		&DynamicHitBoxesMoveVectorsArray[0],
		0.0f,
		{-25.0f, 0.0f, 25.0f}
    };
	//1 ����������� ������
     DynamicHitBoxesArray[1] = {
        3.0f,
        {25.0f, 0.0f, 25.0f},
        {2.0f, 0.0f, 2.0f},
        0.0f,
		&DynamicHitBoxesMoveVectorsArray[1],
		0.0f,
		{25.0f, 0.0f, 25.0f}
    };
	 //2 ����������� ������
     DynamicHitBoxesArray[2] = {
        3.0f,
        {0.0f, 0.0f, -25.0f},
        {2.0f, 0.0f, 2.0f},
        0.0f,
		&DynamicHitBoxesMoveVectorsArray[2],
		0.0f,
		{0.0f, 0.0f, -25.0f}
    };

	 //������������� �������� �������� ������������ ���������
	 InitMoveVectorsAndActiveCyclesAmountForDynamicHitBoxes();
};

inline void __vectorcall MoveAndRotationDynamicHitBox(FXMVECTOR moveVector, float rotationAngle, DynamicHitBox* dynamicHitBox) {
	// ���������� �������
	sseProxyRegister0 = XMLoadFloat3(&(dynamicHitBox->position));
	sseProxyRegister0 += moveVector;
	XMStoreFloat3(&(dynamicHitBox->position), sseProxyRegister0);

	// �������� ����
	if(dynamicHitBox->angle >= XM_2PI){
		dynamicHitBox->angle += -XM_2PI + rotationAngle;
	}
	dynamicHitBox->angle += rotationAngle;
};

void DynamicHitBoxesCollisionDetection() {
	// �������� ������� ������
	sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);

	// ������� ��� ������������ ��������
	for (int i = 0; i < DYNAMIC_HIT_BOX_AMOUNT; ++i) {
		// ������� ��������
		sseProxyRegister4 = XMLoadFloat3(&DynamicHitBoxesArray[i].position);
		// ���������� ������ ������������ ������� ��������� � ������ ��������
		sseProxyRegister1 = sseProxyRegister0 - sseProxyRegister4;
		// ������������� ������� ������� ������������� ��������
		sseProxyRegister2 = XMVectorSet(DynamicHitBoxesArray[i].halfOfShellEdge, 0.0f, DynamicHitBoxesArray[i].halfOfShellEdge, 0.0f);
		// ������������� ������� ������� ��������
		sseProxyRegister3 = XMVectorNegate(sseProxyRegister2);

		// ���� ������ ��������� ������ ������� ��������
		if (XMVector3LessOrEqual(sseProxyRegister1, sseProxyRegister2) and
			XMVector3GreaterOrEqual(sseProxyRegister1, sseProxyRegister3)) 
		{
			// ������� ������� ������
			sseProxyRegister0_Matrix = XMMatrixRotationY(-DynamicHitBoxesArray[i].angle);
			sseProxyRegister1 = XMVector3Transform(sseProxyRegister1, sseProxyRegister0_Matrix);
			// ���������� ������� ������ ������������ ������� ���������, ����������� � ����� ������ ���� ��������
			sseProxyRegister2 = XMLoadFloat3(&DynamicHitBoxesArray[i].halvesOfWidthHeightLength);
			sseProxyRegister1 += sseProxyRegister2;
			// ������ ������ ������ ����� ��������
			sseProxyRegister2 *= 2.0f;

			// ���� ������ ��������� � ��������
			if (XMVector3LessOrEqual(sseProxyRegister1, sseProxyRegister2) and
				XMVector3GreaterOrEqual(sseProxyRegister1, g_XMIdentityR3))
			{
				// ������� ���������� ������� ������ �� ������ ������� ������ ��������
				sseProxyRegister2 = sseProxyRegister2 - sseProxyRegister1;

				// ������� ������� ����� ����������� ������� ������ �� ������ ����� ������ ��������(��� ���������� ������� ������) � 
				// ����������� ������ ������ �� ������� ������ ������ ��������
				sseProxyRegister3 = XMVectorMin(sseProxyRegister1, sseProxyRegister2);

				float minX = XMVectorGetX(sseProxyRegister3);
				float minZ = XMVectorGetZ(sseProxyRegister3);

				// ���� ����������� ���������� �� ������ ��������� �� ��� X
				if (minX < minZ) {
					//���� ����������� ���������� ������� ����������� ����������� �� ����� �� ����� ����� ��������
					if (minX == XMVectorGetX(sseProxyRegister1)) {
						sseProxyRegister1 = XMVectorSetX(sseProxyRegister1, -0.05f); // 0.0005 ��� ���������� �� ������� ������������ ������ �� ����� ��������
					}
					else { //���� ����������� ���������� ������� ����������� ����������� �� ����� �� ������ ����� ��������
						sseProxyRegister2 = XMVectorSetX(sseProxyRegister2, 0.05f);
						sseProxyRegister3 += sseProxyRegister2;
						sseProxyRegister1 += XMVectorAndInt(sseProxyRegister3, g_XMMaskX);
					}
				}
				else {// ���� ����������� ���������� �� ������ ��������� �� ��� Z
					//���� ����������� ���������� ������� ����������� ����������� �� ����� �� ������ ����� ��������
					if (minZ == XMVectorGetZ(sseProxyRegister1)) {
						sseProxyRegister1 = XMVectorSetZ(sseProxyRegister1, -0.05f);
					}
					else { //���� ����������� ���������� ������� ����������� ����������� �� ����� �� ������� ����� ��������
						sseProxyRegister2 = XMVectorSetZ(sseProxyRegister2, 0.05f);
						sseProxyRegister3 += sseProxyRegister2;
						sseProxyRegister1 += XMVectorAndInt(sseProxyRegister3, g_XMMaskZ);
					}
				}
				// �������� �� �������� �������
				sseProxyRegister2 = XMLoadFloat3(&DynamicHitBoxesArray[i].halvesOfWidthHeightLength);
				sseProxyRegister1 -= sseProxyRegister2;
				sseProxyRegister0_Matrix = XMMatrixRotationY(DynamicHitBoxesArray[i].angle);
				sseProxyRegister1 = XMVector3Transform(sseProxyRegister1, sseProxyRegister0_Matrix);
				sseProxyRegister2 = XMLoadFloat3(&DynamicHitBoxesArray[i].position);
				sseProxyRegister1 += sseProxyRegister2;
				// ��������� ���������� ������� � currentCameraPosition
				XMStoreFloat3(&currentCameraPos, sseProxyRegister1);

				//������ view matrix, ��� ����� ��� ��������� ������� ��������� � �����, ��������� �����
				//������� ���������� ����� ������� � ������� ������� ��������� ������
				matricesWVP.mView = XMMatrixTranspose(matricesWVP.mView);
				sseProxyRegister1 = XMVector3Transform(sseProxyRegister1, matricesWVP.mView);
				sseProxyRegister1 = XMVectorSetW(sseProxyRegister1, 0.0f);
				//��������� ������� ��������� ������ � ����� �����
				matricesWVP.mView.r[3] -= sseProxyRegister1;
				matricesWVP.mView = XMMatrixTranspose(matricesWVP.mView);
			}
		}
	}
};

float DynamicHitBoxCyclesAmount(DynamicHitBox* hitBox) {
	// ������� �������� ������������ ������� ���������, ����������� � ����� ������ ���� �����
	sseProxyRegister0 = XMLoadFloat3(&hitBox->position);
	sseProxyRegister0 += halvesOfWidthHeightLengthOfMap;
	
	// ������ ����� ���������� ������� �������� �������������, � ����� �������������
	sseProxyRegister1 = XMVectorAndInt(*hitBox->moveVectorPtr, g_XMNegate3);

	float xValue = XMVectorGetX(sseProxyRegister1);
	float zValue = XMVectorGetZ(sseProxyRegister1);

	// ���� X < 0 ��� X == -0
	if (*((UINT*) &xValue) == 0x80000000U) { // ������ ��������... �� � ��� �� �������, union ��� reinterpret_cast ��?
		sseProxyRegister2 = XMVectorSetX(sseProxyRegister2, 0.0f);
	}
	else {
		// ���� X > 0 ��� X == +0
		sseProxyRegister2 = XMVectorSetX(sseProxyRegister2, 100.0f); // 100 - ������ �����
	}
	
	// ���� Z < 0 ��� Z == -0
	if (*((UINT*) &zValue) == 0x80000000U) {
		sseProxyRegister2 = XMVectorSetZ(sseProxyRegister2, 0.0f);
	}
	else {
		// ���� Z > 0 ��� Z == +0
		sseProxyRegister2 = XMVectorSetZ(sseProxyRegister2, 100.0f); // 100 - ����� �����
	}

	// ���������� moveVector-�� ����������� ��������� � ������� ��������, ��� ����
	// ����� ����� �������� ����� �� ������� �����
	// ������ ���� ����� �� ���������� moveVector ����� +0 ��� -0, �� �� ������� +inf � sseProxyRegister3 �� ����� ���� ����������
	// ��� �������� ������ ������������ ��� ������ �������� ����� ��������� sseProxyRegister3
	sseProxyRegister3 = (sseProxyRegister2 - sseProxyRegister0) / (*hitBox->moveVectorPtr);

	// ����� �������� ����� ���������
	if (XMVectorGetX(sseProxyRegister3) < XMVectorGetZ(sseProxyRegister3)) {
		return XMVectorGetX(XMVectorCeiling(sseProxyRegister3));
	}
	return XMVectorGetZ(XMVectorCeiling(sseProxyRegister3));
};

void CreateNewMoveVectorForDynamicHitBox(DynamicHitBox* hitBox) {
	// ������ ������� ������� ������
	sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);
	// ������� ��������
	sseProxyRegister1 = XMLoadFloat3(&hitBox->position);

	// ������� ������
	sseProxyRegister0 -= sseProxyRegister1;
	sseProxyRegister0 = XMVector3NormalizeEst(sseProxyRegister0);
	sseProxyRegister0 *= DYNAMIC_HIT_BOX_MOVEVECTOR_LENGTH; 
	*hitBox->moveVectorPtr = sseProxyRegister0;
};

inline void InitMoveVectorsAndActiveCyclesAmountForDynamicHitBoxes() {
	for (int i = 0; i < DYNAMIC_HIT_BOX_AMOUNT; ++i) {
		CreateNewMoveVectorForDynamicHitBox(&DynamicHitBoxesArray[i]);
		DynamicHitBoxesArray[i].activeCyclesAmount = DynamicHitBoxCyclesAmount(&DynamicHitBoxesArray[i]);
	}
};

HRESULT InitWallsVertices(Vertex* wallsVertexArray, LPCWSTR wallsVertexShaderName, LPCWSTR wallsPixelShaderName, LPCSTR vsShaderEntryPoint, LPCSTR psShaderEntryPoint) {
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
	if (FAILED(hr)) {
		return hr;
	}

	UINT stride[] = { sizeof(Vertex) };
	UINT offset[] = { 0 };
	g_pImmediateContext->IASetVertexBuffers(0, 1, &pWallsVertexBuffer, stride, offset);

	// ���������� ��������
	// ����� cso ����� � ����������
	
	
	// �������� Input-Layout Object
	// ���������� Input-layout object � ����������

};

void FindFilesInCurrentDirFromFile(WCHAR* filesArray, size_t* filesNamesLengthArray){
	// ����� cso ����� � ����������
	WCHAR fileName [] = "\TriangleVertexShader*";
	size_t fileNameSize = sizeof(fileName);
	
	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;
	
	// ����� �������� ������ ���� �����
	WCHAR fileDirBuffer [MAX_PATH];
	// ����� ���� ����� � ������ null(��� ����� ����� � ������ null), ��� ����� ����� �������� �����
	DWORD fileDirBufLength = GetCurrentDirectory(MAX_PATH, fileDirBuffer) + 1;
	
	// ����� �����
	FindFilesInCurrentDir(fileDirBuffer, fileDirBufLength, fileName, fileNameLength, &fileData);
};

// ���� ������ ���������� ����, �������� � �������� ���������, � �������� ����������
// fileName ������� ������������ � '\', fileDir ������ ������������� ��������� �������� �������� ��� �����, ��� '\'
HRESULT FindFilesInCurrentDir(WCHAR* fileDirBuffer, size_t fileDirBufLength, WCHAR* fileName, size_t fileNameLength, WIN32_FIND_DATA* fileDataPtr){
	// ��������� ������ ���� �� �����
	memcpy(&fileDirBuffer[fileDirBufLength - 1], &fileName[0], fileNameSize);
	
	HANDLE searchHandle = FindFirstFileEx(fileDirBuffer, FindExInfoBasic, fileDataPtr, FindExSearchNameMatch, NULL, 0);
	// ���� ������ ���� �������
	if(searchHandle != INVALID_HANDLE_VALUE){
		// ret fileDirBuffer
		FindClose(searchHandle);
		return S_OK;
	} 
	// ���� ������ ���� �� �������
	else{
		FindClose(searchHandle);
		
		fileDirBuffer[fileDirBufLength + 1] = L'*'; // curDirLength + 1 ������ ��� '*' ����� ��������� ����� '\'
		fileDirBuffer[fileDirBufLength + 2] = NULL;
		fileDirBufLength += 2; 
			
		searchHandle = FindFirstFileEx(fileDirBuffer, FindExInfoBasic, fileDataPtr, FindExSearchNameMatch, NULL, 0);
		// ���� ������� ������� �� ����
		if (searchHandle != INVALID_HANDLE_VALUE) {
		do {
			// ���� �������� ���� - �������
			if(fileDataPtr->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// ��������� ������� ����������
				size_t folderNameLength = FileNameLength(fileDataPtr->cFileName);
				memcpy(&fileDirBuffer[fileDirBufLength - 1], fileDataPtr->cFileName[0], folderNameLength);
				
				// ���� � ��������� ���������� ������� ����
				if (FindFilesInCurrentDir(fileDirBuffer, fileDirBufLength + folderNameLength - 2, fileName, fileNameLength) == S_OK){
					FindClose(searchHandle);
					return S_OK;
				}
			}	
		} while (FindNextFile(searchHandle, fileDataPtr) != 0)
		}
		// ���� ���������� ����� ��� ��� �������� � ���������� ���������	
		else{
			FindClose(searchHandle);
			return E_FAIL;
		}
	}
};

// � ������ null
inline size_t __vectorcall FileNameLength(WCHAR* name){
	size_t nameLength = 1;
	if(name[nameLength] != NULL){
		nameLength++;
	} 
	nameLength++;
	
	return nameLength;
};

void ReleaseObjects() {
	if (dynamicMemory != NULL) {
		VirtualFree(dynamicMemory, 0, MEM_RELEASE);
	}
	if (pIndexBuffer != NULL) {
		pIndexBuffer->Release();
		pIndexBuffer = NULL;
	}
	if (pAngleBufferVSResource != NULL) {
		pAngleBufferVSResource->Release();
		pAngleBufferVSResource = NULL;
	}
	if(pAngleBuffer != NULL) {
		pAngleBuffer->Release();
		pAngleBuffer = NULL;
	}
	for (int i = 0; i < 2; ++i) {
		if (constantBufferArray[i] != NULL) {
			constantBufferArray[i]->Release();
			constantBufferArray[i] = NULL;
		}
	}
	if (g_pInputLayoutObject != NULL) {
		g_pInputLayoutObject->Release();
		g_pInputLayoutObject = NULL;
	}
	if (g_pPixelShader != NULL) {
		g_pPixelShader->Release();
		g_pPixelShader = NULL;
	}
	if (g_pVertexShader != NULL) {
		g_pVertexShader->Release();
		g_pVertexShader = NULL;
	}
	if (PS_Buffer != NULL) {
		PS_Buffer->Release();
		PS_Buffer = NULL;
	}
	if (VS_Buffer != NULL) {
		VS_Buffer->Release();
		VS_Buffer = NULL;
	}
	if (pVertexBuffer != NULL) {
		pVertexBuffer->Release();
		pVertexBuffer = NULL;
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








