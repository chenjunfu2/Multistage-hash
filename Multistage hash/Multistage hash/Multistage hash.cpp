#include <string.h>
#include <math.h>
#include "Multistage hash.h"

#define IfThrow(index) if(!index) throw(index);
#define ZEROMEMORY(mem, size) memset(mem, 0, size)
#define NULL 0

//double my_sqrt(double x)
//{
//	register double ret;
//	_asm
//	{
//		//fsqrt
//		//=t ret
//		//0 x
//	}
//
//}

/*质数操作函数*/
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
_BOOL MultistageHash::GetOrderUseRate(TraversalUseRateFunc fTraversalUseRate)
{
	_BOOL b = true;
	for (ULONG i = 0; i < ulOrderTable && b; ++i)
	{
		b = fTraversalUseRate(pstOrderTable[i].ullOrderElement, pstOrderTable[i].ullUseOrderElement, i);
	}

	return b;
}

_BOOL MultistageHash::GetTotalUseRate(TraversalUseRateFunc fTraversalUseRate)
{
	return fTraversalUseRate(ullTotalElement, ullUseTotalElement, ulOrderTable);
}

/*多阶哈希辅助函数*/
MultistageHash::ELEMENTADDR inline MultistageHash::FindElementDataIsInTheTableAndReturnEmpty(ULONGLONG ullKeyMap, ULONGLONG ullKey)
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



/*多阶哈希操作函数*/
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


MultistageHash::~MultistageHash()
{
	fReleaseMemory(pstOrderTable);
	pstOrderTable = NULL;
}

//添加一个键值对
_BOOL MultistageHash::AddMultiHash(ULONGLONG ullKey, VOID* pData)
{
	if (!pData)
		return false;
	
	ELEMENTADDR stElementAddr = FindElementDataIsInTheTableAndReturnEmpty(fHashKeyMap(ullKey), ullKey);//查找数据是否存在

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

	ELEMENTADDR stModifyElementAddr = FindElementDataIsInTheTableAndReturnEmpty(fHashKeyMap(ullModifyKey), ullModifyKey);//找出目标位置

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
//输入一个键和查找的键值索引结构体指针（为null不返回）
//返回查找元素的值（为null则代表失败）
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
//输入两个键或两个键值索引结构指针和一个元素结构（返回第一个元素结构）
//返回第二个元素结构








//获取所有已使用键值对（用所有键值对做参调用给定函数）(分为正向遍历和反向遍历)
//输入一个函数指针和一个正向/方向遍历的布尔值，对于每个键值对都调用函数（多次调用的函数一旦返回false立刻结束调用并返回）
//返回一个布尔值，如果多次调用函数返回false就返回false，否则全部遍历完成返回true








#undef NULL
#undef ZEROMEMORY
#undef IfThrow