#pragma once
#include<stdint.h>
#include<algorithm>

class UtilityFunctions
{
public:
	static uint32_t min(uint32_t a, int b)
	{
		if(a <= b)
			return a;
		else
			return b & ~(1<<(sizeof(int)*8-1));
	}
};