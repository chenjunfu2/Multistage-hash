#include "Multistage hash.h"
#include <stdlib.h>
#include <Windows.h>

#ifdef _DEBUG
#include <assert.h>
#endif


/*质数操作函数*/

//质数判断
//输入一个数n
//返回这个数是否是质数
bool IsPrimeNum(unsigned long n)
{
	if (n < 2)
		return false;

	for (unsigned long i = 2; i <= n / 2; ++i)
		if (!(n % i))
			return false;

	return true;
}

//最近质数
//输入一个数n
//返回离n最近并且更上一个不相等的质数
unsigned long FindNearPrimeNum(unsigned long n, unsigned l)
{
	//while (!IsPrimeNum(n))
	//	++n;
	//return n;
	
	unsigned long i = 0;
	while (true)
	{
		if (IsPrimeNum(n + i) && (n + i) != l)
			return n + i;
		if (IsPrimeNum(n - i) && (n - i) != l)
			return n - i;
		++i;
	}
	
}



/*多阶哈希辅助函数*/

//查找一个键是否存在于哈希表中，若存在返回元素数据结构若不存在返回第一个可插入此数据的地址
inline ELEMENT_DATA FindElementDataIsInTheTableAndReturnEmpty(const PMULTIHASHBASE pstMultiHashBase, unsigned long ullKey, void* pData)
{
	ELEMENT_DATA stElementData = { 0 };
	PMULTIHASHTABLE pstMultiHashTable = pstMultiHashBase->pstMultiHashOrderTable;
	for (pstMultiHashTable; pstMultiHashTable < pstMultiHashBase->pstMultiHashOrderTable + pstMultiHashBase->ulOrder; ++pstMultiHashTable)
	{
		PMULTIHASHELEMENT pstDataElement = &pstMultiHashTable->pstLayerMultiHashTableElement[ullKey % pstMultiHashTable->ulLayerElement];

		if (!pstDataElement->pData)//此处没有数据
		{
			if (!stElementData.stHashElement)//结构也为空
			{//储存此处空位
				stElementData.stElementAddr.pstOrderTable = pstMultiHashTable;
				stElementData.stElementAddr.ulTableElement = ullKey % pstMultiHashTable->ulLayerElement;
				stElementData.stHashElement = pstDataElement;
			}
			continue;
		}
		else if (pData == pstDataElement->pData)//数据相同
		{
			if (pstDataElement->ullKey == ullKey)//键也相等
			{//储存此处数据
				stElementData.stElementAddr.pstOrderTable = pstMultiHashTable;
				stElementData.stElementAddr.ulTableElement = ullKey % pstMultiHashTable->ulLayerElement;
				stElementData.stHashElement = pstDataElement;
				break;
			}
			continue;
		}
		else//数据不同也不为空
			continue;


		//if (pstDataElement->ullKey != ullKey)//键不相等
		//{
		//	if (!pstDataElement->pData && !stElementData.stHashElement)//确认此处没有数据并且结构为空
		//	{//储存此处空位
		//		stElementData.stElementAddr.pstOrderTable = pstMultiHashTable;
		//		stElementData.stElementAddr.ulTableElement = ullKey % pstMultiHashTable->ulLayerElement;
		//		stElementData.stHashElement = pstDataElement;
		//	}
		//	continue;
		//}
		//else//DataElement->ullKey == ullKey 键值相等
		//{
		//	if (pData == pstDataElement->pData)//数据相同才返回
		//	{//储存此处数据
		//		stElementData.stElementAddr.pstOrderTable = pstMultiHashTable;
		//		stElementData.stElementAddr.ulTableElement = ullKey % pstMultiHashTable->ulLayerElement;
		//		stElementData.stHashElement = pstDataElement;
		//		break;
		//	}
		//}
	}

	return stElementData;
}

//仅查找一个键是否存在于哈希表中
inline ELEMENT_DATA FindElementDataIsInTheTable(const PMULTIHASHBASE pstMultiHashBase, unsigned long ullKey, void* pData)
{
	ELEMENT_DATA stElementData = { 0 };
	PMULTIHASHTABLE pstMultiHashTable = pstMultiHashBase->pstMultiHashOrderTable;
	for (pstMultiHashTable; pstMultiHashTable < pstMultiHashBase->pstMultiHashOrderTable + pstMultiHashBase->ulOrder; ++pstMultiHashTable)
	{
		PMULTIHASHELEMENT pstDataElement = &pstMultiHashTable->pstLayerMultiHashTableElement[ullKey % pstMultiHashTable->ulLayerElement];

		if (pData == pstDataElement->pData && pstDataElement->ullKey == ullKey)//键&值都相等
		{//储存此处数据
			stElementData.stElementAddr.pstOrderTable = pstMultiHashTable;
			stElementData.stElementAddr.ulTableElement = ullKey % pstMultiHashTable->ulLayerElement;
			stElementData.stHashElement = pstDataElement;
			break;
		}
	}

	return stElementData;
}




/*多阶哈希操作函数*/

