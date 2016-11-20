
#include "Common.h"

#include "Files.h"
#include "FastMath.h"
#include "Render.h"
#include "TextureManager.h"

#define MAX_TEX_COUNT 512

typedef struct _TGAHeader
{
	U8  identsize;          // size of ID field that follows 18 byte header (0 usually)
	U8  colourmaptype;      // type of colour map 0=none, 1=has palette
	U8  imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed
	
	U16 colourmapstart;     // first colour map entry in palette
	U16 colourmaplength;    // number of colours in palette
	U8  colourmapbits;      // number of bits per palette entry 15,16,24,32
	
	U16 xstart;             // image x origin
	U16 ystart;             // image y origin
	U16 width;              // image width in pixels
	U16 height;             // image height in pixels
	U8  bits;               // image bits per pixel 8,16,24,32
	U8  descriptor;         // image descriptor bits (vh flip bits)
} TGAHeader;

typedef struct _Texture
{
	char filename[32];
	U32 texture;
	I32 width;
	I32 height;
	U32 flags;
	BOOL loaded;
	BOOL valid;
}
Texture;

Texture textures[MAX_TEX_COUNT];
int current_texture_index;


void TexManager_Init()
{
	for (int i = 0; i < MAX_TEX_COUNT; i ++)
	{
		textures[i].loaded = FALSE;
		textures[i].valid = FALSE;
	}
	
	current_texture_index = -1;
}

int	TexManager_AddTexture(const char *filename, U32 user_flags)
{
	int index = TexManager_GetTextureIndexByName(filename);
	if (index >= 0)
		return index;
	
	index = -1;
	
	for (int i = 0; i < MAX_TEX_COUNT; i++)
	{
		if (!textures[i].valid)
		{
			index = i;
			break;
		}
	}
	
	if (index < 0)
	{
		LogPrint("Error: too many textures!\n");
		return -1;
	}

	if (!TexManager_LoadTexture(index, filename))
		return -1;
	
	textures[index].flags = user_flags;
	textures[index].valid = TRUE;

	return index;
}

