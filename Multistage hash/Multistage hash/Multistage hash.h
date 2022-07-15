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

�׽ṹ������->�׽ṹ������->Ԫ�����ݽṹ������->��ֵ��

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
											{	(ELEMENTDATA)0->(VOID*)pData (ULONGLONG)ullKey
												(ELEMENTDATA)1->(VOID*)pData (ULONGLONG)ullKey
																		.
																		.
																		.						}
(ORDERELEMENT)1->(PELEMENTDATA)->(ELEMENTDATA)0
											{	(ELEMENTDATA)0->(VOID*)pData (ULONGLONG)ullKey
												(ELEMENTDATA)1->(VOID*)pData (ULONGLONG)ullKey
																		.
																		.
																		.						}
(ORDERELEMENT)2->(PELEMENTDATA)->(ELEMENTDATA)0
											{
												(ELEMENTDATA)0->(VOID*)pData (ULONGLONG)ullKey
												(ELEMENTDATA)1->(VOID*)pData (ULONGLONG)ullKey
																		.
																		.
																		.						}				
						.
						.
						.
*/

/*��׹�ϣ�ṹ*/
class MultistageHash
{
public:
	/*�ṹ������&����*/
	typedef struct ElementData//Ԫ�����ݽṹ��
	{
		VOID* pData = NULL;//����ָ��(��ϣֵ)
		ULONGLONG ullKey = 0;//key(��ϣ��)������һһ��Ӧ
		//char cState = 0;//�ṹ״̬ 0:δ��ʹ�� 1:�ѱ�ʹ�� -1:�����޸�
	}ELEMENTDATA, * PELEMENTDATA;
	typedef struct OrderElement//�׽ṹ��
	{
		ULONGLONG ullOrderElement = 0;//�ý�Ԫ����
		ULONGLONG ullUseOrderElement = 0;//��ʹ�õ�Ԫ����
		PELEMENTDATA pstOrderElement = NULL;//���ݽṹ�������ַ
	}ORDERELEMENT, * PORDERELEMENT; 
	typedef struct ElementAddr//Ԫ��Ѱַ
	{
		PORDERELEMENT pstOrderTable = NULL;//�ײ�ָ��
		PELEMENTDATA pstOrderElement = NULL;//���������ṹ
	}ELEMENTADDR, * PELEMENTADDR;
public:
	/*����ָ������*/
	typedef ULONGLONG(*MultiplicationFunc)(ULONGLONG ullLast);//��Ԫ������������
	typedef VOID* (*AllocateMemoryFunc)(ULONGLONG ullByte);//�����ڴ溯��
	typedef VOID (*ReleaseMemoryFunc)(VOID* pMemory);//�����ͷź���
	typedef ULONGLONG(*HashKeyMapFunc)(ULONGLONG ullKey);//��ϣ��ӳ�亯��
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
	ELEMENTADDR inline FindElementDataIsInTheTableOrReturnFirstEmpty(ULONGLONG ullKeyMap, ULONGLONG ullKey);
	ELEMENTADDR inline FindElementDataIsInTheTable(ULONGLONG ullKeyMap, ULONGLONG ullKey);
public:
	/*��ʼ���ͷ����ʼ��*/
	MultistageHash(ULONGLONG _ulStartElement, ULONG _ulOrder, MultiplicationFunc _fMultiplication,
		AllocateMemoryFunc _fAllocateMemory, ReleaseMemoryFunc _fReleaseMemory, HashKeyMapFunc _fHashKeyMap);
	~MultistageHash(VOID);

	/*���ú����б�*/
	MultistageHash operator=(MultistageHash) = delete;
	MultistageHash& operator=(MultistageHash&) = delete;
	MultistageHash(MultistageHash&) = delete;
	MultistageHash(MultistageHash&&) = delete;

	/*ͳ�����ݷ�������*/
	ULONG GetOrderTableLayers(VOID);//��ȡ����

	ULONGLONG GetTotalElement(VOID);//��ȡ���ű���������Ԫ����
	ULONGLONG GetUseTotalElement(VOID);//��ȡ���ű�������Ԫ����

	ULONGLONG GetOrderElement(ULONG ulLayer);//��ȡĳһ����������Ԫ����
	ULONGLONG GetUseOrderElement(ULONG ulLayer);//��ȡĳһ��������Ԫ����

	/*��׹�ϣ��������*/
	_BOOL AddMultiHash(ULONGLONG ullKey, VOID* pData);//���һ����ֵ��
	_BOOL DelMultiHash(ULONGLONG ullKey, VOID** ppReturnData);//ɾ��һ����ֵ��

	_BOOL ModifyMultiHashData(ULONGLONG ullKey, VOID* pModifyData, VOID** ppReturnSourceData);//�޸�һ����ֵ���µ�ֵ
	_BOOL ModifyMultiHashKey(ULONGLONG ullSourceKey, ULONGLONG ullModifyKey);//�޸�һ����ֵ�Եļ�:��ӳ���ֵ��

	_BOOL FindMultiHashData(ULONGLONG ullKey, VOID** ppReturnData);//����һ�����µ�ֵ
	_BOOL SwapMultiHashData(ULONGLONG ullKey1, ULONGLONG ullKey2);//�����������µ�ֵ

	VOID EmptyMultiHash(VOID);//������ű�

	PELEMENTDATA begin(VOID);//���ű��ֵ������
	PELEMENTDATA end(VOID);//���ű��ֵ������
};

#undef NULL