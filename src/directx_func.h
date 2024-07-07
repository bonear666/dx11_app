#define _DIRECTX_FUNC_H

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
// инициализация буфера вершин пирамиды
inline void InitPyramidVertices(Vertex* pyramidVertexArray);
// инициализация шейдерных объектов
inline void InitShaders();
// инициализация буфера вершин стен
inline HRESULT InitWallsIndexBuffer(WORD* indices);