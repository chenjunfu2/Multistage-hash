#pragma once
/*
volatile是一个特征修饰符（type specifier）
volatile的作用是作为指令关键字，确保本条指令不会因编译器的优化而省略，且要求每次直接读值。
volatile的变量是说这变量可能会被意想不到地改变，这样，编译器就不会去假设这个变量的值了。
*/

#define NULL 0


/*多阶哈希结构*/


//数据索引结构
typedef struct MultiHashElement
{
	void* pData = NULL;//数据指针(哈希值)
	unsigned long long ullKey = 0;//哈希键
	//char cState = 0;//结构状态 0:未被使用 1:已被使用 -1:正在修改
}MULTIHASHELEMENT, * PMULTIHASHELEMENT;

//阶索引结构
typedef struct MultiHashTable
{
	unsigned long ulLayerElement = 0;//该阶元素数
	unsigned long ulUseLayerElement = 0;//已使用的元素数
	PMULTIHASHELEMENT pstLayerMultiHashTableElement = NULL;//该阶数组首元素指针
}MULTIHASHTABLE, * PMULTIHASHTABLE;

//基索引结构
typedef struct MultiHashBase
{
	unsigned long ulOrder = 0;//阶数
	unsigned long long ullTotalElement = 0;//总元素数
	unsigned long long ullUseTotalElement = 0;//总使用的元素数
	PMULTIHASHTABLE pstMultiHashOrderTable = NULL;//阶层表首元素指针
}MULTIHASHBASE, * PMULTIHASHBASE;

//元素索引结构
typedef struct ElementAddr
{
	PMULTIHASHTABLE pstOrderTable = 0;//阶层指针
	unsigned long ulTableElement = 0;//阶数组索引
}ELEMENTADDR, * PELEMENTADDR;

//元素数据结构
typedef struct Element_Data
{
	ELEMENTADDR stElementAddr;//数据索引
	PMULTIHASHELEMENT stHashElement = NULL;//数据索引结构
}ELEMENT_DATA, * PELEMENT_DATA;


#undef NULL

/*

基索引结构(结构数组)->阶索引结构(结构数组)->数据索引结构(数据指针)->数据

base->(table) 0 1 2 3 4 5 6 7

(table)
		0->(element) 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
		1->(element) 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
		2->(element) 0 1 2 3 4 5 6 7 8 9 10 11 12
		3->(element) 0 1 2 3 4 5 6 7 8 9 10
		4->(element) 0 1 2 3 4 5 6
		5->(element) 0 1 2 3 4
		6->(element) 0 1 2
		7->(element) 0 1

		(element)
				0->(data) data
				1->(data) data
				.
				.
				.
*/

bool InitMultiHash(unsigned long ulStartElement, double dMultiPower, unsigned long ulOrder, PMULTIHASHBASE pstMultiHashBase);

void FreeMultiHash(PMULTIHASHBASE pstMultiHashBase);

bool AddMultiHash(PMULTIHASHBASE pstMultiHashBase, PELEMENTADDR pstElementAddr, unsigned long ullKey, void* pData);

bool DelMultiHash(PMULTIHASHBASE pstMultiHashBase, PELEMENTADDR pstElementAddr, unsigned long ullKey, void** pData);
