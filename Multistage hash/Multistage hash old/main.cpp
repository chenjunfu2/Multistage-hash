#include "Multistage hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

//Êý¾Ý
typedef struct Data
{
	int n1 = 0;
	int n2 = 0;
	int n3 = 0;
}DATA, * PDATA;

//¶à½×¹þÏ£
//#define VARIABLE_ARRAY_STATE(Size, Arr) \
//_asm\
//{\
//sub esp, [Size];\
//mov [arr], esp;\
//push dword ptr [Size];\
//}\
//
//
//#define VARIABLE_ARRAY_RELEASE(Arr) \
//_asm\
//{\
//lea ecx, [arr - 4];\
//mov ecx, dword ptr [ecx];\
//add ecx, 4;\
//add esp, ecx;\
//}\





//void func(int x)
//{
//	char* arr = 0;
//	unsigned sizearr = sizeof(*arr) * x;
//	_asm
//	{
//		sub esp,sizearr
//		mov arr,esp
//	}
//
//
//	for (int i = 0; i < x - 1; ++i)
//	{
//		arr[i] = '0' + i;
//	}
//	arr[x - 1] = 0;
//
//	_asm
//	{
//		mov arr,0
//		add esp,sizearr
//	}
//	return;
//}


//#define RAND_64 rand()&0xfff<<52|rand()&0xfff<<40|rand()&0xfff<<28|rand()&0xfff<<16|rand()&0xfff<<4|rand()&0xf



int main(void)
{
	int ret = 0;
	MULTIHASHBASE stMultiHashBase;
	if (!InitMultiHash(100000, 1.21, 20, &stMultiHashBase))
	{
		FreeMultiHash(&stMultiHashBase);
		return -2;
	}
		

	//srand(19380);

	unsigned long long i = 0;
	while (AddMultiHash(&stMultiHashBase, NULL, rand(), (void*)(i + 1)))
		++i;
	printf("Add OK\n\n");

	PMULTIHASHTABLE pstTemp= stMultiHashBase.pstMultiHashOrderTable;
	for (int i = 0; i < stMultiHashBase.ulOrder; ++i)
	{
		printf("Table[%02d] Use:%d/%d=%.6lf\n", i, pstTemp[i].ulUseLayerElement, pstTemp[i].ulLayerElement, (double)pstTemp[i].ulUseLayerElement / (double)pstTemp[i].ulLayerElement);
	}
	printf("\nAll Use:%lld/%lld=%.6lf", stMultiHashBase.ullUseTotalElement, stMultiHashBase.ullTotalElement, (double)stMultiHashBase.ullUseTotalElement / (double)stMultiHashBase.ullTotalElement);


	FreeMultiHash(&stMultiHashBase);
	return ret;
}
