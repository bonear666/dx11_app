#define _SHADERFILES_FUNC_H

// функция компилирующая cso-файлы, сохраняющаяя их в отделные файлы, создающаяя шейдерные объекты из списка файлов с исходным кодом(hlsl).
// путь до файла-списка hlsl-файлов, относительный директории процесса, передается в shadersListDir
// созданные шейдерные объекты хранятся в vertexShadersObj и pixelShadersObj
inline void CompileAndSaveShadersFromList(WCHAR* shadersListDir, size_t shadersListDirLength, WCHAR* fileDirBuffer, size_t fileDirBufLength);

// функция проверяет, на основании log-файла, были ли уже скомпилированы шейдеры 
// exeFileDir - директория исполняемого процесса
// logFileDir - путь до log-файла относительно exeFileDir
// функция меняет log-файл, если шейдеры не были скомпилированы
inline bool CheckCompiledShadersFromLogFile(WCHAR* exeFileDir, size_t exeFileDirLength, WCHAR* logFileDir, size_t logFileDirLength);

// функция создает шейдерные объекты из скомпилированных шейдеров(cso)
// названия cso-файлов, из которых нужно создать шейдерные объекты, находятся в текстовом документе(shaders_list)
// относительный путь до текстового файла(включая само название файла) передается в первом параметре
// путь относителен директории в которой находится процесс(exe)
// созданные шейдерные объекты хранятся в vertexShadersObj и pixelShadersObj
inline void CreateShadersObjFromCompiledShaders(WCHAR* shadersListDir, size_t shadersListDirLength, WCHAR* fileDirBuffer, size_t fileDirBufLength);

// ищет первый попавшийся файл, заданный в качестве аргумента, в заданной директории fileDirBuffer
// fileName должден начининаться с '\', fileDirBuffer должен заканчиваться названием крайнего каталога или диска, без '\'
// путь до найденого файла находится в fileDirBuffer
// fileNameLength длина(кол-во символов) названия файла  
HRESULT FindFilesInCurrentDir(WCHAR* fileDirBuffer, size_t fileDirBufLength, WCHAR* fileName, size_t fileNameLength, WIN32_FIND_DATA* fileDataPtr);

// длина C-строки
inline size_t FileNameLength(WCHAR* name);

// находит позицию backslash, начиная с конца пути
inline size_t FindBackslashInPath(WCHAR* dir, size_t length);