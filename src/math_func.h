#define _MATH_FUNC_H

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _STRUCT_H
#include "struct.h"
#endif

#if !defined(_GLOBAL_EXTERN_H) && !defined(_MAIN_CPP)
#include "global_extern.h"
#endif

// Обновление проекционной матрицы (0 < angle < PI/2). angle = fov/2
// nearZ, farZ координаты плоскости отсечений, деленные на W
// nearZ не должен быть равен нулю, так как исходя из формулы, Z координата вершины после умножения на матрицу проекции будет всегда равна 1
void SetProjectionMatrix(MatricesBuffer* pMatricesBuffer, FLOAT angleHoriz, FLOAT angleVert, FLOAT nearZ, FLOAT farZ, BOOL saveProportionsFlag);
// Сохранение пропорций объектов, при выводе в окно. Пропорции сохраняются в соответствии с осью, у которой меньше единичный отрезок. 
void SaveProportions(MatricesBuffer* pMatricesBuffer, HWND hWnd);
// Наибольший элемент
inline FLOAT MaxElement(FLOAT arg0, FLOAT arg1);
inline FLOAT MaxElement(FLOAT arg0, FLOAT arg1) {
	return arg0 >= arg1 ? arg0 : arg1;
};
// Создание обратной матрицы
HRESULT InvertMatrix(XMVECTOR zAxis, XMVECTOR yAxis, XMMATRIX* invertibleMatrix);
// Создание матрицы перехода к другой системе координат
HRESULT NewCoordinateSystemMatrix(XMVECTOR point, XMVECTOR zAxis, XMVECTOR yAxis, XMMATRIX* invertibleMatrix);
// Создание матрицы мира
void SetWorldMatrix(XMVECTOR point, XMVECTOR scale, XMMATRIX* worldMatrix);
// изменение порядка обхода вершин
inline void InvertIndices(WORD* indicesArray, int size);
void InvertIndices(WORD* indicesArray, int size) {
	WORD triangleEnd;

	for (int i = 2; i < size; i = i + 3) {
		triangleEnd = indicesArray[i];

		indicesArray[i] = indicesArray[i - 2];
		indicesArray[i - 2] = triangleEnd;
	}
};
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
