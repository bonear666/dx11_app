#define _HITBOX_FUNC_H

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