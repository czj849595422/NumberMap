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
	//存储数独格子信息的结构体
	int id;			//格子位置信息，范围0~80，由左到右，由上到下排序
	int num;		//当前格子数字
	int next_id;	//下一个可用数字索引
	vector<int> valid_nums;		//可用数字列表

	CellInfo(int i, int n, vector<int> vn) :id(i), num(n), valid_nums(vn), next_id(i + 1) {};
	CellInfo() {};
};

class NumberMap
{
	//数独格子类，合法格子数字为0~9
public:
	vector<vector<int>> mp;			//记录当前表格

	//bitmap标记，均有9个，记录对应位置数字使用情况，1为已使用
	vector<short> LineMark;			//行标记
	vector<short> RowMark;			//列标记
	vector<short> BlockMark;		//块标记

	//配置相关参数
	int ans_count = 1;				//当前正在求解答案序
	string sp = "save_point.txt";	//默认保存点文件
	string ansf = "Answer.txt";		//默认答案保存文件
	bool SaveFile = false;			//是否自动保存答案
	bool MapValid = true;			//数独是否有效
	bool Error = false;				//是否存在错误
	int SaveEpoch = 100;			//自动保存轮次
	int Epoch = 0;					//当前轮次
	stack<CellInfo*> CellStack;		//格子信息栈
	string ErrorText;				//错误信息文本

public:
	NumberMap();					//默认构造函数，创造全0数独
	NumberMap(string savepoint);	//加载保存文件
	~NumberMap();					//默认析构函数
	void OpenSavePoint(string savepoint);	//打开一个保存点文件
	void InitMark(void);			//初始化bitmap标记
	int Blockid(int x, int y);		//计算对应坐标所属block的索引
	int BlockPos(int x, int y);		//计算块内位置
	void SaveAnswer(void);			//保存答案
	void PrintAns(void);			//打印当前答案
	void PrintMap(void);			//打印当前表格
	void SavePoint(void);			//保存当前状态作为保存点
	void RenewCell(int x, int y, int n);	//更新格子信息
	void ResetCell(int x, int y);			//重置格子信息
	void RenewMark(void);					//更新标记信息
	bool RenewLineMark(int x, short mask);	//更新行标记
	bool RenewRowMark(int y, short mask);	//更新列标记
	bool RenewBlockMark(int id, short mask);	//更新块标记
	bool IsMapValid(void);			//检查表格合法性
	vector<int> UsefulNumber(int x, int y);	//计算可用数字
	bool TestFinish(void);			//检查解答是否完成
	void BuildStack(fstream&, int);			//创建格子信息堆栈
	void SaveStack(fstream&);		//保存格子信息堆栈
	void FindAllAnswer(void);		//求解所有答案
	void FindOneAnswer(void);		//求解下一个答案
	void ResetMap(void);			//重置表格
	void ResetMark(void);			//重置标记
	private:
};

#endif
