#define _HITBOX_FUNC_H

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _STRUCT_H
#include "struct.h"
#endif

#if !defined(_GLOBAL_EXTERN_H) && !defined(_MAIN_CPP)
#include "global_extern.h"
#endif

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
inline void __vectorcall MoveAndRotationDynamicHitBox(FXMVECTOR moveVector, float rotationAngle, DynamicHitBox* dynamicHitBox) {
	// перемещаем хитбокс
	sseProxyRegister0 = XMLoadFloat3(&(dynamicHitBox->position));
	sseProxyRegister0 += moveVector;
	XMStoreFloat3(&(dynamicHitBox->position), sseProxyRegister0);

	// изменяем угол
	if (dynamicHitBox->angle >= XM_2PI) {
		dynamicHitBox->angle += -XM_2PI + rotationAngle;
	}
	dynamicHitBox->angle += rotationAngle;
};
// проверка на столкновение камеры с динмаическими хитбоксами
void DynamicHitBoxesCollisionDetection();
// через сколько циклов центр динамического хитбокса выйдет за пределы карты
float DynamicHitBoxCyclesAmount(DynamicHitBox* hitBox);
// создание новго вектора движения динмаического хитбокса
void CreateNewMoveVectorForDynamicHitBox(DynamicHitBox* hitBox);
// начальная инициализация векторов движения динамических хитбоксов
inline void InitMoveVectorsAndActiveCyclesAmountForDynamicHitBoxes();
inline void InitMoveVectorsAndActiveCyclesAmountForDynamicHitBoxes() {
	for (int i = 0; i < DYNAMIC_HIT_BOX_AMOUNT; ++i) {
		CreateNewMoveVectorForDynamicHitBox(&DynamicHitBoxesArray[i]);
		DynamicHitBoxesArray[i].activeCyclesAmount = DynamicHitBoxCyclesAmount(&DynamicHitBoxesArray[i]);
	}
};