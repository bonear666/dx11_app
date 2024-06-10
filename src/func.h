//ПРЕДВАРИТЕЛЬНЫЕ ОБЪЯВЛЕНИЯ ФУНКЦИЙ

// Функция окна
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 
// Создание DirectX компонентов
HRESULT CreateDirect3DComponents(UINT widthParam, UINT heightParam);
// Создание окна
HRESULT MyCreateWindow(CONST WCHAR* wndClassNameParam, CONST WCHAR* wndNameParam, int widthParam, int heightParam, HINSTANCE hInstanceParam, int nShowCmdParam);
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
// Создание константного буфера матриц, константного буфера угла, буфера вершин
HRESULT InitMatrices(WORD* indices);   
// Обновление матриц
void SetMatrices();
// Обновление проекционной матрицы (0 < angle < PI/2). angle = fov/2
// nearZ, farZ координаты плоскости отсечений, деленные на W
// nearZ не должен быть равен нулю, так как исходя из формулы, Z координата вершины после умножения на матрицу проекции будет всегда равна 1
void SetProjectionMatrix(MatricesBuffer* pMatricesBuffer, FLOAT angleHoriz, FLOAT angleVert, FLOAT nearZ, FLOAT farZ, BOOL saveProportionsFlag);
// Сохранение пропорций объектов, при выводе в окно. Пропорции сохраняются в соответствии с осью, у которой меньше единичный отрезок. 
void SaveProportions(MatricesBuffer* pMatricesBuffer, HWND hWnd);
// Наибольший элемент
FLOAT MaxElement(FLOAT arg0, FLOAT arg1);
// Создание обратной матрицы
HRESULT InvertMatrix(XMVECTOR zAxis, XMVECTOR yAxis, XMMATRIX* invertibleMatrix);
// Создание матрицы перехода к другой системе координат
HRESULT NewCoordinateSystemMatrix(XMVECTOR point, XMVECTOR zAxis, XMVECTOR yAxis, XMMATRIX* invertibleMatrix);
// Создание матрицы мира
void SetWorldMatrix(XMVECTOR point, XMVECTOR scale, XMMATRIX* worldMatrix);
// изменение порядка обхода вершин
void InvertIndices(WORD* indicesArray, int size);
// поиск ортогонального вектора к заданному
XMVECTOR FindOrthogonalVector(XMVECTOR vector);
// поиск ортгонального нормализованного вектора к заданному
XMVECTOR FindOrthogonalNormalizedVector(XMVECTOR vector);
// матрица вращения вершины вокруг произвольного вектора
void RotationAroundAxis(XMVECTOR yAxis, XMVECTOR point, FLOAT angle, XMMATRIX* outputMatrix);
// Обновление проекционной матрицы, с учетом расстояния от камеры до ближней плоскости отсечения (0 < angle < PI/2). angle = fov/2
// camera Distance <= nearZ
// используя эту функцию можно визуально опознать ближнюю плоскость отсечения
// cameraDistance - значение расстояния от камеры до ближней плоскости отсечения, задается деленным на W координату
// nearZ, farZ координаты плоскости отсечений, деленные на W
// cameraDistance не должен быть равен нулю, так как исходя из формулы, Z координата вершины после умножения на матрицу проекции будет всегда равна 1
void SetProjectionMatrixWithCameraDistance(MatricesBuffer* pMatricesBuffer, FLOAT angleHoriz, FLOAT angleVert, FLOAT nearZ, FLOAT farZ, FLOAT cameraDistance, BOOL saveProportionsFlag);
// инициализация глобальных статических переменных с размером страницы виртуальной памяти, и переменной с гранулярностью памяти
void InitPageSizeAndAllocGranularityVariables(); 
// проверка на изменение области статических хитбоксов,т.е. изменилась ли позиция камеры относительно текущей области статических хитбоксов
bool ChangesOfStaticHtBoxesArea();
// определенние текущей области статических хитбоксов, в которой находится камера в данный момент
void DefineCurrentStaticHtBoxesArea();
// проверка на столкновение камеры со статическими хитбоксами в текущей области статических хитбоксов
void StaticHitBoxesCollisionDetection();
// инициализация хитбоксов
void InitHitBoxes();
// смещение и поворот динмаического хитбокса
inline void __vectorcall MoveAndRotationDynamicHitBox(FXMVECTOR moveVector, float rotationAngle, DynamicHitBox* dynamicHitBox);
// проверка на столкновение камеры с динмаическими хитбоксами
void DynamicHitBoxesCollisionDetection();
// через сколько циклов центр динамического хитбокса выйдет за пределы карты
float DynamicHitBoxCyclesAmount(DynamicHitBox* hitBox);
// создание новго вектора движения динмаического хитбокса
void CreateNewMoveVectorForDynamicHitBox(DynamicHitBox* hitBox);
// начальная инициализация векторов движения динамических хитбоксов
inline void InitMoveVectorsAndActiveCyclesAmountForDynamicHitBoxes();
// оконная процедура, которая обрабатывает все сообщения по дефолту
LRESULT CALLBACK StartUpWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// 
HRESULT InitWallsVertices(Vertex* wallsVertexArray, LPCWSTR wallsVertexShaderName, LPCWSTR wallsPixelShaderName, LPCSTR vsShaderEntryPoint, LPCSTR psShaderEntryPoint);