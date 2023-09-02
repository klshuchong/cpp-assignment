//People.cpp
//Created by Wang Jie (2022010554)
//People类实现
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "Date.h"
#include "base.h"
#include "Exception.h"

const char People::default_name[node::NAME_MAX_LENGTH] = "default people name";
const char People::default_id[19] = "100000200001010017";
const int People::id_weight[17] = { 7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2 };
std::set<std::string> People::id_set;
unsigned int People::max_people_uid = 0;

using std::ostream;
using std::cerr;
using std::ifstream;
using std::ofstream;
using std::endl;
using std::string;
using std::shared_ptr;

bool People::is_valid_id(const string& tested)
{
	//检查身份证号长度
	if (tested.length() != 18)return false;

	//检查前17位是否为数字并计算校验位
	int sum = 0, i;
	for (i = 0; i < 17; i++)
	{
		if (!isdigit(tested[i]))return false;
		sum += id_weight[i] * (tested[i] - '0');
	}
	int test_digit = (12 - sum % 11) % 11;

	//检查身份证号中包含的日期是否为合法日期
	try 
	{
		int y = (tested[6] - '0') * 1000 + (tested[7] - '0') * 100 + (tested[8] - '0') * 10 + (tested[9] - '0');
		int m = (tested[10] - '0') * 10 + (tested[11] - '0');
		int d = (tested[12] - '0') * 10 + (tested[13] - '0');
		Date tempdate(y, m, d);
	}
	catch (const DateException& dateEx)
	{
		cerr << dateEx.info() << endl;
		return false;
	}

	//检查校验位
	return test_digit < 10 && test_digit == tested[17] - '0' || test_digit == 10 && tested[17] == 'X';
}

People::People(const string& n_name, const string& n_id, const shared_ptr<department>& n_father, node::NodeType n_people_type) :node(n_name, n_people_type, n_father)
{
	//设置身份证号
	setidcard(n_id);

	//设置编号
	people_uid = max_people_uid;
	max_people_uid++;
}

People::People(ifstream& ifs, node::NodeType n_people_type) :node(ifs, n_people_type)
{
	//读取身份证号
	char id_buffer[19];
	ifs.read(id_buffer, 18);
	id_buffer[18] = '\0';
	
	//验证并设置身份证号
	setidcard(id_buffer);

	//读取人员编号
	ifs.read((char*)&people_uid, sizeof(int));
}

void People::print(ostream& os) const
{
	os << "当前正在浏览人员：" << name << endl;
	os << "身份证号：" << id_card << endl;
	os << "性别：";
	switch (gender)
	{
	case Gender::Male:os << "男" << endl; break;
	case Gender::Female:os << "女" << endl; break;
	}
	os << "出生日期：" << birthday << endl;
	os << "人员类型：";
	switch (node_type)
	{
	case node::NodeType::None:os << "无" << endl << "编号：";
	case node::NodeType::Student:os << "学生" << endl << "学号：";
	case node::NodeType::Graduate:os << "研究生" << endl << "学号：";
	case node::NodeType::Teacher:os << "教师" << endl << "工作证号：";
	case node::NodeType::Prof:os << "教授" << endl << "工作证号：";
	case node::NodeType::Ta:os << "助教" << endl << "学号：";
	}
	os << people_uid << endl;
	os << "所属部门：";
	auto print_dept = father.lock()->getfather();
	string path = father.lock()->getname();
	while (print_dept)
	{
		path = (string)print_dept->getname() + "/" + path;
		print_dept = print_dept->getfather();
	}
	os << path << endl;
}

void People::fwrite(ofstream& ofs) const
{
	node::fwrite(ofs);
	ofs.write(id_card, 18);
	ofs.write((char*)&people_uid, sizeof(int));
}

void People::setidcard(const string& n_id)
{
	//检测身份证号的合法性
	if (!is_valid_id(n_id))throw IDException(n_id);

	//检测该身份证号是否已存在
	if (!(id_set.insert(n_id).second))throw SameIDException(n_id);
	strcpy_s(id_card, n_id.c_str());

	//读取性别
	if ((id_card[16] - '0') % 2 == 0)gender = Gender::Female;
	else gender = Gender::Male;

	//读取出生日期，前面已经检查过身份证号的合法性，所以这里构造Date是不会抛异常的
	int y = (id_card[6] - '0') * 1000 + (id_card[7] - '0') * 100 + (id_card[8] - '0') * 10 + (id_card[9] - '0');
	int m = (id_card[10] - '0') * 10 + (id_card[11] - '0');
	int d = (id_card[12] - '0') * 10 + (id_card[13] - '0');
	birthday = Date(y, m, d);
}