//prof.cpp
//Created by Wang Jie (2022010554)
//prof类实现
#include <iostream>
#include <iomanip>
#include <fstream>
#include "base.h"
#include "Derived_people.h"

const char prof::default_name[node::NAME_MAX_LENGTH] = "default prof name";

using std::ostream;
using std::endl;
using std::iostream;
using std::fstream;
using std::shared_ptr;

void prof::print_stus(ostream& os) const
{
	os << name << "教授的学生姓名：" << endl;
	int i = 0;
	for (auto itr = stus.begin(); itr != stus.end(); itr++, i++)
	{
		os << std::setw(6) << i << (*itr)->getname() << endl;
	}
}

prof::~prof()
{
	//需要把这个导师的所有学生的导师设为空
	while (!stus.empty())
	{
		shared_ptr<graduate> stu = *(stus.begin());
		stu->remove_advisor(); //不能写*(stus.begin())->remove_advisor()，因为这会把stus.begin()删除掉
	}
}