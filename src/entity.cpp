#include "entity.h"


int getIndexOfFirstZeroBit(u64 num)
{
	int index = 0;
    
	// If the input number is 0, return 0.
	if (num == 0) 
	{
		return 0;
	}
    
	// Iterate through the bits from the least significant bit (LSB).
	while ((num & 1) == 1) 
	{
		num >>= 1;
		index++;
	}
    
	return index;

}