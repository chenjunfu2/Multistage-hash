#pragma once
#include "Multistage hash.h"//class state
#include <string.h>//using memset function
#include <math.h>//using sqrt function

#define ZEROMEMORY(mem, size) memset(mem, 0, size)
#define NULL 0
#define CONST const

/*质数操作函数*/

//质数判定
template<typename Key, typename Data>
_BOOL inline MultistageHash<Key, Data>::IsPrimeNum(ULONGLONG n) CONST
{
	if (n <= 3)
		return n > 1;
	if (n % 6 != 1 && n % 6 != 5)
		return false;
	register ULONGLONG s = sqrtl(n);
	for (ULONGLONG i = 5; i <= s; i += 6)
		if (!(n % i) || !(n % (i + 2)))
			return false;
	return true;
}


//查找最近的质数
template<typename Key, typename Data>
ULONGLONG inline MultistageHash<Key, Data>::FindNearPrimeNum(ULONGLONG n, ULONGLONG l) CONST
{
	ULONGLONG i = 0;
	while (true)
	{
		if (IsPrimeNum(n + i) && (n + i) != l)//大数优先
			return n + i;
		if (IsPrimeNum(n - i) && (n - i) != l)//小数滚边
			return n - i;
		++i;
	}

}

/*统计数据反馈函数*/

//获取层数
template<typename Key, typename Data>
ULONG MultistageHash<Key, Data>::GetOrderTableLayers(VOID) CONST
{
	return ulOrderTable;
}

//获取整张表最大可容纳元素数
template<typename Key, typename Data>
ULONGLONG MultistageHash<Key, Data>::GetTotalElement(VOID) CONST
{
	return ullTotalElement;
}

//获取整张表已容纳元素数
template<typename Key, typename Data>
ULONGLONG MultistageHash<Key, Data>::GetUseTotalElement(VOID) CONST
{
	return ullUseTotalElement;
}

//获取某一层最大可容纳元素数
template<typename Key, typename Data>
ULONGLONG MultistageHash<Key, Data>::GetOrderElement(ULONG ulLayer) CONST
{
	return pstOrderTable[ulLayer].ullOrderElement;
}

//获取某一层已容纳元素数
template<typename Key, typename Data>
ULONGLONG MultistageHash<Key, Data>::GetUseOrderElement(ULONG ulLayer) CONST
{
	return pstOrderTable[ulLayer].ullUseOrderElement;
}

/*多阶哈希辅助函数*/

//查找一个键是否存在于哈希表中如果不存在则返回第一个空位
template<typename Key, typename Data>
MultistageHash<Key, Data>::ElementAddr<Key, Data> inline MultistageHash<Key, Data>::FindElementDataIsInTheTableOrReturnFirstEmpty(CONST Key&& tKey) CONST
{
	ElementAddr<Key, Data> stElementAddr = { 0 };
	ULONGLONG ullKeyMap = fHashKeyMap((CONST Key&&)tKey);

	for (OrderElement<Key, Data>* pstLoop = pstOrderTable; pstLoop < pstOrderTable + ulOrderTable; ++pstLoop)
	{
		ElementData<Key, Data>* pstElementData = &pstLoop->pstOrderElement[ullKeyMap % pstLoop->ullOrderElement];

		if (!fIsKeyExists((CONST Key&&)pstElementData->tKey))//此处没有数据
		{
			if (!stElementAddr.pstOrderElement)//结构为空
			{//储存此处空位
				stElementAddr.pstOrderTable = pstLoop;
				stElementAddr.pstOrderElement = pstElementData;
			}
			continue;
		}
		else//找到数据
		{
			if (fIsKeyEqual((CONST Key&&)pstElementData->tKey, (CONST Key&&)tKey))//键相等
			{//储存此处数据
				stElementAddr.pstOrderTable = pstLoop;
				stElementAddr.pstOrderElement = pstElementData;
				break;
			}
			continue;
		}
	}

	return stElementAddr;
}


//仅查找一个键是否存在于哈希表中
template<typename Key, typename Data>
MultistageHash<Key, Data>::ElementAddr<Key, Data> inline MultistageHash<Key, Data>::FindElementDataIsInTheTable(CONST Key&& tKey) CONST
{
	ElementAddr<Key, Data> stElementAddr = { 0 };
	ULONGLONG ullKeyMap = fHashKeyMap((CONST Key&&)tKey);

	for (OrderElement<Key, Data>* pstLoop = pstOrderTable; pstLoop < pstOrderTable + ulOrderTable; ++pstLoop)
	{
		ElementData<Key, Data>* pstElementData = &pstLoop->pstOrderElement[ullKeyMap % pstLoop->ullOrderElement];

		if (fIsKeyEqual((CONST Key&&)pstElementData->tKey, (CONST Key&&)tKey))//找到数据并且键相等
		{//储存此处数据
			stElementAddr.pstOrderTable = pstLoop;
			//stElementAddr.ulTableElement = ullKeyMap % pstLoop->ullOrderElement;
			stElementAddr.pstOrderElement = pstElementData;
			break;
		}
		else
		{
			continue;
		}
	}

	return stElementAddr;
}



