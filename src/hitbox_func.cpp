#define _HITBOX_FUNC_CPP

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

void InitPageSizeAndAllocGranularityVariables() {
	SYSTEM_INFO systemInfo;
	GetNativeSystemInfo(&systemInfo);
	pageSize = systemInfo.dwPageSize;
	allocationGranularity = systemInfo.dwAllocationGranularity;
};

bool ChangesOfStaticHtBoxesArea() {
	// если позиция камеры находится внутри текущей области статических хитбоксов
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
	// так как нет корня в бинарном дереве, то приходится отдельно проверять в какой из двух первых(0 и 1) областей 
	// находится камера
	// если камера находится внутри нулевой области статических хитбоксов
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

	// пока не дошли до листа продолжаем определять в какой области находится камера
	while (currentHitBoxArea->leftNode != NULL) {
		// если камера находится внутри нулевой области статических хитбоксов
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
	// кладем текущую позицую камеры
	sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);

	// если позиция камеры оказалась внутри хитбокса(т.е. произошло столкновение), значит нужно камеру вытолкнуть за пределы хитбокса
	// обходим все хибоксы из текущей области хитбоксов
	for (int i = 0; i < currentHitBoxArea->staticHitBoxesAmount; ++i) { 
		// смещаем и поворачиваем текущую позиция камеры
		sseProxyRegister0 = XMVector3Transform(sseProxyRegister0, *(currentHitBoxArea->staticHitBoxesArray[i]->angleMatrixRotation));
		
		// кладем ширину высоту длину хитбокса
		sseProxyRegister1 = XMLoadFloat3(&currentHitBoxArea->staticHitBoxesArray[i]->widthHeightLength);
		// если 0 <= x <= width && 0 <= y <= height && 0 <= z <= length, где xyz принадлежат текущей позиции камеры, а width height length - хитбоксу
		// то есть если позиция камеры внутри хитбокса
		if (XMVector3LessOrEqual(sseProxyRegister0, sseProxyRegister1) and
			XMVector3GreaterOrEqual(sseProxyRegister0, g_XMIdentityR3))
		{
			// находим расстояние позиции камеры до правых верхних граней хитбокса
			sseProxyRegister1 = sseProxyRegister1 - sseProxyRegister0;

			// находим минимум между расстоянием позиции камеры до нижних левых граней хитбокса(это координаты позиции камеры) и 
			// расстоянием позици камеры до верхних правых граней хитбокса
			sseProxyRegister2 = XMVectorMin(sseProxyRegister0, sseProxyRegister1);

			float minX = XMVectorGetX(sseProxyRegister2);
			float minZ = XMVectorGetZ(sseProxyRegister2);

			// если минимальное расстояние до граней находится по оси X
			if (minX < minZ) {
				//если минимальное расстояние явлется минимальным расстоянием от точки до левой грани хитбокса
				if (minX == XMVectorGetX(sseProxyRegister0)) {
					sseProxyRegister0 = XMVectorSetX(sseProxyRegister0, -0.005f); // 0.0005 это расстояние на которое отодвигается камера от грани хитбокса
				} 
				else { //если минимальное расстояние явлется минимальным расстоянием от точки до правой грани хитбокса
					sseProxyRegister1 = XMVectorSetX(sseProxyRegister1, 0.005f);
					sseProxyRegister2 += sseProxyRegister1;
					sseProxyRegister0 += XMVectorAndInt(sseProxyRegister2, g_XMMaskX);
				}
			}
			else {// если минимальное расстояние до граней находится по оси Z
				//если минимальное расстояние явлется минимальным расстоянием от точки до нижней грани хитбокса
				if (minZ == XMVectorGetZ(sseProxyRegister0)) {
					sseProxyRegister0 = XMVectorSetZ(sseProxyRegister0, -0.005f);
				}
				else { //если минимальное расстояние явлется минимальным расстоянием от точки до верхней грани хитбокса
					sseProxyRegister1 = XMVectorSetZ(sseProxyRegister1, 0.005f);
					sseProxyRegister2 += sseProxyRegister1;
					sseProxyRegister0 += XMVectorAndInt(sseProxyRegister2, g_XMMaskZ);
				}
			}
			// умножить на обратную матрицу
			sseProxyRegister0 = XMVector3Transform(sseProxyRegister0, *(currentHitBoxArea->staticHitBoxesArray[i]->invertAngleMatrixRotation));
			// сохранить измененную позицию в currentCameraPosition
			XMStoreFloat3(&currentCameraPos, sseProxyRegister0);

			//меняем view matrix, так чтобы она описывала систему координат в новой, смещенной точке
			//находим координаты новой позиции в текущей системе координат камеры
			sseProxyRegister0 = XMVector3Transform(sseProxyRegister0, matricesWVP.mView);
			//переносим систему координат камеры в новую точку
			matricesWVP.mView.r[3] -= sseProxyRegister0;
		}
	}
};

