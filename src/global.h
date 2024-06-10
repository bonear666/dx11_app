// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ

HINSTANCE               g_hInst = NULL; //указатель на struct, дескриптор(handle) данного приложения.
HWND                    g_hWnd = NULL; //указатель на struct, дескриптор(handle) окна данного приложения.
LONG StartUpWndProcPtr = NULL; // указатель на дефолтную оконную процедуру
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL; //целочисленная константа. Переменная, обозначающая Тип драйвера, определяет, где производить вычисления.
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_9_1; //используемый feature level, точнее, это переменная, которая будет хранить самый высокий feature level, доступный на заданном адаптере.
ID3D11Device* g_pd3dDevice = NULL; //указатель на struct(Объект Интерфейса ID3D11Device). ID3D11Device это COM-интерфейс, который создает ресурсы(текстуры, трехмерные объекты и т.д.) для вывода на дисплей.
ID3D11DeviceContext* g_pImmediateContext = NULL; //указатель на struct(Объект Интерфейса ID3D11DeviceContext). ID3D11DeviceContext это COM-интерфейс, который занимается отрисовкой графической информации на Дисплей.
IDXGISwapChain* g_pSwapChain = NULL; //указатель на struct(Объект Интерфейса IDXGISwapChain). IDXGISwapChain это COM-интерфейс, который хранит в нескольких буферах несколько отрисованых Поверхностей перед их выводом на Дисплей.
ID3D11RenderTargetView* g_pRenderTargetView = NULL; //указатель на struct(Объект Интерфейса ID3D11RenderTargetView). ID3D11RenderTargetView это COM-интерфейс, который хранит ресурсы back buffer-а. 
ShaderModelDesc shadersModel;
ID3D11InputLayout* g_pInputLayoutObject = NULL; // указатель на input layout object
ID3D11VertexShader* g_pVertexShader = NULL; // указатель на интерфейс vertex shader
ID3D11PixelShader* g_pPixelShader = NULL; // указатель на интерфейс pixel shader
ID3DBlob* VS_Buffer = NULL; // указатель на интерфейс буфера с скомпилированным вершинным шейдером 
ID3DBlob* PS_Buffer = NULL; // указатель на интерфейс буфера с скомпилированным пиксельным шейдером 
ID3D11Buffer* pVertexBuffer = NULL; // указатель на буфер вершин пирамиды
ID3D11Buffer* pWallsVertexBuffer = NULL; // указатель на буфер вершин стен
ID3D11Buffer* pConstantBuffer = NULL; // констнантный буфер
ID3D11Buffer* pIndexBuffer = NULL; // буфер индексов
ID3D11Buffer* pAngleBuffer = NULL; // буфер угла 
ID3D11Buffer* constantBufferArray[] = {NULL, NULL, NULL}; // массив указателей на интерфейсы константных буферов
AngleConstantBuffer angleCBufferData = { 0.0f, 0.0f, 0.0f, 0.0f }; // угол поворота
ID3D11ShaderResourceView* pAngleBufferVSResource = NULL; // ресурс вершинного шейдера, к оторм находится угол
ID3D11Texture2D* depthStencilTexture = NULL; // текстура depth буфера
ID3D11DepthStencilView* g_pDepthStencilView = NULL; // ресурсы depth буфера
ID3D11DepthStencilState* pDSState = NULL; // состояние depth-stencil теста
ID3D11RasterizerState* pRasterizerState = NULL; // состояние растеризатора 
MatricesBuffer matricesWVP; // матрицы
XMVECTOR objectsPositions[] = { //массив точек, в которых располагаются объекты
	XMVECTORF32{-25.0f, 0.0f, 25.0f, 0.0f}, // {-25.0f, 0.0f, 25.0f, 0.0f}
	XMVECTORF32{25.f, 0.0f, 25.0f, 0.0f},
	XMVECTORF32{0.0f, 0.0f, -25.0f, 0.0f}
};
XMMATRIX moveAheadMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 0.0f, 0.4f)); // матрица движения вперед
XMVECTOR moveAheadVector = XMVectorSet(0.0f, 0.0f, -0.1f, 0.0f); // вектор движения в положительном направлении оси(в системе координат камеры) 
XMVECTOR moveAheadVectorInGlobalCoord = XMVECTORF32{ 0.0f, 0.0f, 0.1f, 0.0f }; // вектор движения в положительном направлении оси(в глобальной системе координат)
XMVECTOR moveBackVector = XMVectorSet(0.0f, 0.0f, 0.1f, 0.0f); // вектор движения в отрицательном направлении оси
XMVECTOR moveRightVector = XMVectorSet(0.0f, 0.0f, 0.0f, -0.1f); 
XMVECTOR moveRightVectorInGlobalCoord = XMVECTORF32{ 0.1f, 0.0f, 0.0f, 0.0f }; 
XMVECTOR moveLeftVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.1f); 
DWORD pageSize; // размер страницы виртуальной памяти
DWORD allocationGranularity; // выравнивание адресов в виртуальной памяти

