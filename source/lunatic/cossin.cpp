#include "cossin.h"
#include <math.h>

const int DEGREES = 256;

int costab[DEGREES];
int sintab[DEGREES];

void InitCosSin(void)
{
	int i;

	for (i = 0; i < DEGREES; i++)
	{
		costab[i] = (int) (cos((float) i * 3.14159 * 2 / DEGREES) * FIXAMT);
		sintab[i] = (int) (sin((float) i * 3.14159 * 2 / DEGREES) * FIXAMT);
	}
}

int Cosine(int angle)
{
	return costab[angle];
}

int Sine(int angle)
{
	return sintab[angle];
}

void Dampen(int *value, int amt)
{
	if (*value > 0)
	{
		*value -= amt;
		if (*value < 0)
			*value = 0;
	}
	if (*value < 0)
	{
		*value += amt;
		if (*value > 0)
			*value = 0;
	}
}

void Clamp(int *value, int amt)
{
	if (*value > amt)
		*value = amt;
	if (*value<-amt)
		*value = -amt;
}
