#include "Multistage hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>


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





//VOID func(int x)
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

class Rand
{
private:
	unsigned mX = 123456789;
	unsigned mY = 362436069;
	unsigned mZ = 521288629;
	unsigned mW;
public:
	Rand(unsigned seed) :mW(seed){}
	unsigned getUnsigned(VOID)
	{
		unsigned t = mX ^ (mX << 11);
		mX = mY;
		mY = mZ;
		mZ = mW;
		mW = (mW ^ (mW >> 19)) ^ (t ^ (t >> 8));
		return mW;
	}
};




//¶à½×¹þÏ£²âÊÔ
ULONGLONG Multiplication(ULONGLONG ullLast)
{
	return ULONGLONG((long double)ullLast * 1.2);
}

ULONGLONG HashKeyMap(ULONGLONG ullKey)
{
	srand(ullKey & 0x00000000ffffffff);
	ULONGLONG ullRand = rand();
	srand(ullKey >> 32);
	return ullRand + rand();
}

_BOOL TraverUseRate(ULONGLONG ullRate, ULONGLONG ullUseRate, ULONG ulCurrent)
{
	printf("Table[%02ld] Use:%lld/%lld=%.6lf\n", ulCurrent, ullUseRate, ullRate, (long double)ullUseRate / (long double)ullRate);
	return true;
}

_BOOL TraverUseRateAll(ULONGLONG ullRate, ULONGLONG ullUseRate, ULONG ulCurrent)
{
	printf("\nAll Use:%lld/%lld=%.6lf\n", ullUseRate, ullRate, (long double)ullUseRate / (long double)ullRate);
	return true;
}

#define TESTDATA (VOID*)1

int main(VOID)
{
	int ret = 0;
	Rand r0(0);
	Rand r1(1);

	MultistageHash hash(500, 15, Multiplication, malloc, free, HashKeyMap);


	//ULONGLONG i = 0;
	//while (hash.AddMultiHash((ULONGLONG)r0.getUnsigned() << 32 | (ULONGLONG)r1.getUnsigned(), (VOID*)(i + 1)))
		//++i;

	//printf("Add OK,Add:%lld\n\n", i);

	//hash.GetOrderUseRate(TraverUseRate);
	//hash.GetTotalUseRate(TraverUseRateAll);

	hash.AddMultiHash(12358, (VOID*)1);
	hash.AddMultiHash(652323, (VOID*)2);
	hash.AddMultiHash(135842, (VOID*)3);
	hash.AddMultiHash(1265890, (VOID*)4);
	
	VOID* temp;


	if(hash.FindMultiHashData(12358, &temp))
		printf("%ld ", (long)temp);
	if (hash.FindMultiHashData(652323, &temp))
		printf("%ld ", (long)temp);
	if (hash.FindMultiHashData(135842, &temp))
		printf("%ld ", (long)temp);
	if (hash.FindMultiHashData(1265890, &temp))
		printf("%ld", (long)temp);

	putchar('\n');
	hash.EmptyMultiHash();

	if (hash.FindMultiHashData(12358, &temp))
		printf("%ld ", (long)temp);
	if (hash.FindMultiHashData(652323, &temp))
		printf("%ld ", (long)temp);
	if (hash.FindMultiHashData(135842, &temp))
		printf("%ld ", (long)temp);
	if (hash.FindMultiHashData(1265890, &temp))
		printf("%ld", (long)temp);

	return 0;
}