/*初始化和反向初始化*/

//初始化(构造函数)
template<typename Key, typename Data>
MultistageHash<Key, Data>::MultistageHash(ULONGLONG _ulStartElement, ULONG _ulOrder, MultiplicationFunc _fMultiplication,
	AllocateMemoryFunc _fAllocateMemory, ReleaseMemoryFunc _fReleaseMemory, HashKeyMapFunc _fHashKeyMap,
	IsKeyExistsFunc _fIsKeyExists, IsKeyEqualFunc _fIsKeyEqual, EmptyKeyFunc _fEmptyKey):
	ulOrderTable(_ulOrder), fReleaseMemory(_fReleaseMemory), fHashKeyMap(_fHashKeyMap),
	fIsKeyExists(_fIsKeyExists), fIsKeyEqual(_fIsKeyEqual), fEmptyKey(_fEmptyKey)
{
	//计算固定阶数数组内存
	ULONGLONG ullAllocateByte = 0;

	ullAllocateByte += _ulOrder * sizeof(OrderElement<Key, Data>);//先计算层结构数组大小
	_ulStartElement = FindNearPrimeNum(_ulStartElement, 0ULL);//寻找初始层元素的最近质数
	ULONGLONG _ulNextElement = _ulStartElement;

	for (ULONG i = _ulOrder; i > 0; --i)
	{
		ullTotalElement += _ulNextElement;
		ullAllocateByte += _ulNextElement * sizeof(ElementData<Key, Data>);//累加每层元素数
		_ulNextElement = FindNearPrimeNum(_fMultiplication(_ulNextElement), _ulNextElement);//计算上一阶的元素数
	}

	//请求内存
	char* pMemory = (char*)_fAllocateMemory(ullAllocateByte);
	if (!pMemory)
	{
		throw("内存请求失败");
		return;
	}
	ZEROMEMORY(pMemory, ullAllocateByte);//内存置零
	//切分内存
	pstOrderTable = (OrderElement<Key, Data>*)pMemory;
	pMemory += _ulOrder * sizeof(OrderElement<Key, Data>);

	_ulNextElement = _ulStartElement;
	for (ULONG i = _ulOrder; i > 0; --i)
	{
		pstOrderTable[i - 1].ullOrderElement = _ulNextElement;
		pstOrderTable[i - 1].pstOrderElement = (ElementData<Key, Data>*)pMemory;
		pMemory += pstOrderTable[i - 1].ullOrderElement * sizeof(ElementData<Key, Data>);
		_ulNextElement = FindNearPrimeNum(_fMultiplication(_ulNextElement), _ulNextElement);//计算上一阶的元素数
	}

	return;
}

//删除(析构函数)
template<typename Key, typename Data>
MultistageHash<Key, Data>::~MultistageHash(VOID)
{
	fReleaseMemory(pstOrderTable);
	pstOrderTable = NULL;
}

/*多阶哈希操作函数*/

//添加一个键值对
template<typename Key, typename Data>
_BOOL MultistageHash<Key, Data>::AddMultiHash(CONST Key&& tKey, CONST Data&& tData)
{
	ElementAddr<Key, Data> stElementAddr = FindElementDataIsInTheTableOrReturnFirstEmpty((CONST Key&&)tKey);//查找数据是否存在

	if (!stElementAddr.pstOrderElement)//表内无空间
	{
		return false;
	}

	if (!fIsKeyExists((CONST Key&&)stElementAddr.pstOrderElement->tKey))//数据不存在（tKey为空）
	{
		stElementAddr.pstOrderElement->tData = tData;
		stElementAddr.pstOrderElement->tKey = tKey;
		++ullUseTotalElement;//增加总使用个数
		++stElementAddr.pstOrderTable->ullUseOrderElement;//增加层使用个数
	}

	return true;
}

