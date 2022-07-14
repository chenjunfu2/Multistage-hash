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

/*多阶哈希结构*/
class MultistageHash
{
protected:
	/*结构体声明&定义*/
	struct ElementData//元素数据结构体
	{
		VOID* pData = NULL;//数据指针(哈希值)
		ULONGLONG ullKey = 0;//key(哈希键)与数据一一对应
		//char cState = 0;//结构状态 0:未被使用 1:已被使用 -1:正在修改
	};
public:
	typedef ElementData ELEMENTDATA, * PELEMENTDATA;
protected:
	struct OrderElement//阶结构体
	{
		ULONGLONG ullOrderElement = 0;//该阶元素数
		ULONGLONG ullUseOrderElement = 0;//已使用的元素数
		PELEMENTDATA pstOrderElement = NULL;//数据结构体数组地址
	};
public:
	typedef OrderElement ORDERELEMENT, * PORDERELEMENT;
protected:
	struct ElementAddr//元素寻址
	{
		PORDERELEMENT pstOrderTable = NULL;//阶层指针
		PELEMENTDATA pstOrderElement = NULL;//数据索引结构
	};
public:
	typedef ElementAddr ELEMENTADDR, * PELEMENTADDR;
public:
	/*函数指针声明*/
	typedef ULONGLONG(*MultiplicationFunc)(ULONGLONG ullLast);//阶元素数倍增函数
	typedef VOID* (*AllocateMemoryFunc)(ULONGLONG ullByte);//分配内存函数
	typedef VOID (*ReleaseMemoryFunc)(VOID* pMemory);//分配释放函数
	typedef ULONGLONG(*HashKeyMapFunc)(ULONGLONG ullKey);//哈希键映射函数
	typedef _BOOL(*TraversalUseRateFunc)(ULONGLONG ullRate,ULONGLONG ullUseRate, ULONG ulCurrent);//遍历每层的使用率
private:
	ULONGLONG ullTotalElement = 0;//总元素数
	ULONGLONG ullUseTotalElement = 0;//已使用的元素数

	ULONG ulOrderTable = 0;//阶数
	PORDERELEMENT pstOrderTable = NULL;//阶结构体数组地址

	ReleaseMemoryFunc fReleaseMemory = NULL;//释放函数(析构函数使用)
	HashKeyMapFunc fHashKeyMap = NULL;//哈希键映射函数，插入函数使用
private:
	/*质数操作函数*/
	_BOOL inline IsPrimeNum(ULONGLONG n);
	ULONGLONG inline FindNearPrimeNum(ULONGLONG n, ULONGLONG l);
	/*多阶哈希辅助函数*/
	ELEMENTADDR inline FindElementDataIsInTheTableAndReturnEmpty(ULONGLONG ullKeyMap, ULONGLONG ullKey);
	ELEMENTADDR inline FindElementDataIsInTheTable(ULONGLONG ullKeyMap, ULONGLONG ullKey);

public:
	MultistageHash(ULONGLONG _ulStartElement, ULONG _ulOrder, MultiplicationFunc _fMultiplication,
		AllocateMemoryFunc _fAllocateMemory, ReleaseMemoryFunc _fReleaseMemory, HashKeyMapFunc _fHashKeyMap);
	~MultistageHash(VOID);

	MultistageHash operator=(MultistageHash) = delete;
	MultistageHash& operator=(MultistageHash&) = delete;
	MultistageHash(MultistageHash&) = delete;
	MultistageHash(MultistageHash&&) = delete;


	_BOOL GetOrderUseRate(TraversalUseRateFunc fTraversalUseRate);
	_BOOL GetTotalUseRate(TraversalUseRateFunc fTraversalUseRate);

	_BOOL AddMultiHash(ULONGLONG ullKey, VOID* pData);
	_BOOL DelMultiHash(ULONGLONG ullKey, VOID** ppReturnData);

	_BOOL ModifyMultiHashData(ULONGLONG ullKey, VOID* pModifyData, VOID** ppReturnSourceData);
	_BOOL ModifyMultiHashKey(ULONGLONG ullSourceKey, ULONGLONG ullModifyKey);

	_BOOL FindMultiHashData(ULONGLONG ullKey, VOID** ppReturnData);

};

#undef NULL