// переменные, относящиеся к хитбоксам
XMVECTOR halvesOfWidthHeightLengthOfMap = XMVECTORF32{50.0f, 0.0f, 50.0f, 0.0f}; // половины значений глобальной карты
LPVOID dynamicMemory; // сырая память с учетом байтов для выравнивания
SIZE_T pageAmount; // количество страниц, необходимое для того чтобы вместить все хитбоксы
HitBox* StaticHitBoxArray; // указатель на массив статических хитбоксов, выровненный по границе 4 байта
StaticHitBoxArea* StaticHitBoxAreaArray; // указатель на массив StaticHitBoxArea структур
// указатели на массивы позиций в StaticHitBoxDescArray, которые определяют какие статические хитбоксы есть в листьях staticHitBoxArea
HitBox** Leaf0Array; // 0 лист
HitBox** Leaf1Array; // 1 лист
HitBox** Leaf2Array; // 2 лист
HitBox** Leaf3Array; // 3 лист
StaticHitBoxArea* currentHitBoxArea; // указатель на hitboxarea, в которой находится камера в данный момент
XMFLOAT3 currentCameraPos = {0.0f, 0.0f ,0.0f}; // текущая позиция камеры в координатах x,z
XMFLOAT3 previousCameraPos; // позиция камеры на предыдущем кадре
// массив матриц поворота статических хитбоксов
// чтобы выяснить попадает ли позиция камеры внутрь хитбокса, нам нужно сместить центр хитбокса так, чтобы ширина хитбокса совпадала с осью X,
// а длина хитбокса совпадала с осью Z
// первая матрица - смещаем центр координат в центр хибокса
// вторая матрица - поворачиваем точку и хитбокс так, чтобы ширина хитбокса была || оси X , а длинна хитбокса || оси Z
// третья матрица - смещаем центр координат в левый нижний угол хитбокса
const XMMATRIX staticHitBoxesRotationMatricesArray[4] = { // аргументы первой функции - отрицательное значение позиции хитбокса
	// аргументы второй функции - отрицательное значение угла поворота хитбокса, относительно системы координат, проходящей через центр хитбокса
	XMMatrixTranslation(0.0f, 0.0f, -55.0f) * XMMatrixRotationY(0.0f) * XMMatrixTranslation(50.0f, 0.0f, 2.5f), // матрица поворота для 0 хитбокса
	XMMatrixTranslation(-55.0f, 0.0f, 0.0f) * XMMatrixRotationY(0.0f) * XMMatrixTranslation(2.5f, 0.0f, 50.0f), // матрица поворота для 1 хитбокса
	XMMatrixTranslation(0.0f, 0.0f, 55.0f) * XMMatrixRotationY(0.0f) * XMMatrixTranslation(50.0f, 0.0f, 2.5f), // матрица поворота для 2 хитбокса
	XMMatrixTranslation(55.0f, 0.0f, 0.0f) * XMMatrixRotationY(0.0f) * XMMatrixTranslation(2.5f, 0.0f, 50.0f) // матрица поворота для 3 хитбокса
	// было бы правильнее сразу посчитать произведение этих матриц и записать его в таком виде:
	//XMMATRIX{XMVECTORF32{0.0f, 0.0f, 0.0f, 0.0f},
	//XMVECTORF32{0.0f, 0.0f, 0.0f, 0.0f},
	//XMVECTORF32{0.0f, 0.0f, 0.0f, 0.0f},
	//XMVECTORF32{0.0f, 0.0f, 0.0f, 0.0f}}
}; 
// массив обратных матриц к матрицам из staticHitBoxesRotationMatricesArray
const XMMATRIX invertStaticHitBoxesRotationMatricesArray[4] = {
	XMMatrixTranslation(-50.0f, 0.0f, - 2.5f) * XMMatrixRotationY(0.0f) * XMMatrixTranslation(0.0f, 0.0f, 55.0f), // матрица поворота для 0 хитбокса
	XMMatrixTranslation(-2.5f, 0.0f, -50.0f)* XMMatrixRotationY(0.0f)* XMMatrixTranslation(55.0f, 0.0f, 0.0f), // матрица поворота для 1 хитбокса
	XMMatrixTranslation(-50.0f, 0.0f, -2.5f)* XMMatrixRotationY(0.0f)* XMMatrixTranslation(0.0f, 0.0f, -55.0f), // матрица поворота для 2 хитбокса
	XMMatrixTranslation(-2.5f, 0.0f, -50.0f)* XMMatrixRotationY(0.0f)* XMMatrixTranslation(-55.0f, 0.0f, 0.0f), // матрица поворота для 3 хитбокса
};
// массив динамических хитбоксов
DynamicHitBox* DynamicHitBoxesArray;
// массив векторов движения динамических хитбоксов
XMVECTOR DynamicHitBoxesMoveVectorsArray[3];
// переменная, куда кладем произвольные переменные типа XMFLOAT4
XMVECTOR sseProxyRegister0;
XMVECTOR sseProxyRegister1;
XMVECTOR sseProxyRegister2;
XMVECTOR sseProxyRegister3;
XMVECTOR sseProxyRegister4;
XMMATRIX sseProxyRegister0_Matrix;
// переменная, куда кладем произвольные переменные типа XMVECTOR
XMFLOAT3 xmfloat4Storage0;