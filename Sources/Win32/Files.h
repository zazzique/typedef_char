
#ifndef _FILES_H_
#define _FILES_H_


typedef struct _FileHandler
{
    U8 *data;
    I32 size;
    I32 current_pos;
    
    char file_base[128];
	char file_extention[16];
} FileHandler;

void Files_Init();
void Files_Release();
BOOL Files_GetFilePathByName(char *name, char *result);
BOOL Files_OpenFile(FileHandler *file, const char *name);
BOOL Files_OpenFileAltType(FileHandler *file, const char *name, const char *type);
BOOL Files_OpenFileOfType(FileHandler *file, const char *name, const char *type);
BOOL Files_GetData(FileHandler *file, void **data, I32 *size);
I32  Files_GetSize(FileHandler *file);
I32  Files_GetCurrentPos(FileHandler *file);
char *Files_GetFileExtension(FileHandler *file);
char *Files_GetFileBaseName(FileHandler *file);
BOOL Files_Read(FileHandler *file, void *data, int size);
BOOL Files_ReadCompressed(FileHandler *file, void *data);
void Files_Skip(FileHandler *file, int size);
void Files_SetPos(FileHandler *file, int pos);
void Files_CloseFile(FileHandler *file);


#endif /* _FILES_H_ */

