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
	if (stus.empty())
	{
		os << "该教授没有学生。" << endl;
	}
	else
	{
		os << "该教授共有" << stus.size() << "名学生，以下为各学生的信息：" << endl << endl;
		for (auto itr = stus.begin(); itr != stus.end(); itr++)
		{
			os << **itr << endl;
		}
	}
}

prof::~prof()
{
	//需要把这个导师的所有学生的导师设为空
	while (!stus.empty())
	{
		shared_ptr<graduate> stu = *(stus.begin());
		stu->remove_advisor(); //不能写*(stus.begin())->remove_advisor()，因为这会把stus.begin()删除掉
		stus.erase(stu);
	}
}