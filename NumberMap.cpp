#include "NumberMap.h"
NumberMap::NumberMap()
{
	ResetMap();
	ResetMark();
	InitMark();
}

NumberMap::NumberMap(string savepoint)
{
	//启用保存记录点并打开文件
	//输入：保存点文件
	SaveFile = true;
	OpenSavePoint(savepoint);
}

NumberMap::~NumberMap()
{
	//释放所有格子信息堆栈资源
	CellInfo* top;
	while (!CellStack.empty()) {
		top = CellStack.top();
		CellStack.pop();
		delete top;
	}
}

void NumberMap::OpenSavePoint(string savepoint)
{
	//打开保存点文件并恢复现场
	//输入：保存点文件
	ResetMap();
	InitMark();
	this->sp = savepoint;
	fstream record(sp, ios_base::in);
	if (!record.is_open()) {
		ErrorText = "Save Point Not exsist\n";
		Error = true;
		return;
	}
	record >> this->ans_count;
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			int tmp;
			record >> tmp;
			if (tmp >= 0 && tmp <= 9)
				this->mp[i][j] = tmp;
			else {
				Error = true;
				ErrorText = "Data Range Error.\n";
				return;
			}
		}

	RenewMark();
	if (Error) {
		record.close();
		cout << ErrorText;
		return;
	}
	int stacks;
	record >> stacks;
	if (stacks != 0)	BuildStack(record, stacks);
	record.close();
	if (Error) {
		cout << ErrorText;
	}
}

void NumberMap::InitMark(void)
{
	//标记初始化
	LineMark.resize(9, 0);
	RowMark.resize(9, 0);
	BlockMark.resize(9, 0);
}

void NumberMap::ResetMap(void)
{
	//重置表格并初始化为空白（0）
	this->mp.resize(9);
	for (auto& v : mp)	v = vector<int>(9, 0);
}

void NumberMap::ResetMark(void)
{
	//重置标记
	this->LineMark.resize(9);
	this->RowMark.resize(9);
	this->BlockMark.resize(9);
	for (int i = 0; i < 9; i++) {
		this->LineMark[i] = 0;
		this->RowMark[i] = 0;
		this->BlockMark[i] = 0;
	}
}

void NumberMap::RenewMark(void)
{
	//更新标记信息
	if (!IsMapValid() && Error) {
		ErrorText += "Please check the data\n";
	}
}

bool NumberMap::RenewLineMark(int x, short mask)
{
	//更新行标记
	if ((LineMark[x] & mask) == 0) {
		LineMark[x] |= mask;
		return true;
	}
	return false;
}

bool NumberMap::RenewRowMark(int y, short mask)
{
	//更新列标记
	if ((RowMark[y] & mask) == 0) {
		RowMark[y] |= mask;
		return true;
	}
	return false;
}

bool NumberMap::RenewBlockMark(int id, short mask)
{
	//更新块标记
	if ((BlockMark[id] & mask) == 0) {
		BlockMark[id] |= mask;
		return true;
	}
	return false;
}

bool NumberMap::IsMapValid(void)
{
	//检查表格合法性
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			int n = mp[i][j];
			if (n != 0) {
				short mask = 1 << (n - 1);
				if (!RenewLineMark(i, mask)) {
					Error = true;
					ErrorText = "Line Error.\n";
					return false;
				}

				if (!RenewRowMark(j, mask)) {
					Error = true;
					ErrorText = "Row Error.\n";
					return false;
				}

				if (!RenewBlockMark(Blockid(i, j), mask)) {
					Error = true;
					ErrorText = "Block Error.\n";
					return false;
				}
			}
		}
	}
	return true;
}

