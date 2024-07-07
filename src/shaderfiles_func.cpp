#define _SHADERFILES_FUNC_CPP

#ifndef _PREPROC_H
#include "preproc.h"
#endif

#ifndef _STRUCT_H
#include "struct.h"
#endif

#ifndef _PREPROC_FUNC_H
#include "preproc_func.h"
#endif

#ifndef _GLOBAL_EXTERN_H
#include "global_extern.h"
#endif

inline void CompileAndSaveShadersFromList(WCHAR* shadersListDir, size_t shadersListDirLength, WCHAR* fileDirBuffer, size_t fileDirBufLength){
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
	ReadFile(shadersListHandle, (LPVOID*)shadersListBuf, SHADERS_LIST_CHAR_NUM * sizeof(WCHAR), &bytesReadNum, NULL);
	CloseHandle(shadersListHandle);
	WCHAR* shaderNamePtr = shadersListBuf + 1; // игнорировать BOM байты
	
	// массив шейдеров
	shadersBufferArray = (ID3DBlob**)(shadersListBuf + SHADERS_LIST_CHAR_NUM);
	
	for(size_t i = SHADERS_LIST_CHAR_NUM - 1, shaderNum = 0, vertexShaderNum = 0, pixelShaderNum = 0; i != 0;){
		// поиск файла
		FindFilesInCurrentDir(fileDirBuffer, fileDirBufLength, shaderNamePtr + 1, *((unsigned __int16*)shaderNamePtr), &fileData);
		
		// определяем вид шейдера - пиксельный или вершинный
		switch(*(shaderNamePtr + *((unsigned __int16*)shaderNamePtr) - 7)){ // в конце названия шейдера стоит VS либо PS, смотрим V или P в конце
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
				
		csoRelPathLength += *((unsigned __int16*)shaderNamePtr) - 5; // - 5 потому что без "hlsl\0"
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

inline bool CheckCompiledShadersFromLogFile(WCHAR* exeFileDir, size_t exeFileDirLength, WCHAR* logFileDir, size_t logFileDirLength){
	wmemcpy(&exeFileDir[exeFileDirLength - 1], &logFileDir[0], logFileDirLength);
	WCHAR flagsBuf[1 + LOG_FILE_FLAGS_NUM];
	DWORD bytesReadNum;
	
	HANDLE logFileHandle = CreateFile(exeFileDir, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ReadFile(logFileHandle, (LPVOID*)flagsBuf, (1 + LOG_FILE_FLAGS_NUM) * sizeof(WCHAR), &bytesReadNum, NULL);
	
	// если шейдеры уже были скомпилированы
	if((unsigned __int16)flagsBuf[LOG_FILE_COMPILED_SHADERS_FLAG_POS] == 1){
		exeFileDir[exeFileDirLength - 1] = NULL;
		CloseHandle(logFileHandle);
		return true;
	} 
	else {
		flagsBuf[LOG_FILE_COMPILED_SHADERS_FLAG_POS] = 0x0001;
		WriteFile(logFileHandle, (LPVOID*) flagsBuf, (1 + LOG_FILE_FLAGS_NUM) * sizeof(WCHAR), &bytesReadNum, NULL);
		
		exeFileDir[exeFileDirLength - 1] = NULL;
		CloseHandle(logFileHandle);
		return false;
	}
};

inline void CreateShadersObjFromCompiledShaders(WCHAR* shadersListDir, size_t shadersListDirLength, WCHAR* fileDirBuffer, size_t fileDirBufLength){
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
	
	for(size_t i = COMPILED_SHADERS_LIST_CHAR_NUM - 1, shaderNum = 0, vertexShaderNum = 0, pixelShaderNum = 0; i != 0;){
		// поиск файла
		FindFilesInCurrentDir(fileDirBuffer, fileDirBufLength, shaderNamePtr + 1, *((unsigned __int16*)shaderNamePtr), &fileData);
		
		// загрузить шейдер
		HRESULT hr = D3DReadFileToBlob(fileDirBuffer, &shadersBufferArray[shaderNum]); // создается ли blob или его нужно создать самому перед вызовом?
	
		// получение информации о шейдере
		D3DReflect(shadersBufferArray[shaderNum]->GetBufferPointer(), shadersBufferArray[shaderNum]->GetBufferSize(),IID_ID3D11ShaderReflection, (void**) &shaderReflect);
		shaderReflect->GetDesc(&shaderDesc);
		
		switch(D3D11_SHVER_GET_TYPE(shaderDesc.Version)){
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

HRESULT FindFilesInCurrentDir(WCHAR* fileDirBuffer, size_t fileDirBufLength, WCHAR* fileName, size_t fileNameLength, WIN32_FIND_DATA* fileDataPtr){
	// создается полный путь до файла
	wmemcpy(&fileDirBuffer[fileDirBufLength - 1], &fileName[0], fileNameLength);
	
	HANDLE searchHandle = FindFirstFileEx(fileDirBuffer, FindExInfoBasic, fileDataPtr, FindExSearchNameMatch, NULL, 0);
	// если нужный файл нашелся
	if(searchHandle != INVALID_HANDLE_VALUE){
		// ret fileDirBuffer
		FindClose(searchHandle);
		return S_OK;
	} 
	// если нужный файл не нашелся
	else{
		FindClose(searchHandle);
		
		fileDirBuffer[fileDirBufLength] = L'*'; // fileDirLength потому что '*' нужно поставить после '\'
		fileDirBuffer[fileDirBufLength + 1] = NULL;
		fileDirBufLength += 2; 
			
		searchHandle = FindFirstFileEx(fileDirBuffer, FindExInfoBasic, fileDataPtr, FindExSearchNameMatch, NULL, 0);
		// если текущий каталог не пуст
		if (searchHandle != INVALID_HANDLE_VALUE) {
		do {
			// если найденый файл - каталог
			if(fileDataPtr->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// изменение текущей директории
				size_t folderNameLength = FileNameLength(fileDataPtr->cFileName);
				wmemcpy(&fileDirBuffer[fileDirBufLength - 2], (const wchar_t*)fileDataPtr->cFileName[0], folderNameLength);
				
				// если в следующей директории нашелся файл
				if (FindFilesInCurrentDir(fileDirBuffer, fileDirBufLength + folderNameLength - 2, fileName, fileNameLength, fileDataPtr) == S_OK){
					FindClose(searchHandle);
					return S_OK;
				}
			}	
		} while (FindNextFile(searchHandle, fileDataPtr) != 0);
		}
		// если директория пуста или все каталоги в директории проверены	
		else{
			FindClose(searchHandle);
			return E_FAIL;
		}
	}
};

inline size_t FileNameLength(WCHAR* name){
	size_t nameLength = 0;
	while(name[nameLength] != NULL){
		nameLength++;
	} 
	nameLength++;
	
	return nameLength;
};

inline size_t FindBackslashInPath(WCHAR* dir, size_t length){
	while(dir[length - 1] != L'\\'){
		length--;
	}
	return length;
};
