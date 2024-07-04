#ifndef MATH_EXTRAS_H
#define MATH_EXTRAS_H

#include <stdlib.h>

inline constexpr div_t floor_div(int numerator, int denominator)
{
	// equivalent to div(numerator, denominator) but constexpr
	div_t res = { numerator / denominator, numerator % denominator };
	if ((numerator >= 0) != (denominator >= 0) && res.rem != 0)
	{
		res.quot -= 1;
		res.rem += denominator;
	}
	return res;
}

inline constexpr ldiv_t floor_div(long numerator, long denominator)
{
	// equivalent to ldiv(numerator, denominator) but constexpr
	ldiv_t res = { numerator / denominator, numerator % denominator };
	if ((numerator >= 0) != (denominator >= 0) && res.rem != 0)
	{
		res.quot -= 1;
		res.rem += denominator;
	}
	return res;
}

inline constexpr lldiv_t floor_div(long long numerator, long long denominator)
{
	// equivalent to lldiv(numerator, denominator) but constexpr
	lldiv_t res = { numerator / denominator, numerator % denominator };
	if ((numerator >= 0) != (denominator >= 0) && res.rem != 0)
	{
		res.quot -= 1;
		res.rem += denominator;
	}
	return res;
}

#endif  // MATH_EXTRAS_H