BOOL TexManager_LoadTexture(int index, const char *filename)
{
	if (index < 0 || index >= MAX_TEX_COUNT)
		return FALSE;

	if (textures[index].loaded)
		return TRUE;

	int i, j;
	
	if (filename != textures[index].filename)
		strncpy(textures[index].filename, filename, 31);

	FileHandler tex_file;

	if (!Files_OpenFileAltType(&tex_file, filename, "crt"))
	{
		LogPrint("Error: texture '%s' not found!\n", filename);
		return FALSE;
	}
	
	if (strcmp(Files_GetFileExtension(&tex_file), "tga") == 0) // TGA files
	{
		TGAHeader header;
		
		Files_Read(&tex_file, &header.identsize, sizeof(U8));
		Files_Read(&tex_file, &header.colourmaptype, sizeof(U8));
		Files_Read(&tex_file, &header.imagetype, sizeof(U8));
		Files_Read(&tex_file, &header.colourmapstart, sizeof(U16));
		Files_Read(&tex_file, &header.colourmaplength, sizeof(U16));
		Files_Read(&tex_file, &header.colourmapbits, sizeof(U8));
		Files_Read(&tex_file, &header.xstart, sizeof(U16));
		Files_Read(&tex_file, &header.ystart, sizeof(U16));
		Files_Read(&tex_file, &header.width, sizeof(U16));
		Files_Read(&tex_file, &header.height, sizeof(U16));
		Files_Read(&tex_file, &header.bits, sizeof(U8));
		Files_Read(&tex_file, &header.descriptor, sizeof(U8));
		
		if (header.colourmaptype != 0 || (header.imagetype != 2 && header.imagetype != 3))
		{
			LogPrint("Error: unsupported tga format!\n");
			Files_CloseFile(&tex_file);
			return FALSE;
		}
		
		int current_pos;
		
		if (header.bits == 8)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height);
			
			for (j = 0; j < header.height; j++)
			{
				for (i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i);
					else
						current_pos = ((header.height - (j + 1)) * header.width + i);
					
					
					Files_Read(&tex_file, &image_data[current_pos], sizeof(U8));
				}
			}
			
	        Render_CreateTexture(&textures[index].texture, image_data, header.width, header.height, header.bits, FALSE, 0);
			
			free(image_data);
		}
		else if (header.bits == 16)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height * 4);
			
			U16 in_color;

			for (j = 0; j < header.height; j++)
			{
				for (i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i) * 4;
					else
						current_pos = ((header.height - (j + 1)) * header.width + i) * 4;
					
					Files_Read(&tex_file, &in_color, sizeof(U16));
					image_data[current_pos + 0] = ((in_color >> 10) & 0xff) * 8;
					image_data[current_pos + 1] = ((in_color >> 5) & 0xff) * 8;
					image_data[current_pos + 2] = ((in_color >> 0) & 0xff) * 8;
					image_data[current_pos + 3] = (image_data[current_pos + 0] + image_data[current_pos + 1] + image_data[current_pos + 2] == 0) ? 0x00: 0xff;
					
					//ApplyColorModifications(&image_data[current_pos + 2], &image_data[current_pos + 1], &image_data[current_pos + 0]);
				}
			}
			
			Render_CreateTexture(&textures[index].texture, image_data, header.width, header.height, header.bits, FALSE, 0);
            
			free(image_data);
		}
		else if (header.bits == 24)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height * 3);
			
			for (j = 0; j < header.height; j++)
			{
				for (i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i) * 3;
					else
						current_pos = ((header.height - (j + 1)) * header.width + i) * 3;
					
					Files_Read(&tex_file, &image_data[current_pos + 2], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 1], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 0], sizeof(U8));
					
					//ApplyColorModifications(&image_data[current_pos + 2], &image_data[current_pos + 1], &image_data[current_pos + 0]);
				}
			}
			
            Render_CreateTexture(&textures[index].texture, image_data, header.width, header.height, header.bits, FALSE, 0);
			
			free(image_data);
		}
		else if (header.bits == 32)
		{
			U8 *image_data = (U8 *)malloc(header.width * header.height * 4);
			
			for (j = 0; j < header.height; j++)
			{
				for (i = 0; i < header.width; i++)
				{
					if (header.descriptor & 0x20)
						current_pos = (j * header.width + i) * 4;
					else
						current_pos = ((header.height - (j + 1)) * header.width + i) * 4;
					
					Files_Read(&tex_file, &image_data[current_pos + 2], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 1], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 0], sizeof(U8));
					Files_Read(&tex_file, &image_data[current_pos + 3], sizeof(U8));
					
					//ApplyColorModifications(&image_data[current_pos + 2], &image_data[current_pos + 1], &image_data[current_pos + 0]);
				}
			}
			
			Render_CreateTexture(&textures[index].texture, image_data, header.width, header.height, header.bits, FALSE, 0);
			
			free(image_data);
		}
		
		textures[index].width = header.width;
		textures[index].height = header.height;
	}
	else if ((strcmp(Files_GetFileExtension(&tex_file), "crt") == 0) || (strcmp(Files_GetFileExtension(&tex_file), "crthd") == 0)) // CRT files
	{
		U16 width, height;
		U8 bits;
		
		Files_Read(&tex_file, &width, sizeof(U16));
		Files_Read(&tex_file, &height, sizeof(U16));
		Files_Read(&tex_file, &bits, sizeof(U8));

		U8 *image_data = NULL;
		Files_GetData(&tex_file, (void **)&image_data, NULL);

		if (image_data == NULL)
		{
			LogPrint("Error: couldn't get texture data!\n");
			Files_CloseFile(&tex_file);
			return FALSE;
		}

		if (bits & 128)
		{
			bits &= 127;

			U32 pvrtc_rawsize = 0;
			Files_Read(&tex_file, &pvrtc_rawsize, sizeof(U32));
			
			I32 current_pos = Files_GetCurrentPos(&tex_file);

			Render_CreateTexture(&textures[index].texture, &image_data[current_pos], width, height, bits, TRUE, pvrtc_rawsize);
		}
		else
		{
			I32 current_pos = Files_GetCurrentPos(&tex_file);

            Render_CreateTexture(&textures[index].texture, &image_data[current_pos], width, height, bits, FALSE, 0);
		}
		
		textures[index].width = width;
		textures[index].height = height;
	}
	
	else
	{
		LogPrint("Error: unsupported texture format!\n");
		Files_CloseFile(&tex_file);
		return FALSE;
	}

	current_texture_index = index;
	
	textures[index].loaded = TRUE;

	Files_CloseFile(&tex_file);
	
	return TRUE;
}

