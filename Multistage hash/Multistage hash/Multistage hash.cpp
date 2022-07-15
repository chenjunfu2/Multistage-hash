#include "Multistage hash.h"//class state
#include <string.h>//using memset function
#include <math.h>//using sqrt function

#define IfThrow(index) if(!index) throw(index);
#define ZEROMEMORY(mem, size) memset(mem, 0, size)
#define NULL 0

/*质数操作函数*/

//质数判定
_BOOL inline MultistageHash::IsPrimeNum(ULONGLONG n)
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
ULONGLONG inline MultistageHash::FindNearPrimeNum(ULONGLONG n, ULONGLONG l)
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
ULONG MultistageHash::GetOrderTableLayers(VOID)
{
	return ulOrderTable;
}

//获取整张表最大可容纳元素数
ULONGLONG MultistageHash::GetTotalElement(VOID)
{
	return ullTotalElement;
}

//获取整张表已容纳元素数
ULONGLONG MultistageHash::GetUseTotalElement(VOID)
{
	return ullUseTotalElement;
}

//获取某一层最大可容纳元素数
ULONGLONG MultistageHash::GetOrderElement(ULONG ulLayer)
{
	return pstOrderTable[ulLayer].ullOrderElement;
}

//获取某一层已容纳元素数
ULONGLONG MultistageHash::GetUseOrderElement(ULONG ulLayer)
{
	return pstOrderTable[ulLayer].ullUseOrderElement;
}

/*多阶哈希辅助函数*/

