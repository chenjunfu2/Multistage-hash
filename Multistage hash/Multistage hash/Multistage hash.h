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
#define CONST const

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

/*��׹�ϣ�ṹ*/
template<typename Key, typename Data>
class MultistageHash
{
public:
	/*�ṹ������&����*/
	template<typename Key, typename Data>
	struct ElementData//Ԫ�����ݽṹ��
	{
		Data tData = 0;//����ָ��(��ϣֵ)
		Key tKey = 0;//key(��ϣ��)������һһ��Ӧ
		//char cState = 0;//�ṹ״̬ 0:δ��ʹ�� 1:�ѱ�ʹ�� -1:�����޸�
	};//typedef ElementData<Key, Data> ELEMENTDATA, * PELEMENTDATA;

	template<typename Key, typename Data>
	struct OrderElement//�׽ṹ��
	{
		ULONGLONG ullOrderElement = 0;//�ý�Ԫ����
		ULONGLONG ullUseOrderElement = 0;//��ʹ�õ�Ԫ����
		ElementData<Key, Data>* pstOrderElement = NULL;//���ݽṹ�������ַ
	};//typedef OrderElement<Key, Data> ORDERELEMENT, * PORDERELEMENT;
	
	template<typename Key, typename Data>
	struct ElementAddr//Ԫ��Ѱַ
	{
		OrderElement<Key, Data>* pstOrderTable = NULL;//�ײ�ָ��
		ElementData<Key, Data>* pstOrderElement = NULL;//���������ṹ
	};//typedef ElementAddr<Key, Data> ELEMENTADDR, * PELEMENTADDR;
public:
	/*����ָ������*/
	typedef ULONGLONG(*MultiplicationFunc)(ULONGLONG ullLast);//��Ԫ������������
	typedef VOID* (*AllocateMemoryFunc)(ULONGLONG ullByte);//�����ڴ溯��
	typedef VOID (*ReleaseMemoryFunc)(VOID* pMemory);//�ڴ��ͷź���
	typedef ULONGLONG(*HashKeyMapFunc)(CONST Key&& tKey);//��ϣ��ӳ�亯��
	typedef _BOOL(*IsKeyEqualFunc)(CONST Key&& tKey1, CONST Key&& tKey2);//�ж�����key�Ƿ����
	typedef _BOOL(*IsKeyExistsFunc)(CONST Key&& tKey);//�ж�Key�Ƿ�Ϊ��
	typedef VOID(*EmptyKeyFunc)(Key& tKey);//��Key�ÿ�
private:
	ULONGLONG ullTotalElement = 0;//��Ԫ����
	ULONGLONG ullUseTotalElement = 0;//��ʹ�õ�Ԫ����

	ULONG ulOrderTable = 0;//����
	OrderElement<Key, Data>* pstOrderTable = NULL;//�׽ṹ�������ַ

	ReleaseMemoryFunc fReleaseMemory = NULL;//�ͷź���(��������ʹ��)
	HashKeyMapFunc fHashKeyMap = NULL;//��ϣ��ӳ�亯�������뺯��ʹ��
	IsKeyEqualFunc fIsKeyEqual = NULL;//�����
	IsKeyExistsFunc fIsKeyExists = NULL;//��Ϊ��
	EmptyKeyFunc fEmptyKey = NULL;//�ÿռ�
private:
	/*������������*/
	_BOOL inline IsPrimeNum(ULONGLONG n) CONST;
	ULONGLONG inline FindNearPrimeNum(ULONGLONG n, ULONGLONG l) CONST;
	/*��׹�ϣ��������*/
	ElementAddr<Key, Data> inline FindElementDataIsInTheTableOrReturnFirstEmpty(CONST Key&& tKey) CONST;
	ElementAddr<Key, Data> inline FindElementDataIsInTheTable(CONST Key&& tKey) CONST;
public:
	/*��ʼ���ͷ����ʼ��*/
	MultistageHash(ULONGLONG _ulStartElement, ULONG _ulOrder, MultiplicationFunc _fMultiplication,
		AllocateMemoryFunc _fAllocateMemory, ReleaseMemoryFunc _fReleaseMemory, HashKeyMapFunc _fHashKeyMap,
		IsKeyExistsFunc _fIsKeyExists, IsKeyEqualFunc _fIsKeyEqual, EmptyKeyFunc _fEmptyKey);
	~MultistageHash(VOID);

	/*���ú����б�*/
	MultistageHash operator=(MultistageHash) = delete;
	MultistageHash& operator=(MultistageHash&) = delete;
	MultistageHash(MultistageHash&) = delete;
	MultistageHash(MultistageHash&&) = delete;

	/*ͳ�����ݷ�������*/
	ULONG GetOrderTableLayers(VOID) CONST;//��ȡ����

	ULONGLONG GetTotalElement(VOID) CONST;//��ȡ���ű���������Ԫ����
	ULONGLONG GetUseTotalElement(VOID) CONST;//��ȡ���ű�������Ԫ����

	ULONGLONG GetOrderElement(ULONG ulLayer) CONST;//��ȡĳһ����������Ԫ����
	ULONGLONG GetUseOrderElement(ULONG ulLayer) CONST;//��ȡĳһ��������Ԫ����

	/*��׹�ϣ��������*/
	_BOOL AddMultiHash(CONST Key&& tKey, CONST Data&& tData);//���һ����ֵ��
	_BOOL AddMultiHash(CONST Key& tKey, CONST Data& tData)
	{
		return AddMultiHash((CONST Key&&)tKey, (CONST Data&&)tData);
	}

	_BOOL DelMultiHash(CONST Key&& tKey, Data** tppReturnData);//ɾ��һ����ֵ��
	_BOOL DelMultiHash(CONST Key& tKey, Data** tppReturnData)
	{
		return DelMultiHash((CONST Key&&)tKey, (Data**)tppReturnData);
	}

	_BOOL ModifyMultiHashData(CONST Key&& tKey, CONST Data&& tModifyData, Data** tppReturnSourceData);//�޸�һ����ֵ���µ�ֵ
	_BOOL ModifyMultiHashData(CONST Key& tKey, CONST Data& tModifyData, Data** tppReturnSourceData)
	{
		return ModifyMultiHashData((CONST Key&&)tKey, (CONST Data&&)tModifyData, (Data**)tppReturnSourceData);
	}

	_BOOL ModifyMultiHashKey(CONST Key&& tSourceKey, CONST Key&& tModifyKey);//�޸�һ����ֵ�Եļ�:��ӳ���ֵ��
	_BOOL ModifyMultiHashKey(CONST Key& tSourceKey, CONST Key& tModifyKey)
	{
		return ModifyMultiHashKey((CONST Key&&)tSourceKey, (CONST Key&&)tModifyKey);
	}

	_BOOL FindMultiHashData(CONST Key&& tKey, Data*& tReturnData);//����һ�����µ�ֵ
	_BOOL FindMultiHashData(CONST Key& tKey, Data*& tReturnData)
	{
		return FindMultiHashData((CONST Key&&)tKey, (Data*&)tReturnData);
	}

	_BOOL SwapMultiHashData(CONST Key&& tKey1, CONST Key&& tKey2);//�����������µ�ֵ
	_BOOL SwapMultiHashData(CONST Key& tKey1, CONST Key& tKey2)
	{
		return SwapMultiHashData((CONST Key&&)tKey1, (CONST Key&&)tKey2);
	}

	VOID EmptyMultiHash(VOID);//������ű�
};

#undef CONST
#undef NULL