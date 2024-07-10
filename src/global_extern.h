#define _GLOBAL_EXTERN_H

// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ

extern HINSTANCE               g_hInst; //указатель на struct, дескриптор(handle) данного приложения.
extern HWND                    g_hWnd; //указатель на struct, дескриптор(handle) окна данного приложения.
extern LONG StartUpWndProcPtr; // указатель на дефолтную оконную процедуру

extern D3D_DRIVER_TYPE         g_driverType; //целочисленная константа. Переменная, обозначающая Тип драйвера, определяет, где производить вычисления.
extern D3D_FEATURE_LEVEL       g_featureLevel; //используемый feature level, точнее, это переменная, которая будет хранить самый высокий feature level, доступный на заданном адаптере.
extern ID3D11Device* g_pd3dDevice; //указатель на struct(Объект Интерфейса ID3D11Device). ID3D11Device это COM-интерфейс, который создает ресурсы(текстуры, трехмерные объекты и т.д.) для вывода на дисплей.
extern ID3D11DeviceContext* g_pImmediateContext; //указатель на struct(Объект Интерфейса ID3D11DeviceContext). ID3D11DeviceContext это COM-интерфейс, который занимается отрисовкой графической информации на Дисплей.
extern IDXGISwapChain* g_pSwapChain; //указатель на struct(Объект Интерфейса IDXGISwapChain). IDXGISwapChain это COM-интерфейс, который хранит в нескольких буферах несколько отрисованых Поверхностей перед их выводом на Дисплей.
extern ID3D11RenderTargetView* g_pRenderTargetView; //указатель на struct(Объект Интерфейса ID3D11RenderTargetView). ID3D11RenderTargetView это COM-интерфейс, который хранит ресурсы back buffer-а. 
extern ShaderModelDesc shadersModel; // модель шейдеров
extern ID3D11InputLayout* g_pInputLayoutObjectPyramid; // указатель на input layout object вершин пирамиды
extern ID3D11InputLayout* g_pInputLayoutObjectWalls; // указатель на ILO врешин стен
extern ID3D11VertexShader* g_pVertexShader; // указатель на интерфейс vertex shader
extern ID3D11PixelShader* g_pPixelShader; // указатель на интерфейс pixel shader
extern ID3D11VertexShader* vertexShadersObj[]; // массив объектов вершинных шейдеров
extern ID3D11PixelShader* pixelShadersObj[]; // массив объектов пиксельных шейдеров
extern ID3DBlob* VS_Buffer; // указатель на интерфейс буфера с скомпилированным вершинным шейдером 
extern ID3DBlob* PS_Buffer; // указатель на интерфейс буфера с скомпилированным пиксельным шейдером 
extern ID3DBlob** shadersBufferArray; // массив указатели на скомпилированные шейдеры (порядок шейдеров в массиве как в файле со списком шейдеров)
extern ID3D11ShaderReflection*  shaderReflect; // информация о шейдере
extern ID3D11Buffer* pPyramidVertexBuffer; // указатель на буфер вершин пирамиды
extern ID3D11Buffer* pWallsVertexBuffer; // указатель на буфер вершин стен
extern ID3D11Buffer* pConstantBuffer; // констнантный буфер
extern ID3D11Buffer* pIndexBuffer; // буфер индексов пирамиды
extern ID3D11Buffer* pWallsIndexBuffer; // буфер индексов пирамиды
extern ID3D11Buffer* pAngleBuffer; // буфер угла 
extern ID3D11Buffer* constantBufferArray[]; // массив указателей на интерфейсы константных буферов
extern AngleConstantBuffer angleCBufferData; // угол поворота
extern ID3D11ShaderResourceView* pAngleBufferVSResource; // ресурс вершинного шейдера, к оторм находится угол
extern ID3D11Texture2D* depthStencilTexture; // текстура depth буфера
extern ID3D11DepthStencilView* g_pDepthStencilView; // ресурсы depth буфера
extern ID3D11DepthStencilState* pDSState; // состояние depth-stencil теста
extern ID3D11RasterizerState* pRasterizerState; // состояние растеризатора 