void InitHitBoxes() {
	// предположим, что размер страницы всегда является степенью двойки
	// тогда кол-во страниц, необходимое для того чтобы вместить все хитбоксы, можно вычислить так:

	//определяем есть ли выравнивание по 4 байта у гранулярности выделения памяти
	// остаток от деления гранулярности на 4 байта
	DWORD allocationGranularityAlignModulo = allocationGranularity & (4UL - 1UL);
	// дополнительные байты чтобы адрес выделенной памяти был выровнен
	SIZE_T extraBytes = 0;

	if (allocationGranularityAlignModulo != 0UL) { // если адрес выделяемой памяти невыровнен 
		extraBytes += (4UL - allocationGranularityAlignModulo);
	}
	// p.s. хотя наверно в windows всегда гранулярность выделения памяти через virtualalloc будет кратна 4 байтам

	// размер памяти, занимаемы хитбоксами
	SIZE_T hitBoxAmount = sizeof(extraBytes +
		STATIC_HIT_BOX_AMOUNT * sizeof(HitBox) +
		STATIC_HIT_BOX_AREA_AMOUNT * sizeof(StaticHitBoxArea) +
		STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_0 * sizeof(HitBox*) +
		STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_1 * sizeof(HitBox*) +
		STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_2 * sizeof(HitBox*) +
		STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_3 * sizeof(HitBox*) +
		DYNAMIC_HIT_BOX_AMOUNT * sizeof(DynamicHitBox));

	// количество страниц, необходимое для того чтобы вместить все хитбоксы
	pageAmount = hitBoxAmount & (0xFFFFFFFF ^ (pageSize - 1)); // сейчас здесь хранится только целая часть от деления hitBoxAmount на pageSize
	//SIZE_T moduloPart = hitBoxAmount & (pageSize - 1);
	//SIZE_T integerPart = hitBoxAmount & (0xFFFFFFFF ^ (pageSize - 1));

	// если остаток от деления hitBoxAmount на pageSize не ноль
	if ((hitBoxAmount & (pageSize - 1UL)) != 0UL) {
		++pageAmount;
	}
	// p.s. конечно легче и понятнее было бы посчитать количество занимаемх страниц памяти с помощью div и mod

	// выделение памяти под хитбоксы
	// сырая память с учетом байтов для выравнивания
	dynamicMemory = VirtualAlloc(NULL, pageAmount * pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	// указатель на массив статических хитбоксов, выровненный по границе 4 байта
	StaticHitBoxArray = (HitBox*)((BYTE*)dynamicMemory + extraBytes);
	// указатель на массив StaticHitBoxArea структур
	StaticHitBoxAreaArray = (StaticHitBoxArea*)(StaticHitBoxArray + STATIC_HIT_BOX_AMOUNT);
	// указатели на массивы позиций в StaticHitBoxDescArray, которые определяют какие статические хитбоксы есть в листьях staticHitBoxArea
	// 0 лист
	Leaf0Array = (HitBox**)(StaticHitBoxAreaArray + STATIC_HIT_BOX_AREA_AMOUNT);
	// 1 лист
	Leaf1Array = Leaf0Array + STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_0;
	// 2 лист
	Leaf2Array = Leaf1Array + STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_1;
	// 3 лист
	Leaf3Array = Leaf2Array + STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_2;

	// кладем данные в массивы статических хитбоксов
	// 0 хитбокс
	StaticHitBoxArray[0] = {
		0.0f,
		55.0f,
		{100.0f, 0.0f, 5.0f},
		&staticHitBoxesRotationMatricesArray[0],
		&invertStaticHitBoxesRotationMatricesArray[0]
	};
	// 1 хитбокс
	StaticHitBoxArray[1] = {
		55.0f,
		0.0f,
		{5.0f, 0.0f, 100.0f},
		&staticHitBoxesRotationMatricesArray[1],
		&invertStaticHitBoxesRotationMatricesArray[1]
	};
	// 2 хитбокс
	StaticHitBoxArray[2] = {
		0.0f,
		-55.0f,
		{100.0f, 0.0f, 5.0f},
		&staticHitBoxesRotationMatricesArray[2],
		&invertStaticHitBoxesRotationMatricesArray[2]
	};
	// 3 хитбокс
	StaticHitBoxArray[3] = {
		-55.0f,
		0.0f,
		{5.0f, 0.0f, 100.0f},
		&staticHitBoxesRotationMatricesArray[3],
		&invertStaticHitBoxesRotationMatricesArray[3]
	};

	// заполняем массивы листьев зон статических хитбоксов
	// массив 0 листа
	Leaf0Array[0] = &StaticHitBoxArray[0];
	Leaf0Array[1] = &StaticHitBoxArray[3];
	// массив 1 листа
	Leaf1Array[0] = &StaticHitBoxArray[0];
	Leaf1Array[1] = &StaticHitBoxArray[1];
	// массив 2 листа
	Leaf2Array[0] = &StaticHitBoxArray[2];
	Leaf2Array[1] = &StaticHitBoxArray[3];
	// массив 3 листа
	Leaf3Array[0] = &StaticHitBoxArray[1];
	Leaf3Array[1] = &StaticHitBoxArray[2];

	// кладем данные в массив областей статических хитбоксов
	// 0 область
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
	// 1 область
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
	// 2 область
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
	// 3 область
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
	// 4 область
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
	// 5 область
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
    
	// указатель на массив динмических хитбоксов
    DynamicHitBoxesArray = (DynamicHitBox*)(Leaf3Array + STATIC_HIT_BOX_AMOUNT_IN_STATIC_HIT_BOX_AREA_LEAF_3);
    
	//0 динмический хибокс
    DynamicHitBoxesArray[0] = {
        3.0f,
        {-25.0f, 0.0f, 25.0f},
        {2.0f, 0.0f, 2.0f},
        0.0f,
		&DynamicHitBoxesMoveVectorsArray[0],
		0.0f,
		{-25.0f, 0.0f, 25.0f}
    };
	//1 динмический хибокс
     DynamicHitBoxesArray[1] = {
        3.0f,
        {25.0f, 0.0f, 25.0f},
        {2.0f, 0.0f, 2.0f},
        0.0f,
		&DynamicHitBoxesMoveVectorsArray[1],
		0.0f,
		{25.0f, 0.0f, 25.0f}
    };
	 //2 динмический хибокс
     DynamicHitBoxesArray[2] = {
        3.0f,
        {0.0f, 0.0f, -25.0f},
        {2.0f, 0.0f, 2.0f},
        0.0f,
		&DynamicHitBoxesMoveVectorsArray[2],
		0.0f,
		{0.0f, 0.0f, -25.0f}
    };

	 //инициализация векторов движения динамических хитбоксов
	 InitMoveVectorsAndActiveCyclesAmountForDynamicHitBoxes();
};
/*
inline void __vectorcall MoveAndRotationDynamicHitBox(FXMVECTOR moveVector, float rotationAngle, DynamicHitBox* dynamicHitBox) {
	// перемещаем хитбокс
	sseProxyRegister0 = XMLoadFloat3(&(dynamicHitBox->position));
	sseProxyRegister0 += moveVector;
	XMStoreFloat3(&(dynamicHitBox->position), sseProxyRegister0);

	// изменяем угол
	if(dynamicHitBox->angle >= XM_2PI){
		dynamicHitBox->angle += -XM_2PI + rotationAngle;
	}
	dynamicHitBox->angle += rotationAngle;
}; */

void DynamicHitBoxesCollisionDetection() {
	// текущаяя позиция камеры
	sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);

	// обходим все динамические хитбоксы
	for (int i = 0; i < DYNAMIC_HIT_BOX_AMOUNT; ++i) {
		// позиция хитбокса
		sseProxyRegister4 = XMLoadFloat3(&DynamicHitBoxesArray[i].position);
		// координаты камеры относительно системы координат в центре хитбокса
		sseProxyRegister1 = sseProxyRegister0 - sseProxyRegister4;
		// положительные границы обертки динмаического хитбокса
		sseProxyRegister2 = XMVectorSet(DynamicHitBoxesArray[i].halfOfShellEdge, 0.0f, DynamicHitBoxesArray[i].halfOfShellEdge, 0.0f);
		// отрицательные границы обертки хитбокса
		sseProxyRegister3 = XMVectorNegate(sseProxyRegister2);

		// если камера находится внутри обертки хитбокса
		if (XMVector3LessOrEqual(sseProxyRegister1, sseProxyRegister2) and
			XMVector3GreaterOrEqual(sseProxyRegister1, sseProxyRegister3)) 
		{
			// поворот позиции камеры
			sseProxyRegister0_Matrix = XMMatrixRotationY(-DynamicHitBoxesArray[i].angle);
			sseProxyRegister1 = XMVector3Transform(sseProxyRegister1, sseProxyRegister0_Matrix);
			// координаты позиции камеры относительно системы координат, находящейся в левом нижнем углу хитбокса
			sseProxyRegister2 = XMLoadFloat3(&DynamicHitBoxesArray[i].halvesOfWidthHeightLength);
			sseProxyRegister1 += sseProxyRegister2;
			// кладем ширину высоту длину хитбокса
			sseProxyRegister2 *= 2.0f;

			// если камера находится в хитбоксе
			if (XMVector3LessOrEqual(sseProxyRegister1, sseProxyRegister2) and
				XMVector3GreaterOrEqual(sseProxyRegister1, g_XMIdentityR3))
			{
				// находим расстояние позиции камеры до правых верхних граней хитбокса
				sseProxyRegister2 = sseProxyRegister2 - sseProxyRegister1;

				// находим минимум между расстоянием позиции камеры до нижних левых граней хитбокса(это координаты позиции камеры) и 
				// расстоянием позици камеры до верхних правых граней хитбокса
				sseProxyRegister3 = XMVectorMin(sseProxyRegister1, sseProxyRegister2);

				float minX = XMVectorGetX(sseProxyRegister3);
				float minZ = XMVectorGetZ(sseProxyRegister3);

				// если минимальное расстояние до граней находится по оси X
				if (minX < minZ) {
					//если минимальное расстояние явлется минимальным расстоянием от точки до левой грани хитбокса
					if (minX == XMVectorGetX(sseProxyRegister1)) {
						sseProxyRegister1 = XMVectorSetX(sseProxyRegister1, -0.05f); // 0.0005 это расстояние на которое отодвигается камера от грани хитбокса
					}
					else { //если минимальное расстояние явлется минимальным расстоянием от точки до правой грани хитбокса
						sseProxyRegister2 = XMVectorSetX(sseProxyRegister2, 0.05f);
						sseProxyRegister3 += sseProxyRegister2;
						sseProxyRegister1 += XMVectorAndInt(sseProxyRegister3, g_XMMaskX);
					}
				}
				else {// если минимальное расстояние до граней находится по оси Z
					//если минимальное расстояние явлется минимальным расстоянием от точки до нижней грани хитбокса
					if (minZ == XMVectorGetZ(sseProxyRegister1)) {
						sseProxyRegister1 = XMVectorSetZ(sseProxyRegister1, -0.05f);
					}
					else { //если минимальное расстояние явлется минимальным расстоянием от точки до верхней грани хитбокса
						sseProxyRegister2 = XMVectorSetZ(sseProxyRegister2, 0.05f);
						sseProxyRegister3 += sseProxyRegister2;
						sseProxyRegister1 += XMVectorAndInt(sseProxyRegister3, g_XMMaskZ);
					}
				}
				// умножить на обратную матрицу
				sseProxyRegister2 = XMLoadFloat3(&DynamicHitBoxesArray[i].halvesOfWidthHeightLength);
				sseProxyRegister1 -= sseProxyRegister2;
				sseProxyRegister0_Matrix = XMMatrixRotationY(DynamicHitBoxesArray[i].angle);
				sseProxyRegister1 = XMVector3Transform(sseProxyRegister1, sseProxyRegister0_Matrix);
				sseProxyRegister2 = XMLoadFloat3(&DynamicHitBoxesArray[i].position);
				sseProxyRegister1 += sseProxyRegister2;
				// сохранить измененную позицию в currentCameraPosition
				XMStoreFloat3(&currentCameraPos, sseProxyRegister1);

				//меняем view matrix, так чтобы она описывала систему координат в новой, смещенной точке
				//находим координаты новой позиции в текущей системе координат камеры
				matricesWVP.mView = XMMatrixTranspose(matricesWVP.mView);
				sseProxyRegister1 = XMVector3Transform(sseProxyRegister1, matricesWVP.mView);
				sseProxyRegister1 = XMVectorSetW(sseProxyRegister1, 0.0f);
				//переносим систему координат камеры в новую точку
				matricesWVP.mView.r[3] -= sseProxyRegister1;
				matricesWVP.mView = XMMatrixTranspose(matricesWVP.mView);
			}
		}
	}
};

