#pragma once
/*
volatile��һ���������η���type specifier��
volatile����������Ϊָ��ؼ��֣�ȷ������ָ�������������Ż���ʡ�ԣ���Ҫ��ÿ��ֱ�Ӷ�ֵ��
volatile�ı�����˵��������ܻᱻ���벻���ظı䣬�������������Ͳ���ȥ�������������ֵ�ˡ�
*/

#define NULL 0


/*��׹�ϣ�ṹ*/


//���������ṹ
typedef struct MultiHashElement
{
	void* pData = NULL;//����ָ��(��ϣֵ)
	unsigned long long ullKey = 0;//��ϣ��
	//char cState = 0;//�ṹ״̬ 0:δ��ʹ�� 1:�ѱ�ʹ�� -1:�����޸�
}MULTIHASHELEMENT, * PMULTIHASHELEMENT;

//�������ṹ
typedef struct MultiHashTable
{
	unsigned long ulLayerElement = 0;//�ý�Ԫ����
	unsigned long ulUseLayerElement = 0;//��ʹ�õ�Ԫ����
	PMULTIHASHELEMENT pstLayerMultiHashTableElement = NULL;//�ý�������Ԫ��ָ��
}MULTIHASHTABLE, * PMULTIHASHTABLE;

//�������ṹ
typedef struct MultiHashBase
{
	unsigned long ulOrder = 0;//����
	unsigned long long ullTotalElement = 0;//��Ԫ����
	unsigned long long ullUseTotalElement = 0;//��ʹ�õ�Ԫ����
	PMULTIHASHTABLE pstMultiHashOrderTable = NULL;//�ײ����Ԫ��ָ��
}MULTIHASHBASE, * PMULTIHASHBASE;

//Ԫ�������ṹ
typedef struct ElementAddr
{
	PMULTIHASHTABLE pstOrderTable = 0;//�ײ�ָ��
	unsigned long ulTableElement = 0;//����������
}ELEMENTADDR, * PELEMENTADDR;

//Ԫ�����ݽṹ
typedef struct Element_Data
{
	ELEMENTADDR stElementAddr;//��������
	PMULTIHASHELEMENT stHashElement = NULL;//���������ṹ
}ELEMENT_DATA, * PELEMENT_DATA;


#undef NULL

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

bool InitMultiHash(unsigned long ulStartElement, double dMultiPower, unsigned long ulOrder, PMULTIHASHBASE pstMultiHashBase);

void FreeMultiHash(PMULTIHASHBASE pstMultiHashBase);

bool AddMultiHash(PMULTIHASHBASE pstMultiHashBase, PELEMENTADDR pstElementAddr, unsigned long ullKey, void* pData);

bool DelMultiHash(PMULTIHASHBASE pstMultiHashBase, PELEMENTADDR pstElementAddr, unsigned long ullKey, void** pData);