extern MatricesBuffer matricesWVP; // матрицы
extern XMVECTOR objectsPositions[]; //массив точек, в которых располагаются объекты
extern XMMATRIX moveAheadMatrix; // матрица движения вперед
extern XMVECTOR moveAheadVector; // вектор движения в положительном направлении оси(в системе координат камеры) 
extern XMVECTOR moveAheadVectorInGlobalCoord; // вектор движения в положительном направлении оси(в глобальной системе координат)
extern XMVECTOR moveBackVector; // вектор движения в отрицательном направлении оси
extern XMVECTOR moveRightVector; 
extern XMVECTOR moveRightVectorInGlobalCoord; 
extern XMVECTOR moveLeftVector;
 
extern DWORD pageSize; // размер страницы виртуальной памяти
extern DWORD allocationGranularity; // выравнивание адресов в виртуальной памяти
extern WCHAR* shadersListBuf;

// переменные, относящиеся к хитбоксам
extern XMVECTOR halvesOfWidthHeightLengthOfMap; // половины значений глобальной карты
extern LPVOID dynamicMemory; // сырая память с учетом байтов для выравнивания
extern SIZE_T pageAmount; // количество страниц, необходимое для того чтобы вместить все хитбоксы
extern HitBox* StaticHitBoxArray; // указатель на массив статических хитбоксов, выровненный по границе 4 байта
extern StaticHitBoxArea* StaticHitBoxAreaArray; // указатель на массив StaticHitBoxArea структур
// указатели на массивы позиций в StaticHitBoxDescArray, которые определяют какие статические хитбоксы есть в листьях staticHitBoxArea
extern HitBox** Leaf0Array; // 0 лист
extern HitBox** Leaf1Array; // 1 лист
extern HitBox** Leaf2Array; // 2 лист
extern HitBox** Leaf3Array; // 3 лист
extern StaticHitBoxArea* currentHitBoxArea; // указатель на hitboxarea, в которой находится камера в данный момент
extern XMFLOAT3 currentCameraPos; // текущая позиция камеры в координатах x,z
extern XMFLOAT3 previousCameraPos; // позиция камеры на предыдущем кадре
// массив матриц поворота статических хитбоксов
// чтобы выяснить попадает ли позиция камеры внутрь хитбокса, нам нужно сместить центр хитбокса так, чтобы ширина хитбокса совпадала с осью X,
// а длина хитбокса совпадала с осью Z
// первая матрица - смещаем центр координат в центр хибокса
// вторая матрица - поворачиваем точку и хитбокс так, чтобы ширина хитбокса была || оси X , а длинна хитбокса || оси Z
// третья матрица - смещаем центр координат в левый нижний угол хитбокса
extern const XMMATRIX staticHitBoxesRotationMatricesArray[4]; 
// массив обратных матриц к матрицам из staticHitBoxesRotationMatricesArray
extern const XMMATRIX invertStaticHitBoxesRotationMatricesArray[4];
// массив динамических хитбоксов
extern DynamicHitBox* DynamicHitBoxesArray;
// массив векторов движения динамических хитбоксов
extern XMVECTOR DynamicHitBoxesMoveVectorsArray[3];
// переменная, куда кладем произвольные переменные типа XMFLOAT4
extern XMVECTOR sseProxyRegister0;
extern XMVECTOR sseProxyRegister1;
extern XMVECTOR sseProxyRegister2;
extern XMVECTOR sseProxyRegister3;
extern XMVECTOR sseProxyRegister4;
extern XMMATRIX sseProxyRegister0_Matrix;
// переменная, куда кладем произвольные переменные типа XMVECTOR
extern XMFLOAT3 xmfloat4Storage0;