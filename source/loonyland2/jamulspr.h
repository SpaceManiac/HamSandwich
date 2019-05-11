#ifndef JAMULSPR_H
#define JAMULSPR_H

#include "winpch.h"
#include "mgldraw.h"
#pragma pack(1)

class sprite_t
{
	public:
		sprite_t(void);
		sprite_t(byte *info);

		~sprite_t(void);

		bool LoadData(FILE *f);
		bool SaveData(FILE *f);
		void GetHeader(byte *buffer);
		void Draw(int x,int y,MGLDraw *mgl);

		void DrawBright(int x,int y,MGLDraw *mgl,char bright);
		void DrawColored(int x,int y,MGLDraw *mgl,byte color,char bright);
		void DrawGhost(int x,int y,MGLDraw *mgl,char bright);
		void DrawGlow(int x,int y,MGLDraw *mgl,char bright);
		void DrawOffColor(int x,int y,MGLDraw *mgl,byte fromColor,byte toColor,char bright);

		// this makes half-height tilted black shadows (they darken by 4)
		void DrawShadow(int x,int y,MGLDraw *mgl);
		void GetCoords(int x,int y,int *rx,int *ry,int *rx2,int *ry2);

		word width;
		word height;
		short ofsx;
		short ofsy;
		dword size;
		byte *data;
};

class sprite_set_t
{
	public:
		sprite_set_t(void);
		sprite_set_t(char *fname);

		~sprite_set_t(void);

		bool Save(char *fname);
		bool Load(char *fname);
		void Free(void);
		sprite_t *GetSprite(int which);
		word GetCount(void);
	protected:
		word count;
		sprite_t **spr;
};

void NewComputerSpriteFix(char *fname,int x,int y);

#endif
