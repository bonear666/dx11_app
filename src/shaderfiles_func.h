#define _SHADERFILES_FUNC_H

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _STRUCT_H
#include "struct.h"
#endif

#if !defined(_GLOBAL_EXTERN_H) && !defined(_MAIN_CPP)
#include "global_extern.h"
#endif

#ifndef _DIRECTX_FUNC_H
#include "directx_func.h"
#endif

HRESULT CompileShader(LPCWSTR srcName, LPCSTR entryPoint, LPCSTR target, ID3DBlob** buffer); // пришлось добвать объявление здесь

// ищет первый попавшийся файл, заданный в качестве аргумента, в заданной директории fileDirBuffer
// fileName должден начининаться с '\', fileDirBuffer должен заканчиваться названием крайнего каталога или диска, без '\'
// путь до найденого файла находится в fileDirBuffer
// fileNameLength длина(кол-во символов) названия файла  
HRESULT FindFilesInCurrentDir(WCHAR* fileDirBuffer, size_t fileDirBufLength, WCHAR* fileName, size_t fileNameLength, WIN32_FIND_DATA* fileDataPtr);

// функция компилирующая cso-файлы, сохраняющаяя их в отделные файлы, создающаяя шейдерные объекты из списка файлов с исходным кодом(hlsl).
// путь до файла-списка hlsl-файлов, относительный директории процесса, передается в shadersListDir
// созданные шейдерные объекты хранятся в vertexShadersObj и pixelShadersObj
inline void CompileAndSaveShadersFromList(WCHAR* shadersListDir, size_t shadersListDirLength, WCHAR* fileDirBuffer, size_t fileDirBufLength);
inline void CompileAndSaveShadersFromList(WCHAR* shadersListDir, size_t shadersListDirLength, WCHAR* fileDirBuffer, size_t fileDirBufLength) {
	HRESULT hr;
	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;

	// буфер хранения строки пути файла
	//WCHAR fileDirBuffer [MAX_PATH];
	// длина пути файла с учетом null(все длины строк с учетом null), без учета имени искомого файла
	//DWORD fileDirBufLength = GetCurrentDirectory(MAX_PATH, fileDirBuffer) + 1;

	// путь до списка шейдеров
	WCHAR shadersList[MAX_PATH];
	wmemcpy(shadersList, fileDirBuffer, fileDirBufLength);
	wmemcpy(&shadersList[fileDirBufLength - 1], shadersListDir, shadersListDirLength);

	// значения из файла списка шейдеров, хранятся в том же буфере, что и хитбоксы
	shadersListBuf = (WCHAR*)(DynamicHitBoxesArray + DYNAMIC_HIT_BOX_AMOUNT);
	DWORD bytesReadNum;

	// извлечение информации из файла списка шейдеров
	HANDLE shadersListHandle = CreateFile(shadersList, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	hr = ReadFile(shadersListHandle, (LPVOID*)shadersListBuf, SHADERS_LIST_CHAR_NUM * sizeof(WCHAR), &bytesReadNum, NULL);
	CloseHandle(shadersListHandle);
	WCHAR* shaderNamePtr = shadersListBuf + 1; // игнорировать BOM байты

	// массив шейдеров
	shadersBufferArray = (ID3DBlob**)(shadersListBuf + SHADERS_LIST_CHAR_NUM);

	for (size_t i = SHADERS_LIST_CHAR_NUM - 1, shaderNum = 0, vertexShaderNum = 0, pixelShaderNum = 0; i != 0;) {
		// поиск файла
		FindFilesInCurrentDir(fileDirBuffer, fileDirBufLength, shaderNamePtr + 1, *((unsigned __int16*)shaderNamePtr), &fileData);

		// определяем вид шейдера - пиксельный или вершинный
		switch (*(shaderNamePtr + *((unsigned __int16*)shaderNamePtr) - 7)) { // в конце названия шейдера стоит VS либо PS, смотрим V или P в конце
		case L'V':
		{
			hr = CompileShader(fileDirBuffer, "main", shadersModel.vertexShaderModel, &shadersBufferArray[shaderNum]);
			g_pd3dDevice->CreateVertexShader(shadersBufferArray[shaderNum]->GetBufferPointer(), shadersBufferArray[shaderNum]->GetBufferSize(), NULL, &vertexShadersObj[vertexShaderNum]);

			vertexShaderNum++;
			break;
		}

		case L'P':
		{
			hr = CompileShader(fileDirBuffer, "main", shadersModel.pixelShaderModel, &shadersBufferArray[shaderNum]);
			g_pd3dDevice->CreatePixelShader(shadersBufferArray[shaderNum]->GetBufferPointer(), shadersBufferArray[shaderNum]->GetBufferSize(), NULL, &pixelShadersObj[pixelShaderNum]);

			pixelShaderNum++;
			break;
		}
		}
		// сохранить шейдер в файле
		// создание относительного пути до cso-файла (res\compiled_shaders\*shaderName_shaderType*.cso), пусть путь хранится в shadersList
		size_t csoRelPathLength = WCHAR_NUM_OF_STRING(L"res\\compiled_shaders");

		wmemcpy(shadersList, L"res\\compiled_shaders", csoRelPathLength);
		wmemcpy(&shadersList[csoRelPathLength - 1], shaderNamePtr + 1, *((unsigned __int16*)shaderNamePtr) - 5);

		csoRelPathLength += *((unsigned __int16*)shaderNamePtr) - 5 - 1; // - 5 потому что без "hlsl\0"
		wmemcpy(&shadersList[csoRelPathLength], L"cso", WCHAR_NUM_OF_STRING(L"cso"));

		// создание cso-файла
		HANDLE csoFileHandle = CreateFile(shadersList, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		WriteFile(csoFileHandle, shadersBufferArray[shaderNum]->GetBufferPointer(), shadersBufferArray[shaderNum]->GetBufferSize(), &bytesReadNum, NULL);
		CloseHandle(csoFileHandle);
		//shadersBufferArray[shaderNum]->Realese();

		i -= *((unsigned __int16*)shaderNamePtr) + 1;
		shaderNamePtr += *((unsigned __int16*)shaderNamePtr) + 1;
		fileDirBuffer[fileDirBufLength - 1] = NULL;
		shaderNum++;
	}
};

// функция проверяет, на основании log-файла, были ли уже скомпилированы шейдеры 
// exeFileDir - директория исполняемого процесса
// logFileDir - путь до log-файла относительно exeFileDir
// функция меняет log-файл, если шейдеры не были скомпилированы
inline bool CheckCompiledShadersFromLogFile(WCHAR* exeFileDir, size_t exeFileDirLength, WCHAR* logFileDir, size_t logFileDirLength);
inline bool CheckCompiledShadersFromLogFile(WCHAR* exeFileDir, size_t exeFileDirLength, WCHAR* logFileDir, size_t logFileDirLength) {
	wmemcpy(&exeFileDir[exeFileDirLength - 1], &logFileDir[0], logFileDirLength);
	WCHAR flagsBuf[1 + LOG_FILE_FLAGS_NUM];
	DWORD bytesReadNum;

	HANDLE logFileHandle = CreateFile(exeFileDir, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ReadFile(logFileHandle, (LPVOID*)flagsBuf, (1 + LOG_FILE_FLAGS_NUM) * sizeof(WCHAR), &bytesReadNum, NULL);

	// если шейдеры уже были скомпилированы
	if ((unsigned __int16)flagsBuf[LOG_FILE_COMPILED_SHADERS_FLAG_POS] == 1) {
		exeFileDir[exeFileDirLength - 1] = NULL;
		CloseHandle(logFileHandle);
		return true;
	}
	else {
		flagsBuf[LOG_FILE_COMPILED_SHADERS_FLAG_POS] = 0x0001;
		SetFilePointer(logFileHandle, ((1 + LOG_FILE_FLAGS_NUM) * (int)sizeof(WCHAR)) * (-1), NULL, FILE_CURRENT);

		WriteFile(logFileHandle, (LPVOID*)flagsBuf, (1 + LOG_FILE_FLAGS_NUM) * sizeof(WCHAR), &bytesReadNum, NULL);

		exeFileDir[exeFileDirLength - 1] = NULL;
		CloseHandle(logFileHandle);
		return false;
	}
};

// функция создает шейдерные объекты из скомпилированных шейдеров(cso)
// названия cso-файлов, из которых нужно создать шейдерные объекты, находятся в текстовом документе(shaders_list)
// относительный путь до текстового файла(включая само название файла) передается в первом параметре
// путь относителен директории в которой находится процесс(exe)
// созданные шейдерные объекты хранятся в vertexShadersObj и pixelShadersObj
inline void CreateShadersObjFromCompiledShaders(WCHAR* shadersListDir, size_t shadersListDirLength, WCHAR* fileDirBuffer, size_t fileDirBufLength);
inline void CreateShadersObjFromCompiledShaders(WCHAR* shadersListDir, size_t shadersListDirLength, WCHAR* fileDirBuffer, size_t fileDirBufLength) {
	// найти cso файлы в текущей директории

	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;

	// буфер хранения строки пути файла
	//WCHAR fileDirBuffer [MAX_PATH];
	// длина пути файла с учетом null(все длины строк с учетом null), без учета имени искомого файла
	//DWORD fileDirBufLength = GetCurrentDirectory(MAX_PATH, fileDirBuffer) + 1;

	// путь до списка шейдеров
	WCHAR shadersList[MAX_PATH];
	wmemcpy(shadersList, fileDirBuffer, fileDirBufLength);
	wmemcpy(&shadersList[fileDirBufLength - 1], shadersListDir, shadersListDirLength);

	// значения из файла списка шейдеров, хранятся в том же буфере, что и хитбоксы
	shadersListBuf = (WCHAR*)(DynamicHitBoxesArray + DYNAMIC_HIT_BOX_AMOUNT);
	DWORD bytesReadNum;

	// извлечение информации из файла списка шейдеров
	HANDLE shadersListHandle = CreateFile(shadersList, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	ReadFile(shadersListHandle, (LPVOID*)shadersListBuf, COMPILED_SHADERS_LIST_CHAR_NUM * sizeof(WCHAR), &bytesReadNum, NULL);
	CloseHandle(shadersListHandle);
	WCHAR* shaderNamePtr = shadersListBuf + 1; // игнорировать BOM байты

	// массив шейдеров
	shadersBufferArray = (ID3DBlob**)(shadersListBuf + COMPILED_SHADERS_LIST_CHAR_NUM);
	// информация о шейдере
	D3D11_SHADER_DESC shaderDesc;

	for (size_t i = COMPILED_SHADERS_LIST_CHAR_NUM - 1, shaderNum = 0, vertexShaderNum = 0, pixelShaderNum = 0; i != 0;) {
		// поиск файла
		FindFilesInCurrentDir(fileDirBuffer, fileDirBufLength, shaderNamePtr + 1, *((unsigned __int16*)shaderNamePtr), &fileData);

		// загрузить шейдер
		HRESULT hr = D3DReadFileToBlob(fileDirBuffer, &shadersBufferArray[shaderNum]); // создается ли blob или его нужно создать самому перед вызовом?

		// получение информации о шейдере
		D3DReflect(shadersBufferArray[shaderNum]->GetBufferPointer(), shadersBufferArray[shaderNum]->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&shaderReflect);
		shaderReflect->GetDesc(&shaderDesc);
		shaderReflect->Release();

		switch (D3D11_SHVER_GET_TYPE(shaderDesc.Version)) {
			// создание шейдерных объектов
			// если вершинный шейдер
		case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_VERTEX_SHADER:
		{
			g_pd3dDevice->CreateVertexShader(shadersBufferArray[shaderNum]->GetBufferPointer(), shadersBufferArray[shaderNum]->GetBufferSize(), NULL, &vertexShadersObj[vertexShaderNum]);
			//shadersBufferArray[shaderNum]->Realese();
			vertexShaderNum++;
			break;
		}

		// если пиксельный
		case D3D11_SHADER_VERSION_TYPE::D3D11_SHVER_PIXEL_SHADER:
		{
			g_pd3dDevice->CreatePixelShader(shadersBufferArray[shaderNum]->GetBufferPointer(), shadersBufferArray[shaderNum]->GetBufferSize(), NULL, &pixelShadersObj[pixelShaderNum]);
			//shadersBufferArray[shaderNum]->Realese();
			pixelShaderNum++;
			break;
		}
		}

		i -= *((unsigned __int16*)shaderNamePtr) + 1;
		shaderNamePtr += *((unsigned __int16*)shaderNamePtr) + 1;
		fileDirBuffer[fileDirBufLength - 1] = NULL;
		shaderNum++;
	}
};

// длина C-строки
inline size_t FileNameLength(WCHAR* name);
inline size_t FileNameLength(WCHAR* name) {
	size_t nameLength = 0;
	while (name[nameLength] != NULL) {
		++nameLength;
	}

	return ++nameLength;
};

// находит позицию backslash, начиная с конца пути
inline size_t FindBackslashInPath(WCHAR* dir, size_t length);
inline size_t FindBackslashInPath(WCHAR* dir, size_t length) {
	--length;
	while (dir[length] != L'\\') {
		--length;
	}
	return length;
};