vector<int> NumberMap::UsefulNumber(int x, int y)
{
	//计算可用数字
	//输入：x：行坐标（0~8）
	//		y：列坐标（0~8）
	//输出：可用数字数组
	vector<int> nums;
	short mask = LineMark[x] | RowMark[y] | BlockMark[Blockid(x, y)];
	for (int i = 1; i <= 9; i++) {
		if ((mask & 1) == 0)	nums.push_back(i);
		mask = mask >> 1;
	}
	return nums;
}

bool NumberMap::TestFinish(void)
{
	//检查解答是否完成
	for (auto f : LineMark)	if (f != 511)	return false;
	for (auto f : RowMark)	if (f != 511)	return false;
	for (auto f : BlockMark)	if (f != 511)	return false;
	return true;
}

void NumberMap::BuildStack(fstream& f, int l)
{
	//创建格子信息堆栈
	//输入：f:输入文件指针
	//		l:栈层数
	if (l <= 0)	return;
	CellInfo* ci = new CellInfo();
	f >> ci->id >> ci->num >> ci->next_id;
	int size;
	f >> size;
	ci->valid_nums.resize(size);
	for (int i = 0; i < size; i++) {
		int tmp;
		f >> tmp;
		if (tmp >= 0 && tmp <= 9)
			ci->valid_nums[i] = tmp;
		else {
			Error = true;
			ErrorText = "Data Range Error\n";
			return;
		}
	}
	CellStack.push(ci);
	BuildStack(f, l - 1);
}

void NumberMap::SaveStack(fstream& f)
{
	//保存格子信息堆栈
	//输入：f：输出文件指针
	if (CellStack.empty())	return;
	CellInfo* top = CellStack.top();
	CellStack.pop();
	SaveStack(f);
	f << top->id << " " << top->num << " " << top->next_id << " " << top->valid_nums.size();
	for (auto n : top->valid_nums)	f << " " << n;
	f << endl;
	CellStack.push(top);
}

void NumberMap::FindAllAnswer(void)
{
	//求解所有答案，若数独表格有大量空白（0）需大量计算，且耗时时间长
	//请确保计算量小或在保存文件的基础上使用
	if (Error) {
		cout << "Something wrong,Please check the data input.\n";
		cout << ErrorText;
		return;
	}
	if (TestFinish()) {
		cout << "A Finished Map.\n";
		return;
	}
	int id = 0, x, y;
	CellInfo* top;
	if (CellStack.empty()) {
		while (id < 81 && mp[id / 9][id % 9] != 0)	id++;
		x = id / 9; y = id % 9;
		top = new CellInfo(id, 0, UsefulNumber(x, y));
		CellStack.push(top);
	}
	else	id = CellStack.top()->next_id;

	while (!CellStack.empty()) {
		top = CellStack.top();
		id = top->id;
		x = id / 9; y = id % 9;
		if (top->num == top->valid_nums.size()) {
			ResetCell(x, y);
			CellStack.pop();
			delete top;
		}
		else {
			RenewCell(x, y, top->valid_nums[top->num++]);
			while (id < 81 && mp[id / 9][id % 9] != 0)	id++;
			if (id == 81 && TestFinish()) {
				//得出答案
				PrintAns();
				if (SaveFile) {
					SaveAnswer();
				}
				ans_count++;
				Epoch++;
				if (Epoch == SaveEpoch) {
					SavePoint();
					Epoch = 0;
				}
			}
			else {
				//下一层
				top->next_id = id;
				x = id / 9; y = id % 9;
				top = new CellInfo(id, 0, UsefulNumber(x, y));
				CellStack.push(top);
			}
		}
	}
}

