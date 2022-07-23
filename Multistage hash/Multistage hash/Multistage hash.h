#pragma once
/*
volatile是一个特征修饰符（type specifier）
volatile的作用是作为指令关键字，确保本条指令不会因编译器的优化而省略，且要求每次直接读值。
volatile的变量是说这变量可能会被意想不到地改变，这样，编译器就不会去假设这个变量的值了。
*/

#define NULL 0

typedef unsigned long long ULONGLONG;
typedef unsigned long ULONG;
typedef bool _BOOL;
typedef void VOID;
#define CONST const

/*

阶结构体数组->阶结构体数组->元素数据结构体数组->键值对

(PORDERELEMENT)pstOrderTable->(ORDERELEMENT) 0 1 2 3 4 5 6 7

(ORDERELEMENT)0->(PELEMENTDATA)->(ELEMENTDATA)0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18
(ORDERELEMENT)1->(PELEMENTDATA)->(ELEMENTDATA)0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
(ORDERELEMENT)2->(PELEMENTDATA)->(ELEMENTDATA)0 1 2 3 4 5 6 7 8 9 10 11 12
(ORDERELEMENT)3->(PELEMENTDATA)->(ELEMENTDATA)0 1 2 3 4 5 6 7 8 9 10
(ORDERELEMENT)4->(PELEMENTDATA)->(ELEMENTDATA)0 1 2 3 4 5 6
(ORDERELEMENT)5->(PELEMENTDATA)->(ELEMENTDATA)0 1 2 3 4
(ORDERELEMENT)6->(PELEMENTDATA)->(ELEMENTDATA)0 1 2
(ORDERELEMENT)7->(PELEMENTDATA)->(ELEMENTDATA)0 1


(ORDERELEMENT)0->(PELEMENTDATA)->(ELEMENTDATA)0
											{	(ELEMENTDATA)0->(VOID*)tData (Key)tKey
												(ELEMENTDATA)1->(VOID*)tData (Key)tKey
																		.
																		.
																		.						}
(ORDERELEMENT)1->(PELEMENTDATA)->(ELEMENTDATA)0
											{	(ELEMENTDATA)0->(VOID*)tData (Key)tKey
												(ELEMENTDATA)1->(VOID*)tData (Key)tKey
																		.
																		.
																		.						}
(ORDERELEMENT)2->(PELEMENTDATA)->(ELEMENTDATA)0
											{
												(ELEMENTDATA)0->(VOID*)tData (Key)tKey
												(ELEMENTDATA)1->(VOID*)tData (Key)tKey
																		.
																		.
																		.						}				
						.
						.
						.
*/

/*多阶哈希结构*/
template<typename Key, typename Data>
class MultistageHash
{
public:
	/*结构体声明&定义*/
	template<typename Key, typename Data>
	struct ElementData//元素数据结构体
	{
		Data tData = 0;//数据指针(哈希值)
		Key tKey = 0;//key(哈希键)与数据一一对应
		//char cState = 0;//结构状态 0:未被使用 1:已被使用 -1:正在修改
	};//typedef ElementData<Key, Data> ELEMENTDATA, * PELEMENTDATA;

	template<typename Key, typename Data>
	struct OrderElement//阶结构体
	{
		ULONGLONG ullOrderElement = 0;//该阶元素数
		ULONGLONG ullUseOrderElement = 0;//已使用的元素数
		ElementData<Key, Data>* pstOrderElement = NULL;//数据结构体数组地址
	};//typedef OrderElement<Key, Data> ORDERELEMENT, * PORDERELEMENT;
	
	template<typename Key, typename Data>
	struct ElementAddr//元素寻址
	{
		OrderElement<Key, Data>* pstOrderTable = NULL;//阶层指针
		ElementData<Key, Data>* pstOrderElement = NULL;//数据索引结构
	};//typedef ElementAddr<Key, Data> ELEMENTADDR, * PELEMENTADDR;
public:
	/*函数指针声明*/
	typedef ULONGLONG(*MultiplicationFunc)(ULONGLONG ullLast);//阶元素数倍增函数
	typedef VOID* (*AllocateMemoryFunc)(ULONGLONG ullByte);//分配内存函数
	typedef VOID (*ReleaseMemoryFunc)(VOID* pMemory);//内存释放函数
	typedef ULONGLONG(*HashKeyMapFunc)(CONST Key&& tKey);//哈希键映射函数
	typedef _BOOL(*IsKeyEqualFunc)(CONST Key&& tKey1, CONST Key&& tKey2);//判断两个key是否相等
	typedef _BOOL(*IsKeyExistsFunc)(CONST Key&& tKey);//判断Key是否为空
	typedef VOID(*EmptyKeyFunc)(Key& tKey);//将Key置空
private:
	ULONGLONG ullTotalElement = 0;//总元素数
	ULONGLONG ullUseTotalElement = 0;//已使用的元素数