//查找一个键是否存在于哈希表中如果不存在则返回第一个空位
MultistageHash::ELEMENTADDR inline MultistageHash::FindElementDataIsInTheTableOrReturnFirstEmpty(ULONGLONG ullKeyMap, ULONGLONG ullKey)
{
	ELEMENTADDR stElementAddr = { 0 };

	for (PORDERELEMENT pstLoop = pstOrderTable; pstLoop < pstOrderTable + ulOrderTable; ++pstLoop)
	{
		PELEMENTDATA pstElementData = &pstLoop->pstOrderElement[ullKeyMap % pstLoop->ullOrderElement];

		if (!pstElementData->pData)//此处没有数据
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
			if (pstElementData->ullKey == ullKey)//键相等
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
MultistageHash::ELEMENTADDR inline MultistageHash::FindElementDataIsInTheTable(ULONGLONG ullKeyMap, ULONGLONG ullKey)
{
	ELEMENTADDR stElementAddr = { 0 };

	for (PORDERELEMENT pstLoop = pstOrderTable; pstLoop < pstOrderTable + ulOrderTable; ++pstLoop)
	{
		PELEMENTDATA pstElementData = &pstLoop->pstOrderElement[ullKeyMap % pstLoop->ullOrderElement];

		if (pstElementData->pData && pstElementData->ullKey == ullKey)//找到数据并且键相等
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
MultistageHash::MultistageHash(ULONGLONG _ulStartElement, ULONG _ulOrder, MultiplicationFunc _fMultiplication,
	AllocateMemoryFunc _fAllocateMemory, ReleaseMemoryFunc _fReleaseMemory, HashKeyMapFunc _fHashKeyMap) :
	ulOrderTable(_ulOrder), fReleaseMemory(_fReleaseMemory), fHashKeyMap(_fHashKeyMap)
{
	IfThrow(ulOrderTable);
	IfThrow(_fMultiplication);
	IfThrow(_fAllocateMemory);
	IfThrow(fReleaseMemory);
	IfThrow(fHashKeyMap);

	//计算固定阶数数组内存
	ULONGLONG ullAllocateByte = 0;

	ullAllocateByte += _ulOrder * sizeof(ORDERELEMENT);//先计算层结构数组大小
	_ulStartElement = FindNearPrimeNum(_ulStartElement, 0ULL);//寻找初始层元素的最近质数
	ULONGLONG _ulNextElement = _ulStartElement;

	for (ULONG i = _ulOrder; i > 0; --i)
	{
		ullTotalElement += _ulNextElement;
		ullAllocateByte += _ulNextElement * sizeof(ELEMENTDATA);//累加每层元素数
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
	pstOrderTable = (PORDERELEMENT)pMemory;
	pMemory += _ulOrder * sizeof(ORDERELEMENT);

	_ulNextElement = _ulStartElement;
	for (ULONG i = _ulOrder; i > 0; --i)
	{
		pstOrderTable[i - 1].ullOrderElement = _ulNextElement;
		pstOrderTable[i - 1].pstOrderElement = (PELEMENTDATA)pMemory;
		pMemory += pstOrderTable[i - 1].ullOrderElement * sizeof(ELEMENTDATA);
		_ulNextElement = FindNearPrimeNum(_fMultiplication(_ulNextElement), _ulNextElement);//计算上一阶的元素数
	}

	return;
}

//删除(析构函数)
MultistageHash::~MultistageHash(VOID)
{
	fReleaseMemory(pstOrderTable);
	pstOrderTable = NULL;
}

/*多阶哈希操作函数*/

//添加一个键值对
_BOOL MultistageHash::AddMultiHash(ULONGLONG ullKey, VOID* pData)
{
	if (!pData)
		return false;
	
	ELEMENTADDR stElementAddr = FindElementDataIsInTheTableOrReturnFirstEmpty(fHashKeyMap(ullKey), ullKey);//查找数据是否存在

	if (!stElementAddr.pstOrderElement)//表内无空间
	{
		return false;
	}

	if (!stElementAddr.pstOrderElement->pData)//数据不存在（pData为NULL）
	{
		stElementAddr.pstOrderElement->pData = pData;
		stElementAddr.pstOrderElement->ullKey = ullKey;
		++ullUseTotalElement;//增加总使用个数
		++stElementAddr.pstOrderTable->ullUseOrderElement;//增加层使用个数
	}

	return true;
}

//删除一个键值对（返回pData）
_BOOL MultistageHash::DelMultiHash(ULONGLONG ullKey, VOID** ppReturnData)
{
	ELEMENTADDR stElementAddr = FindElementDataIsInTheTable(fHashKeyMap(ullKey), ullKey);//查找数据是否存在

	if (!stElementAddr.pstOrderElement || !stElementAddr.pstOrderElement->pData)//没找到或数据为空
	{
		return false;//删除失败
	}

	if (ppReturnData)
	{
		*ppReturnData = stElementAddr.pstOrderElement->pData;
	}

	stElementAddr.pstOrderElement->pData = NULL;
	stElementAddr.pstOrderElement->ullKey = 0;
	--ullUseTotalElement;//减少总使用个数
	--stElementAddr.pstOrderTable->ullUseOrderElement;//减少层使用个数

	return true;
}


//修改一个键的值（只修改值）
_BOOL MultistageHash::ModifyMultiHashData(ULONGLONG ullKey, VOID* pModifyData, VOID** ppReturnSourceData)
{
	ELEMENTADDR stElementAddr = FindElementDataIsInTheTable(fHashKeyMap(ullKey), ullKey);

	if (!stElementAddr.pstOrderElement || !stElementAddr.pstOrderElement->pData)//没找到或数据为空
	{
		return false;//修改失败
	}

	if (ppReturnSourceData)
	{
		*ppReturnSourceData = stElementAddr.pstOrderElement->pData;
	}

	stElementAddr.pstOrderElement->pData = pModifyData;


	return true;
}


//修改一个值的键（重映射键值对）(失败后原键值对不变)
_BOOL MultistageHash::ModifyMultiHashKey(ULONGLONG ullSourceKey, ULONGLONG ullModifyKey)
{
	ELEMENTADDR stSourceElementAddr = FindElementDataIsInTheTable(fHashKeyMap(ullSourceKey), ullSourceKey);//找出原位置

	if (!stSourceElementAddr.pstOrderElement || !stSourceElementAddr.pstOrderElement->pData)//没找到或数据为空
	{
		return false;//修改失败
	}

	ELEMENTADDR stModifyElementAddr = FindElementDataIsInTheTableOrReturnFirstEmpty(fHashKeyMap(ullModifyKey), ullModifyKey);//找出目标位置

	if (!stModifyElementAddr.pstOrderElement || stModifyElementAddr.pstOrderElement->pData)//没找到或已有数据
	{
		return false;//修改失败
	}

	stModifyElementAddr.pstOrderElement->pData = stSourceElementAddr.pstOrderElement->pData;//移动数据
	stModifyElementAddr.pstOrderElement->ullKey = ullModifyKey;//关联key
	++stModifyElementAddr.pstOrderTable->ullUseOrderElement;//增加统计计数

	stSourceElementAddr.pstOrderElement->pData = NULL;//删除数据
	stSourceElementAddr.pstOrderElement->ullKey = 0;//删除key
	--stSourceElementAddr.pstOrderTable->ullUseOrderElement;//减少统计计数

	return true;
}



//查找一个键值对（只能通过键查找值）
_BOOL MultistageHash::FindMultiHashData(ULONGLONG ullKey, VOID** ppReturnData)
{
	if (!ppReturnData)
	{
		return false;
	}

	ELEMENTADDR stElementAddr = FindElementDataIsInTheTable(fHashKeyMap(ullKey), ullKey);

	if (!stElementAddr.pstOrderElement)
	{
		return false;
	}

	*ppReturnData = stElementAddr.pstOrderElement->pData;

	return true;
}


//交换两个键值对下的值（只交换值不交换键）
_BOOL MultistageHash::SwapMultiHashData(ULONGLONG ullKey1, ULONGLONG ullKey2)
{
	ELEMENTADDR stElementAddr1 = FindElementDataIsInTheTable(fHashKeyMap(ullKey1), ullKey1);
	if (!stElementAddr1.pstOrderElement)
	{
		return false;
	}
	ELEMENTADDR stElementAddr2 = FindElementDataIsInTheTable(fHashKeyMap(ullKey2), ullKey2);
	if (!stElementAddr2.pstOrderElement)
	{
		return false;
	}

	register void* rpDataTemp = stElementAddr1.pstOrderElement->pData;
	stElementAddr1.pstOrderElement->pData = stElementAddr2.pstOrderElement->pData;
	stElementAddr2.pstOrderElement->pData = rpDataTemp;

	return true;
}

//获取所有键值对的迭代器
MultistageHash::PELEMENTDATA MultistageHash::begin(VOID)
{
	return pstOrderTable[0].pstOrderElement;
}

MultistageHash::PELEMENTDATA MultistageHash::end(VOID)
{
	return pstOrderTable[ulOrderTable].pstOrderElement + pstOrderTable[ulOrderTable].ullOrderElement + 1;
}

//清空整张表数据
VOID MultistageHash::EmptyMultiHash(VOID)
{
	for (ULONG i = 0; i < ulOrderTable; ++i)
	{
		ZEROMEMORY(pstOrderTable[i].pstOrderElement, pstOrderTable[i].ullOrderElement * sizeof(ELEMENTDATA));//清空表元素
		pstOrderTable[i].ullUseOrderElement = 0ULL;
	}
	return;
}

#undef NULL
#undef ZEROMEMORY
#undef IfThrow