void NumberMap::FindOneAnswer(void)
{
	//求解下一个答案
	if (Error) {
		cout << "Something wrong,Please check the data input.\n";
		cout << ErrorText;
		return;
	}
	cout << "Find one answer of:\n";
	PrintMap();
	if (TestFinish()) {
		cout << "A Finished Map.\n";
		return;
	}
	int id = 0, x, y;
	CellInfo* top;
	if (CellStack.empty()) {
		while (id < 81 && mp[id / 9][id % 9] != 0)	id++;
		x = id / 9; y = id % 9;
		top = new CellInfo(id, 0, UsefulNumber(x, y));
		CellStack.push(top);
	}
	else	id = CellStack.top()->next_id;

	int flag = false;
	while (!CellStack.empty() && !flag) {
		top = CellStack.top();
		id = top->id;
		x = id / 9; y = id % 9;
		if (top->num == top->valid_nums.size()) {
			ResetCell(x, y);
			CellStack.pop();
			delete top;
		}
		else {
			RenewCell(x, y, top->valid_nums[top->num++]);
			while (id < 81 && mp[id / 9][id % 9] != 0)	id++;
			if (id == 81 && TestFinish()) {
				//得出答案
				PrintAns();
				if (SaveFile) {
					SaveAnswer();
				}
				ans_count++;
				flag = true;
			}
			else {
				//下一层
				top->next_id = id;
				x = id / 9; y = id % 9;
				top = new CellInfo(id, 0, UsefulNumber(x, y));
				CellStack.push(top);
			}
		}
	}
	if (!flag) {
		cout << "Did not find one answer.\n";
	}
}

void NumberMap::RenewCell(int x, int y, int n)
{
	//更新格子信息
	//输入：x：行坐标（0~8）
	//		y：列坐标（0~8）
	//		n：更新的数字
	if (n == 0) {
		this->ResetCell(x, y);
		return;
	}
	if (mp[x][y] != 0)	this->ResetCell(x, y);
	mp[x][y] = n;
	short mask = (1 << (n - 1));
	LineMark[x] |= mask;
	RowMark[y] |= mask;
	BlockMark[this->Blockid(x, y)] |= mask;
}

void NumberMap::ResetCell(int x, int y)
{
	//重置格子信息
	//输入：x：行坐标（0~8）
	//		y：列坐标（0~8）
	if (mp[x][y] == 0)	return;
	int n = mp[x][y];
	mp[x][y] = 0;
	short mask = ~(1 << (n - 1));
	LineMark[x] &= mask;
	RowMark[y] &= mask;
	BlockMark[this->Blockid(x, y)] &= mask;
}

void NumberMap::SaveAnswer(void)
{
	//保存答案
	fstream ans(ansf, ios_base::app);
	ans << ans_count << ":" << endl;
	for (auto v : this->mp) {
		for (auto n : v)	ans << n << " ";
		ans << endl;
	}
	ans << endl;
	ans.close();
}

void NumberMap::PrintAns(void)
{
	//打印当前答案
	cout << ans_count << ":" << endl;
	this->PrintMap();
}

void NumberMap::PrintMap(void)
{
	//打印当前表格
	for (auto v : this->mp) {
		for (auto n : v)	cout << n << " ";
		cout << endl;
	}
	cout << endl;
}

void NumberMap::SavePoint(void)
{
	//保存当前状态作为保存点
	fstream spf(this->sp, ios_base::out);
	if (!spf.is_open())	return;
	spf << this->ans_count << endl;
	for (auto v : this->mp) {
		for (auto n : v)	spf << n << " ";
		spf << endl;
	}
	int stacks = CellStack.size();
	spf << stacks << endl;
	if (stacks > 0)	SaveStack(spf);
	spf << endl;
	spf.close();
}

int NumberMap::Blockid(int x, int y)
{
	//计算对应坐标所属block的索引
	//输入：x：行坐标（0~8）
	//		y：列坐标（0~8）
	//输出：块索引（0~8）
	x = x / 3;
	y = y / 3;
	return 3 * x + y;
}

int NumberMap::BlockPos(int x, int y)
{
	//计算块内位置
	//输入：x：行坐标（0~8）
	//		y：列坐标（0~8）
	//输出：块内坐标（0~8）
	x = x % 3;
	y = y % 3;
	return 3 * x + y;
}
