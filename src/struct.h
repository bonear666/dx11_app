
using namespace DirectX;											
// ОПИСАНИЕ СТРУКТУР

// Описание вершин
struct Vertex {
	XMFLOAT4 position;
	XMFLOAT4 color;
};

// модель шейдеров
struct ShaderModelDesc {
	LPCSTR vertexShaderModel;
	LPCSTR pixelShaderModel;
};

// матрицы
struct MatricesBuffer {
	XMMATRIX mWorld;              // Матрица мира
	XMMATRIX mView;        // Матрица вида
	XMMATRIX mProjection;  // Матрица проекции
	XMMATRIX mRotationAroundAxis; // Матрица поворота вокруг оси
};

// структура угла
struct AngleConstantBuffer {
	float angle0;
	float angle1;
	float angle2;
	float angle3;
};

// Ширина и высота хитбокса
typedef XMFLOAT2 HitBoxWidthAndHeight;
// угол хитбокса
typedef FLOAT HitBoxAngle;
// смещения в массиве
typedef unsigned __int8 ArrayOffset8Bit;
typedef unsigned __int16 ArrayOffset16Bit;

// структура центра хитбокса, и позиции камеры в глобальных координатах
typedef struct {
	float x;
	float z;
} HitBoxCenter, CameraPosition;

// структура хитбокса
struct HitBox {
	float centerX;
	float centerZ;
	XMFLOAT3 widthHeightLength;
	const XMMATRIX* angleMatrixRotation; // матрица смещения и поворота
	const XMMATRIX* invertAngleMatrixRotation; // обратная матрица смещения и поворота
};

// структура описания хитбокса
struct HitBoxDesc {
	ArrayOffset8Bit widthAndHeightArrayPos; // позиция в массиве структур, которые описывают ширину и высоту хитбокса
	ArrayOffset8Bit centerArrayPos; // позиция в массиве структур, которые описывают центр хитбокса
	ArrayOffset16Bit angleArrayPos; // позиция в массиве структур, которые описывают угол хитбокса
};// хотя такая структура скорее всего будет медленее работать, чем если бы вместо 1-байтных интов я использовал обычные 4-байтные инты
// по идее процессору предется каждый раз логически умножать 1-байтный инт на маску чтобы занулить ненужные биты и смещать его в начало регистра, я так предполагаю

// структура области неподвижных хитбоксок
struct StaticHitBoxArea {
	StaticHitBoxArea* leftNode; //левый узел в бинарном дереве
	// у каждого узла, который не является листком, всегда есть два узла-потомка
	// у листка нет потомков, значит можно ввести такое объединение:
	union {
		StaticHitBoxArea* rightNode; //правый узел в бинарном дереве
		HitBox** staticHitBoxesArray; //указатель на массив, в котором храянтся указатели хитбоксов, которые находятся в данной зоне
	};
	StaticHitBoxArea* parentNode; //родительский узел в бинарном дереве
	float centerX; //центр области
	float centerZ;
	float lowX; // отрезок, характеризующий ширину области(координаты отрезка представлены в глобальной системе координат)
	float highX;
	float lowZ; // отрезок, характеризующий длину области(координаты отрезка представлены в глобальной системе координат)
	float highZ;
	int staticHitBoxesAmount; // количество статических хитбоксов в массиве статических массивов(актуально только для листьев)
	bool nodeSideFlag; // 0 если узел является левым узлом parentNode, 1 если узел является правым узлом parentNode
};

// структура динамических хитбоксов
struct DynamicHitBox {
	float halfOfShellEdge;
	XMFLOAT3 position;
	XMFLOAT3 halvesOfWidthHeightLength;
	float angle;
	XMVECTOR* moveVectorPtr;
	float activeCyclesAmount;
	XMFLOAT3 startPosition; // куда возвращаться, когда хитбокс вышел за пределы карты
};