	ULONG ulOrderTable = 0;//阶数
	OrderElement<Key, Data>* pstOrderTable = NULL;//阶结构体数组地址

	ReleaseMemoryFunc fReleaseMemory = NULL;//释放函数(析构函数使用)
	HashKeyMapFunc fHashKeyMap = NULL;//哈希键映射函数，插入函数使用
	IsKeyEqualFunc fIsKeyEqual = NULL;//键相等
	IsKeyExistsFunc fIsKeyExists = NULL;//键为空
	EmptyKeyFunc fEmptyKey = NULL;//置空键
private:
	/*质数操作函数*/
	_BOOL inline IsPrimeNum(ULONGLONG n) CONST;
	ULONGLONG inline FindNearPrimeNum(ULONGLONG n, ULONGLONG l) CONST;
	/*多阶哈希辅助函数*/
	ElementAddr<Key, Data> inline FindElementDataIsInTheTableOrReturnFirstEmpty(CONST Key&& tKey) CONST;
	ElementAddr<Key, Data> inline FindElementDataIsInTheTable(CONST Key&& tKey) CONST;
public:
	/*初始化和反向初始化*/
	MultistageHash(ULONGLONG _ulStartElement, ULONG _ulOrder, MultiplicationFunc _fMultiplication,
		AllocateMemoryFunc _fAllocateMemory, ReleaseMemoryFunc _fReleaseMemory, HashKeyMapFunc _fHashKeyMap,
		IsKeyExistsFunc _fIsKeyExists, IsKeyEqualFunc _fIsKeyEqual, EmptyKeyFunc _fEmptyKey);
	~MultistageHash(VOID);

	/*禁用函数列表*/
	MultistageHash operator=(MultistageHash) = delete;
	MultistageHash& operator=(MultistageHash&) = delete;
	MultistageHash(MultistageHash&) = delete;
	MultistageHash(MultistageHash&&) = delete;

	/*统计数据反馈函数*/
	ULONG GetOrderTableLayers(VOID) CONST;//获取层数

	ULONGLONG GetTotalElement(VOID) CONST;//获取整张表最大可容纳元素数
	ULONGLONG GetUseTotalElement(VOID) CONST;//获取整张表已容纳元素数

	ULONGLONG GetOrderElement(ULONG ulLayer) CONST;//获取某一层最大可容纳元素数
	ULONGLONG GetUseOrderElement(ULONG ulLayer) CONST;//获取某一层已容纳元素数

	/*多阶哈希操作函数*/
	_BOOL AddMultiHash(CONST Key&& tKey, CONST Data&& tData);//添加一个键值对
	_BOOL AddMultiHash(CONST Key& tKey, CONST Data& tData)
	{
		return AddMultiHash((CONST Key&&)tKey, (CONST Data&&)tData);
	}

	_BOOL DelMultiHash(CONST Key&& tKey, Data** tppReturnData);//删除一个键值对
	_BOOL DelMultiHash(CONST Key& tKey, Data** tppReturnData)
	{
		return DelMultiHash((CONST Key&&)tKey, (Data**)tppReturnData);
	}

	_BOOL ModifyMultiHashData(CONST Key&& tKey, CONST Data&& tModifyData, Data** tppReturnSourceData);//修改一个键值对下的值
	_BOOL ModifyMultiHashData(CONST Key& tKey, CONST Data& tModifyData, Data** tppReturnSourceData)
	{
		return ModifyMultiHashData((CONST Key&&)tKey, (CONST Data&&)tModifyData, (Data**)tppReturnSourceData);
	}

	_BOOL ModifyMultiHashKey(CONST Key&& tSourceKey, CONST Key&& tModifyKey);//修改一个键值对的键:重映射键值对
	_BOOL ModifyMultiHashKey(CONST Key& tSourceKey, CONST Key& tModifyKey)
	{
		return ModifyMultiHashKey((CONST Key&&)tSourceKey, (CONST Key&&)tModifyKey);
	}

	_BOOL FindMultiHashData(CONST Key&& tKey, Data*& tReturnData);//查找一个键下的值
	_BOOL FindMultiHashData(CONST Key& tKey, Data*& tReturnData)
	{
		return FindMultiHashData((CONST Key&&)tKey, (Data*&)tReturnData);
	}

	_BOOL SwapMultiHashData(CONST Key&& tKey1, CONST Key&& tKey2);//交换两个键下的值
	_BOOL SwapMultiHashData(CONST Key& tKey1, CONST Key& tKey2)
	{
		return SwapMultiHashData((CONST Key&&)tKey1, (CONST Key&&)tKey2);
	}

	VOID EmptyMultiHash(VOID);//清空整张表
};

#undef CONST
#undef NULL