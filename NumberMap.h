#pragma once
#ifndef _NUMBERMAP_H_
#define _NUMBERMAP_H_
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
using namespace std;

struct CellInfo
{
	//�洢����������Ϣ�Ľṹ��
	int id;			//����λ����Ϣ����Χ0~80�������ң����ϵ�������
	int num;		//��ǰ��������
	int next_id;	//��һ��������������
	vector<int> valid_nums;		//���������б�

	CellInfo(int i, int n, vector<int> vn) :id(i), num(n), valid_nums(vn), next_id(i + 1) {};
	CellInfo() {};
};

class NumberMap
{
	//���������࣬�Ϸ���������Ϊ0~9
public:
	vector<vector<int>> mp;			//��¼��ǰ���

	//bitmap��ǣ�����9������¼��Ӧλ������ʹ�������1Ϊ��ʹ��
	vector<short> LineMark;			//�б��
	vector<short> RowMark;			//�б��
	vector<short> BlockMark;		//����

	//������ز���
	int ans_count = 1;				//��ǰ����������
	string sp = "save_point.txt";	//Ĭ�ϱ�����ļ�
	string ansf = "Answer.txt";		//Ĭ�ϴ𰸱����ļ�
	bool SaveFile = false;			//�Ƿ��Զ������
	bool MapValid = true;			//�����Ƿ���Ч
	bool Error = false;				//�Ƿ���ڴ���
	int SaveEpoch = 100;			//�Զ������ִ�
	int Epoch = 0;					//��ǰ�ִ�
	stack<CellInfo*> CellStack;		//������Ϣջ
	string ErrorText;				//������Ϣ�ı�

public:
	NumberMap();					//Ĭ�Ϲ��캯��������ȫ0����
	NumberMap(string savepoint);	//���ر����ļ�
	~NumberMap();					//Ĭ����������
	void OpenSavePoint(string savepoint);	//��һ��������ļ�
	void InitMark(void);			//��ʼ��bitmap���
	int Blockid(int x, int y);		//�����Ӧ��������block������
	int BlockPos(int x, int y);		//�������λ��
	void SaveAnswer(void);			//�����
	void PrintAns(void);			//��ӡ��ǰ��
	void PrintMap(void);			//��ӡ��ǰ���
	void SavePoint(void);			//���浱ǰ״̬��Ϊ�����
	void RenewCell(int x, int y, int n);	//���¸�����Ϣ
	void ResetCell(int x, int y);			//���ø�����Ϣ
	void RenewMark(void);					//���±����Ϣ
	bool RenewLineMark(int x, short mask);	//�����б��
	bool RenewRowMark(int y, short mask);	//�����б��
	bool RenewBlockMark(int id, short mask);	//���¿���
	bool IsMapValid(void);			//�����Ϸ���
	vector<int> UsefulNumber(int x, int y);	//�����������
	bool TestFinish(void);			//������Ƿ����
	void BuildStack(fstream&, int);			//����������Ϣ��ջ
	void SaveStack(fstream&);		//���������Ϣ��ջ
	void FindAllAnswer(void);		//������д�
	void FindOneAnswer(void);		//�����һ����
	void ResetMap(void);			//���ñ��
	void ResetMark(void);			//���ñ��
	private:
};

#endif