BOOL TexManager_UnloadTexture(int index)
{
	if (index < 0 || index >= MAX_TEX_COUNT)
		return FALSE;

	Render_DeleteTexture(&textures[index].texture);
	textures[index].loaded = FALSE;
        
	return TRUE;
}

void TexManager_LoadAll()
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			if (!textures[i].loaded)
				TexManager_LoadTexture(i, textures[i].filename);
}

void TexManager_UnloadAll()
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			if (textures[i].loaded)
				TexManager_UnloadTexture(i);
}

int TexManager_GetTextureIndexByName(const char *name)
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			if (textures[i].filename[0] == name[0])
			{
				int result = strcmp(textures[i].filename, name);
				if (result == 0)
					return i;
			}
	
	return -1;
}


void TexManager_SetTextureByIndex(int index)
{
	if (index == current_texture_index)
		return;
	
	if (index >= 0 && index < MAX_TEX_COUNT)
	{
		if (textures[index].valid)
		{
			if (!textures[index].loaded)
			{
				TexManager_LoadTexture(index, textures[index].filename);
			}
			else
			{
				Render_BindTexture(textures[index].texture);
				current_texture_index = index;
			}
		}
	}
	else
	{
		current_texture_index = -1;
	}
}


void TexManager_SetTextureByName(const char *name)
{
	int index = TexManager_GetTextureIndexByName(name);
	
	TexManager_SetTextureByIndex(index);
}

void TexManager_GetTextureResolutionByIndex(int index, I32 *width, I32 *height)
{
	if (index < 0)
		return;
	
	*width = textures[index].width;
	*height = textures[index].height;
}

void TexManager_GetTextureResolutionByName(const char *name, I32 *width, I32 *height)
{
	int index = TexManager_GetTextureIndexByName(name);

	if (index < 0)
		return;
	
	TexManager_GetTextureResolutionByIndex(index, width, height);
}

void TexManager_RemoveTextureByIndex(int index)
{
	if (index >= 0 && index < MAX_TEX_COUNT)
	{
		if (textures[index].loaded)
			Render_DeleteTexture(&textures[index].texture);
        
		textures[index].loaded = FALSE;
		textures[index].valid = FALSE;
	
		if (index == current_texture_index)
			current_texture_index = -1;
	}
}


void TexManager_RemoveTextureByName(const char *name)
{
	int index = TexManager_GetTextureIndexByName(name);
	
	TexManager_RemoveTextureByIndex(index);
}


void TexManager_RemoveTexturesByFlag(U32 user_flag)
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			if (textures[i].flags & user_flag)
				TexManager_RemoveTextureByIndex(i);
}


void TexManager_RemoveAllTextures()
{
	for (int i = 0; i < MAX_TEX_COUNT; i++)
		if (textures[i].valid)
			TexManager_RemoveTextureByIndex(i);
}


void TexManager_Release()
{
	TexManager_RemoveAllTextures();
}