//删除一个键值对（返回tData）
template<typename Key, typename Data>
_BOOL MultistageHash<Key, Data>::DelMultiHash(CONST Key&& tKey, Data** tppReturnData)
{
	ElementAddr<Key, Data> stElementAddr = FindElementDataIsInTheTable((CONST Key&&)tKey);//查找数据是否存在

	if (!stElementAddr.pstOrderElement || !fIsKeyExists((CONST Key&&)stElementAddr.pstOrderElement->tKey))//没找到或为空
	{
		return false;//删除失败
	}

	if (tppReturnData)
	{
		*tppReturnData = stElementAddr.pstOrderElement->tData;
	}

	//stElementAddr.pstOrderElement->tData = NULL;
	//懒惰删除
	fEmptyKey(stElementAddr.pstOrderElement->tKey);
	--ullUseTotalElement;//减少总使用个数
	--stElementAddr.pstOrderTable->ullUseOrderElement;//减少层使用个数

	return true;
}


//修改一个键的值（只修改值）
template<typename Key, typename Data>
_BOOL MultistageHash<Key, Data>::ModifyMultiHashData(CONST Key&& tKey, CONST Data&& tModifyData, Data** tppReturnSourceData)
{
	ElementAddr<Key, Data> stElementAddr = FindElementDataIsInTheTable((CONST Key&&)tKey);

	if (!stElementAddr.pstOrderElement || !fIsKeyExists((CONST Key&&)stElementAddr.pstOrderElement->tKey))//没找到或为空
	{
		return false;//修改失败
	}

	if (tppReturnSourceData)
	{
		*tppReturnSourceData = stElementAddr.pstOrderElement->tData;
	}

	stElementAddr.pstOrderElement->tData = tModifyData;


	return true;
}


//修改一个值的键（重映射键值对）(失败后原键值对不变)
template<typename Key, typename Data>
_BOOL MultistageHash<Key, Data>::ModifyMultiHashKey(CONST Key&& tSourceKey, CONST Key&& tModifyKey)
{
	ElementAddr<Key, Data> stSourceElementAddr = FindElementDataIsInTheTable((CONST Key&&)tSourceKey);//找出原位置

	if (!stSourceElementAddr.pstOrderElement || !fIsKeyExists((CONST Key&&)stSourceElementAddr.pstOrderElement->tKey))//没找到或为空
	{
		return false;//修改失败
	}

	ElementAddr<Key, Data> stModifyElementAddr = FindElementDataIsInTheTableOrReturnFirstEmpty((CONST Key&&)tModifyKey);//找出目标位置

	if (!stModifyElementAddr.pstOrderElement || fIsKeyExists((CONST Key&&)stModifyElementAddr.pstOrderElement->tKey))//没找到或已存在
	{
		return false;//修改失败
	}

	stModifyElementAddr.pstOrderElement->tData = stSourceElementAddr.pstOrderElement->tData;//移动数据
	stModifyElementAddr.pstOrderElement->tKey = tModifyKey;//关联key
	++stModifyElementAddr.pstOrderTable->ullUseOrderElement;//增加统计计数

	//stSourceElementAddr.pstOrderElement->tData = NULL;//删除数据
	//懒惰删除
	fEmptyKey(stSourceElementAddr.pstOrderElement->tKey);//删除key
	--stSourceElementAddr.pstOrderTable->ullUseOrderElement;//减少统计计数

	return true;
}



//查找一个键值对（只能通过键查找值）
template<typename Key, typename Data>
_BOOL MultistageHash<Key, Data>::FindMultiHashData(CONST Key&& tKey, Data*& tReturnData)
{
	ElementAddr<Key, Data> stElementAddr = FindElementDataIsInTheTable((CONST Key&&)tKey);

	if (!stElementAddr.pstOrderElement)
	{
		return false;
	}

	tReturnData = stElementAddr.pstOrderElement->tData;

	return true;
}


//交换两个键值对下的值（只交换值不交换键）
template<typename Key, typename Data>
_BOOL MultistageHash<Key, Data>::SwapMultiHashData(CONST Key&& tKey1, CONST Key&& tKey2)
{
	ElementAddr<Key, Data> stElementAddr1 = FindElementDataIsInTheTable((CONST Key&&)tKey1);
	if (!stElementAddr1.pstOrderElement)
	{
		return false;
	}
	ElementAddr<Key, Data> stElementAddr2 = FindElementDataIsInTheTable((CONST Key&&)tKey2);
	if (!stElementAddr2.pstOrderElement)
	{
		return false;
	}

	Data tDataTemp = stElementAddr1.pstOrderElement->tData;
	stElementAddr1.pstOrderElement->tData = stElementAddr2.pstOrderElement->tData;
	stElementAddr2.pstOrderElement->tData = tDataTemp;

	return true;
}

//清空整张表数据
template<typename Key, typename Data>
VOID MultistageHash<Key, Data>::EmptyMultiHash(VOID)
{
	ZEROMEMORY(pstOrderTable->pstOrderElement, ullTotalElement * sizeof(ElementData<Key, Data>));//清空表元素
	return;
}

#undef CONST
#undef NULL
#undef ZEROMEMORY