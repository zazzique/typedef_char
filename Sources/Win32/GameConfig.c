
#include <stdio.h>
#include "Common.h"
#include "GameDefines.h"
#include "GameVariables.h"
#include "GameConfig.h"

BOOL GameConfig_Load()
{
	U32 size;
	char *data;
	
	FILE *file = fopen("game.cfg", "rb");
	
	if (file == NULL)
		return FALSE;
	
	fseek (file, 0, SEEK_END);
	size = ftell (file);
	rewind (file);
	
	data = (char *)malloc((size + 1) * sizeof(char));

	if (data == NULL)
		return FALSE;
		
	size_t result = fread(data, sizeof(char), size, file);
	
	if (result != size)
		return FALSE;

	data[size] = '\0';
		
	char *cur_exp = NULL;
	cur_exp = strtok(data, CONTENT_PARSE_SEPARATORS);
	
	while (cur_exp != NULL)
	{
		if (strcmp(cur_exp, "debug") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			debug_mode = atoi(cur_exp);
			continue;
		}
		else if (strcmp(cur_exp, "sound_volume") == 0)
		{
			cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
			sound_volume = atof(cur_exp);
			continue;
		}
		
		cur_exp = strtok(NULL, CONTENT_PARSE_SEPARATORS);
	}

	fclose(file);
	free(data);


/*	file = fopen("state.bin", "rb");

	if (file == NULL)
		return FALSE;

	I32 levels_count = 0;
	fread(&levels_count, sizeof(I32), 1, file);
	
	for (int i = 0; i < levels_count; i ++)
	{
		fread(&level_stats[i].locked, sizeof(BOOL), 1, file);
		fread(&level_stats[i].scored, sizeof(BOOL), 1, file);
		fread(&level_stats[i].time, sizeof(float), 1, file);
		fread(&level_stats[i].score, sizeof(I32), 1, file);
	}

	fclose(file);*/
	
	return TRUE;
}

void GameConfig_Save()
{
	char line[256];

	FILE *file = fopen("game.cfg", "w");
	
	sprintf(line, "debug = %d\n", debug_mode);
	fputs(line, file);

	sprintf(line, "sound_volume = %g\n", sound_volume);
	fputs(line, file);

	fclose(file);


/*	file = fopen("state.bin", "wb");

	I32 levels_count = LEVELS_COUNT;
	fwrite(&levels_count, sizeof(I32), 1, file);
	
	for (int i = 0; i < LEVELS_COUNT; i ++)
	{
		fwrite(&level_stats[i].locked, sizeof(BOOL), 1, file);
		fwrite(&level_stats[i].scored, sizeof(BOOL), 1, file);
		fwrite(&level_stats[i].time, sizeof(float), 1, file);
		fwrite(&level_stats[i].score, sizeof(I32), 1, file);
	}

	fclose(file);*/
}
