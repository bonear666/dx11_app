#define _MATH_FUNC_CPP

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _PREPROC_FUNC_H
#include "preproc_func.h"
#endif

void SetProjectionMatrix(MatricesBuffer* pMatricesBuffer, FLOAT angleHoriz, FLOAT angleVert, FLOAT nearZ, FLOAT farZ, BOOL saveProportionsFlag) {
	// (0 < angle < PI/2) => (0 < tg(angle) < +inf) => (0 < newCoeff < 1)

	FLOAT sinAngle;
	FLOAT cosAngle;
	FLOAT tangentAngle;
	FLOAT newCoeff;

	//FLOAT nearZ = 2.5f; // ближняя плоскоть отсечения
	//FLOAT farZ = 3.2f; // дальняя плоскость отсечения
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

inline FLOAT MaxElement(FLOAT arg0, FLOAT arg1) {
	arg0 >= arg1? return arg0 : return arg1;
};

HRESULT InvertMatrix(XMVECTOR zAxis, XMVECTOR yAxis, XMMATRIX* invertibleMatrix) {
	// проверка векторов на ортогональность 
	XMFLOAT4 dotProduct;
	XMStoreFloat4(&dotProduct, XMVector3Dot(zAxis, yAxis));
	if (dotProduct.x != 0.0f) {
		return E_FAIL;
	}
	
	// нахождение новой оси X
	XMVECTOR xAxis = XMVector3Cross(yAxis, zAxis); // правая тройка векторов
	// векторы линейно независимы, значит можно искать обратную матрицу

	// матрица обратная обратной матрице, обычная матрица
	XMVECTOR axisArray[] = { xAxis, yAxis, zAxis };

	// порядок строк в обратной матрице, который получился в результате поиска обратной матрицы
	int invertibleMatrixRowsOrder[3];

	// единичная матрица
	XMMATRIX invertibleMatrixOutput = {
	 XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
	 XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
	 XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
	 XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
	};

	//поиск обратной матрицы методом гаусса
	for (size_t axisIndex = 0; axisIndex <= 1; ++axisIndex) { // обход вниз по строкам обычной матрицы
		for (size_t i = 0; i <= 2; ++i) { // обход по координатам строки обычной матицы
			float axisElement = XMVectorGetByIndex(axisArray[axisIndex], i);
			if (axisElement != 0) {
				axisArray[axisIndex] /= XMVectorSet(axisElement, axisElement, axisElement, axisElement);
				invertibleMatrixOutput.r[axisIndex] /= XMVectorSet(axisElement, axisElement, axisElement, axisElement);
				invertibleMatrixRowsOrder[axisIndex] = i;

				for (size_t nextAxisIndex = axisIndex + 1; nextAxisIndex <= 2; ++nextAxisIndex) { // прибавление строки к другим строкам
					float nextAxisElement = XMVectorGetByIndex(axisArray[nextAxisIndex], i);
					XMVECTOR mulVector = axisArray[axisIndex] * XMVectorSet(-nextAxisElement, -nextAxisElement, -nextAxisElement, -nextAxisElement); // вместо простого умножения axisArray[axisIndex] на -nextAxisElement приходится умножать на вектор, так как в ином случае возникает сильная погрешность
					XMVECTOR invertMatrixMulVector = invertibleMatrixOutput.r[axisIndex] * XMVectorSet(-nextAxisElement, -nextAxisElement, -nextAxisElement, -nextAxisElement);

					axisArray[nextAxisIndex] += mulVector;
					invertibleMatrixOutput.r[nextAxisIndex] += invertMatrixMulVector;
				}
				break;
			}
		}
	};
	for (size_t i = 0; i <= 2; ++i) { // обход полседней строки обычной матрицы
		float axisElement = XMVectorGetByIndex(axisArray[2], i);
		if (axisElement != 0) {
			axisArray[2] /= XMVectorSet(axisElement, axisElement, axisElement, axisElement);
			invertibleMatrixOutput.r[2] /= XMVectorSet(axisElement, axisElement, axisElement, axisElement);
			invertibleMatrixRowsOrder[2] = i;

			break;
		}
	};
	for (size_t axisIndex = 2; axisIndex >= 1; --axisIndex) { // обход вверх по строкам обычной матрицы
		for (size_t i = 0; i <= 2; ++i) { // обход по координатам строки обычной матицы
			float axisElement = XMVectorGetByIndex(axisArray[axisIndex], i);
			if (axisElement != 0) {
				for (size_t nextAxisIndex = axisIndex - 1; nextAxisIndex < UINT_MAX; --nextAxisIndex) { // прибавление строки к другим строкам
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

	//преобразование обратной матрицы к правильному порядку строк
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
	// при умножении вершины на матрицу перехода к новой системе координат вычитаем начало координат новой системы координат
	point *= XMVectorSet(-1.0f, -1.0f, -1.0f, 1.0f);
	*invertibleMatrix = XMMatrixTranslationFromVector(point) * (*invertibleMatrix);

	return S_OK;
};

void SetWorldMatrix(XMVECTOR point, XMVECTOR scale, XMMATRIX* worldMatrix) {
	*worldMatrix = XMMatrixTranspose(XMMatrixScalingFromVector(scale) * XMMatrixTranslationFromVector(point));
};

inline void InvertIndices(WORD* indicesArray, int size) {
	WORD triangleEnd;

	for (int i = 2; i < size; i = i + 3) {
		triangleEnd = indicesArray[i];

		indicesArray[i] = indicesArray[i-2];
		indicesArray[i - 2] = triangleEnd;
	}
};

XMVECTOR FindOrthogonalVector(XMVECTOR vector) { // скалярное произведение ортогональных векторов равно нулю, на использовании этого факта и строится поиск ортогонального вектора
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
			// если координата x не ноль
			if (i == 0) {
				float zOfFirstVector = XMVectorGetByIndex(vector, 2);
				orthogonalVector.z = coordinate / (sqrt(coordinate * coordinate + zOfFirstVector * zOfFirstVector));
				orthogonalVector.x = (-orthogonalVector.z * zOfFirstVector) / coordinate;

				return XMVectorSet(orthogonalVector.x, 0.0f, orthogonalVector.z, 0.0f);
			}

			// если координата y не ноль
			if (i == 1) {
				float zOfFirstVector = XMVectorGetByIndex(vector, 2);
				orthogonalVector.z = coordinate / (sqrt(coordinate * coordinate + zOfFirstVector * zOfFirstVector));
				orthogonalVector.y = (-orthogonalVector.z * zOfFirstVector) / coordinate;

				return XMVectorSet(0.0f, orthogonalVector.y, orthogonalVector.z, 0.0f);
			}

			// если координата z не ноль
			if (i == 2) {
				float yOfFirstVector = XMVectorGetByIndex(vector, 1);
				orthogonalVector.y = coordinate / (sqrt(coordinate * coordinate + yOfFirstVector * yOfFirstVector));
				orthogonalVector.z = (-orthogonalVector.y * yOfFirstVector) / coordinate;

				return XMVectorSet(0.0f, orthogonalVector.y, orthogonalVector.z, 0.0f);
			}
		}
	}
	// если дан нулевой вектор
	return XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
};

void RotationAroundAxis(XMVECTOR yAxis, XMVECTOR point, FLOAT angle, XMMATRIX* outputMatrix) {
	yAxis = XMVector3Normalize(yAxis);
	XMVECTOR zAxis = FindOrthogonalNormalizedVector(yAxis);
	XMMATRIX offsetMatrix = XMMatrixTranslationFromVector(_mm_mul_ps(point, XMVectorSet(-1.0f, -1.0f, -1.0f, 0.0f))); // матрица переноса вершины, которая будет поворачиваться вокруг оси, на -point
	XMMATRIX backOffsetMatrix = XMMatrixTranslationFromVector(point); // матрица переноса перенесенной вершины, которую уже повернули вокруг оси, на +point

	// переход координат вершины от координат в старом базисе к координатам в новом базисе, где заданная ось является осью Y
	XMMATRIX newCoordinates;
	NewCoordinateSystemMatrix(g_XMZero, zAxis, yAxis, &newCoordinates); // вектор xAxis, который находится в этой функции будет нормализованным, так как вектор, полученный
																	 // в результате векторного произведения нормализованных и ортогональных векторов тоже будет нормализованным  

	// так как заданная ось является осью Y, то и вращение будет происходить вокруг оси Y
	XMMATRIX yAxisRotationMatrix = XMMatrixRotationY(angle);

	// матрица возврата к старым координатам
	XMMATRIX transformationMatrix = {
		XMVector3Cross(yAxis, zAxis),
		yAxis,
		zAxis,
		g_XMIdentityR3 };

	// можно перемножить все матрицы, так как перемножение матриц ассоциативно, и получить одну матрицу. Не придется в шейдере на каждую вершину делать несколько перемножений матриц. 
	*outputMatrix = offsetMatrix * newCoordinates * yAxisRotationMatrix * transformationMatrix * backOffsetMatrix; 
};

void SetProjectionMatrixWithCameraDistance(MatricesBuffer* pMatricesBuffer, FLOAT angleHoriz, FLOAT angleVert, FLOAT nearZ, FLOAT farZ, FLOAT cameraDistance, BOOL saveProportionsFlag) {
	// (0 < angle < PI/2) => (0 < tg(angle) < +inf) => (0 < newCoeff < 1)

	FLOAT sinAngle;
	FLOAT cosAngle;
	FLOAT tangentAngle;
	FLOAT newCoeff;

	//nearZ = 0.6f; // ближняя плоскоть отсечения, деленная на W координату
	//farZ = 4.2f; // дальняя плоскость отсечения, деленная на W координату

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



