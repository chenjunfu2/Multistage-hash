#pragma once
/*
volatile��һ���������η���type specifier��
volatile����������Ϊָ��ؼ��֣�ȷ������ָ�������������Ż���ʡ�ԣ���Ҫ��ÿ��ֱ�Ӷ�ֵ��
volatile�ı�����˵��������ܻᱻ���벻���ظı䣬�������������Ͳ���ȥ�������������ֵ�ˡ�
*/

#define NULL 0

typedef unsigned long long ULONGLONG;
typedef unsigned long ULONG;
typedef bool _BOOL;
typedef void VOID;

/*

�������ṹ(�ṹ����)->�������ṹ(�ṹ����)->���������ṹ(����ָ��)->����

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

/*��׹�ϣ�ṹ*/
class MultistageHash
{
protected:
	/*�ṹ������&����*/
	struct ElementData//Ԫ�����ݽṹ��
	{
		VOID* pData = NULL;//����ָ��(��ϣֵ)
		ULONGLONG ullKey = 0;//key(��ϣ��)������һһ��Ӧ
		//char cState = 0;//�ṹ״̬ 0:δ��ʹ�� 1:�ѱ�ʹ�� -1:�����޸�
	};
public:
	typedef ElementData ELEMENTDATA, * PELEMENTDATA;
protected:
	struct OrderElement//�׽ṹ��
	{
		ULONGLONG ullOrderElement = 0;//�ý�Ԫ����
		ULONGLONG ullUseOrderElement = 0;//��ʹ�õ�Ԫ����
		PELEMENTDATA pstOrderElement = NULL;//���ݽṹ�������ַ
	};
public:
	typedef OrderElement ORDERELEMENT, * PORDERELEMENT;
protected:
	struct ElementAddr//Ԫ��Ѱַ
	{
		PORDERELEMENT pstOrderTable = NULL;//�ײ�ָ��
		PELEMENTDATA pstOrderElement = NULL;//���������ṹ
	};
public:
	typedef ElementAddr ELEMENTADDR, * PELEMENTADDR;
public:
	/*����ָ������*/
	typedef ULONGLONG(*MultiplicationFunc)(ULONGLONG ullLast);//��Ԫ������������
	typedef VOID* (*AllocateMemoryFunc)(ULONGLONG ullByte);//�����ڴ溯��
	typedef VOID (*ReleaseMemoryFunc)(VOID* pMemory);//�����ͷź���
	typedef ULONGLONG(*HashKeyMapFunc)(ULONGLONG ullKey);//��ϣ��ӳ�亯��
	typedef _BOOL(*TraversalUseRateFunc)(ULONGLONG ullRate,ULONGLONG ullUseRate, ULONG ulCurrent);//����ÿ���ʹ����
private:
	ULONGLONG ullTotalElement = 0;//��Ԫ����
	ULONGLONG ullUseTotalElement = 0;//��ʹ�õ�Ԫ����

	ULONG ulOrderTable = 0;//����
	PORDERELEMENT pstOrderTable = NULL;//�׽ṹ�������ַ

	ReleaseMemoryFunc fReleaseMemory = NULL;//�ͷź���(��������ʹ��)
	HashKeyMapFunc fHashKeyMap = NULL;//��ϣ��ӳ�亯�������뺯��ʹ��
private:
	/*������������*/
	_BOOL inline IsPrimeNum(ULONGLONG n);
	ULONGLONG inline FindNearPrimeNum(ULONGLONG n, ULONGLONG l);
	/*��׹�ϣ��������*/
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