float DynamicHitBoxCyclesAmount(DynamicHitBox* hitBox) {
	// позиция хитбокса относительно системы координат, находящейся в левом нижнем углу карты
	sseProxyRegister0 = XMLoadFloat3(&hitBox->position);
	sseProxyRegister0 += halvesOfWidthHeightLengthOfMap;
	
	// узнаем какие координаты вектора движения отрицательные, и какие положительные
	sseProxyRegister1 = XMVectorAndInt(*hitBox->moveVectorPtr, g_XMNegate3);

	float xValue = XMVectorGetX(sseProxyRegister1);
	float zValue = XMVectorGetZ(sseProxyRegister1);

	// если X < 0 или X == -0
	if (*((UINT*) &xValue) == 0x80000000U) { // жуть... ну а как по другому, union или reinterpret_cast мб?
		sseProxyRegister2 = XMVectorSetX(sseProxyRegister2, 0.0f);
	}
	else {
		// если X > 0 или X == +0
		sseProxyRegister2 = XMVectorSetX(sseProxyRegister2, 100.0f); // 100 - ширина карты
	}
	
	// если Z < 0 или Z == -0
	if (*((UINT*) &zValue) == 0x80000000U) {
		sseProxyRegister2 = XMVectorSetZ(sseProxyRegister2, 0.0f);
	}
	else {
		// если Z > 0 или Z == +0
		sseProxyRegister2 = XMVectorSetZ(sseProxyRegister2, 100.0f); // 100 - длина карты
	}

	// количество moveVector-ов необходимых прибавить к позиции хитбокса, для того
	// чтобы центр хитбокса вышел за пределы карты
	// причем если какая то координата moveVector равна +0 или -0, то мы получим +inf в sseProxyRegister3 на месте этой координаты
	// это свойство хорошо используется при поиске минимума среди координат sseProxyRegister3
	sseProxyRegister3 = (sseProxyRegister2 - sseProxyRegister0) / (*hitBox->moveVectorPtr);

	// поиск минимума среди координат
	if (XMVectorGetX(sseProxyRegister3) < XMVectorGetZ(sseProxyRegister3)) {
		return XMVectorGetX(XMVectorCeiling(sseProxyRegister3));
	}
	return XMVectorGetZ(XMVectorCeiling(sseProxyRegister3));
};

void CreateNewMoveVectorForDynamicHitBox(DynamicHitBox* hitBox) {
	// кладем текущую позицию камеры
	sseProxyRegister0 = XMLoadFloat3(&currentCameraPos);
	// позиция хитбокса
	sseProxyRegister1 = XMLoadFloat3(&hitBox->position);

	// находим вектор
	sseProxyRegister0 -= sseProxyRegister1;
	sseProxyRegister0 = XMVector3NormalizeEst(sseProxyRegister0);
	sseProxyRegister0 *= DYNAMIC_HIT_BOX_MOVEVECTOR_LENGTH; 
	*hitBox->moveVectorPtr = sseProxyRegister0;
};
/*
inline void InitMoveVectorsAndActiveCyclesAmountForDynamicHitBoxes() {
	for (int i = 0; i < DYNAMIC_HIT_BOX_AMOUNT; ++i) {
		CreateNewMoveVectorForDynamicHitBox(&DynamicHitBoxesArray[i]);
		DynamicHitBoxesArray[i].activeCyclesAmount = DynamicHitBoxCyclesAmount(&DynamicHitBoxesArray[i]);
	}
};*/