//初始化多阶哈希
//输入一个初始的最小元素数量ulStartElement(会近似到最近质数)，一个向上扩展倍率dMultiPower，一个固定阶数ulOrder，一个待初始化多阶哈希基结构体指针pstMultiHashBase
//返回多阶哈希表是否构造成功
bool InitMultiHash(unsigned long ulStartElement, double dMultiPower, unsigned long ulOrder, PMULTIHASHBASE pstMultiHashBase)
{
	PMULTIHASHTABLE pstMultiHashTable = (PMULTIHASHTABLE)malloc(ulOrder * sizeof(MULTIHASHTABLE));//申请固定阶数数组内存
	if (!pstMultiHashTable)
		return false;

	RtlZeroMemory(pstMultiHashTable, ulOrder * sizeof(MULTIHASHTABLE));//内存置零

	pstMultiHashBase->ulOrder = ulOrder;
	pstMultiHashBase->pstMultiHashOrderTable = pstMultiHashTable;


	ulStartElement = FindNearPrimeNum(ulStartElement, 0);//寻找初始阶层最小元素数的最近质数
	pstMultiHashBase->ullTotalElement = 0;

	for (unsigned long i = ulOrder; i > 0; --i)
	{
		pstMultiHashTable[i - 1].pstLayerMultiHashTableElement = (PMULTIHASHELEMENT)malloc(ulStartElement * sizeof(MULTIHASHELEMENT));
		if (!(pstMultiHashTable[i - 1].pstLayerMultiHashTableElement))
			return false;

		RtlZeroMemory(pstMultiHashTable[i - 1].pstLayerMultiHashTableElement, ulStartElement * sizeof(MULTIHASHELEMENT));//内存置零

		pstMultiHashTable[i - 1].ulLayerElement = ulStartElement;
		pstMultiHashBase->ullTotalElement += ulStartElement;

		ulStartElement = FindNearPrimeNum((unsigned long)(ulStartElement * dMultiPower), ulStartElement);//计算上一阶的元素数
	}

	return true;
}

//释放多阶哈希表内存
//输入一个待释放的多阶哈希基结构体指针pstMultiHashBase
//返回void
void FreeMultiHash(PMULTIHASHBASE pstMultiHashBase)
{
	PMULTIHASHTABLE pstMultiHashTable = pstMultiHashBase->pstMultiHashOrderTable;

	if (pstMultiHashTable)
	{
		for (unsigned long i = 0; i < pstMultiHashBase->ulOrder; ++i)
		{
			if (pstMultiHashTable[i].pstLayerMultiHashTableElement)
			{
				free(pstMultiHashTable[i].pstLayerMultiHashTableElement);
				pstMultiHashTable[i].pstLayerMultiHashTableElement = NULL;
			}
		}
		free(pstMultiHashTable);
		pstMultiHashBase->pstMultiHashOrderTable = pstMultiHashTable = NULL;
	}

	return;
}

//值：指向数据的指针

//增加一个键值对
//输入一对键值和插入后键值索引结构体指针（为NULL不返回）
//返回插入是否成功
bool AddMultiHash(PMULTIHASHBASE pstMultiHashBase, PELEMENTADDR pstElementAddr, unsigned long ullKey, void* pData)
{
#ifdef _DEBUG
	assert(pstMultiHashBase);
#endif
	if (!pData)
		return false;

	ELEMENT_DATA stElementData = FindElementDataIsInTheTableAndReturnEmpty(pstMultiHashBase, ullKey, pData);//查找数据是否存在

	if (pstElementAddr)//pstElementAddr不为NULL则返回数据插入位置
		*pstElementAddr = stElementData.stElementAddr;

	if (!stElementData.stHashElement)//表内无空间
	{
		return false;
	}
	
	if (!stElementData.stHashElement->pData)//数据不存在（pData为NULL）
	{
		stElementData.stHashElement->pData = pData;
		stElementData.stHashElement->ullKey = ullKey;
		++pstMultiHashBase->ullUseTotalElement;
		++stElementData.stElementAddr.pstOrderTable->ulUseLayerElement;
	}

	return true;
}



//删除一个键值对
//输入一个键或输入键值索引结构指针来删除一个元素
//返回操作是否成功
bool DelMultiHash(PMULTIHASHBASE pstMultiHashBase, PELEMENTADDR pstElementAddr, unsigned long ullKey, void* pData)
{
#ifdef _DEBUG
	assert(pstMultiHashBase);
#endif
	if (!pData)
		return false;

	ELEMENT_DATA stElementData;

	if (pstElementAddr)
	{
		stElementData.stHashElement = &pstElementAddr->pstOrderTable->pstLayerMultiHashTableElement[pstElementAddr->ulTableElement];
	}
	else
	{
		stElementData = FindElementDataIsInTheTable(pstMultiHashBase, ullKey, pData);//查找数据是否存在
	}


	if (stElementData.stHashElement->pData == pData)//存在数据:不为null且等于pData
	{
		//*pData = stElementData.stHashElement->pData;
		stElementData.stHashElement->pData = NULL;
		stElementData.stHashElement->ullKey = 0;
		--pstMultiHashBase->ullUseTotalElement;
		++stElementData.stElementAddr.pstOrderTable->ulUseLayerElement;
	}

	return true;
}


//修改一个键值对（分为修改键和修改值）
//输入一个键或输入键值索引结构指针和要修改的键或值和修改后键值索引结构体指针（为null不返回）
//返回修改后的值（为null则代表失败）


//查找一个键值对（只能通过键查找值）
//输入一个键和查找的键值索引结构体指针（为null不返回）
//返回查找元素的值（为null则代表失败）


//直接定位一个表元素（通过阶层单元索引和阶单元索引直接定位）
//输入键值索引结构指针
//返回元素结构指针（为null则代表失败）


//交换两个键值对下的值（只交换值不交换键）
//输入两个键或两个键值索引结构指针和一个元素结构（返回第一个元素结构）
//返回第二个元素结构


//获取所有已使用键值对（用所有键值对做参调用给定函数）(分为正向遍历和反向遍历)
//输入一个函数指针和一个正向/方向遍历的布尔值，对于每个键值对都调用函数（多次调用的函数一旦返回false立刻结束调用并返回）
//返回一个布尔值，如果多次调用函数返回false就返回false，否则全部遍历完成返回true