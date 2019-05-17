#include "palettes.h"
#include "ham_mgldraw.h"
#include <algorithm>

void WaterPalette(MGLDraw *mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for(int i=0; i<8; i++)
		for(int j=0; j<32; j++)
		{
			pal2[i*32+j] = pal[i*32+j];
			pal2[i*32+j].b = (byte) std::min((int) pal2[i*32+j].b + 4*40, 4*63);
		}

	mgl->SetSecondaryPalette(pal2);
}

void LavaPalette(MGLDraw *mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for(int i=0; i<8; i++)
		for(int j=0; j<32; j++)
		{
			pal2[i*32+j] = pal[i*32+j];
			pal2[i*32+j].r = (byte) std::min((int) pal2[i*32+j].r + 4*40, 4*63);
		}

	mgl->SetSecondaryPalette(pal2);
}

void DumbSidePalette(MGLDraw *mgl)
{
	const RGB* pal = mgl->GetPalette();
	PALETTE pal2;

	for(int i=0; i<8; i++)
		for(int j=0; j<32; j++)
		{
			pal2[i*32+j] = pal[i*32+j];
			pal2[i*32+j].r = (byte) std::min((int) pal2[i*32+j].r + 4*30, 4*63);
			pal2[i*32+j].b = (byte) std::min((int) pal2[i*32+j].b + 4*30, 4*63);
			if(pal2[i*32+j].g > 10*4)
				pal2[i*32+j].g -= 10*4;
			else
				pal2[i*32+j].g = 0;
		}

	mgl->SetSecondaryPalette(pal2);
}
