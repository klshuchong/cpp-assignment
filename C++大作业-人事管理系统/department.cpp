//department.cpp
//Created by Wang Jie (2022010554)
//department类实现
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <set>
#include "base.h"
#include "Exception.h"

const char department::default_name[node::NAME_MAX_LENGTH] = "default department name";

using std::ostream;
using std::endl;
using std::ofstream;
using std::set;
using std::string;

void department::print(ostream& os) const
{
	os << "当前正在浏览部门：";
	auto print_dept = father.lock();
	string path = name;
	while (print_dept) //若指针不为空
	{
		path = (string)print_dept->name + "/" + path;
		print_dept = print_dept->father.lock();
	}
	os << path << endl;

	using std::setw;
	os << setw(6) << "序号" << setw(10) << "类别" << "名称" << endl;
	int i = 0;
	for (auto itr = child.begin(); itr != child.end(); itr++, i++)
	{
		os << setw(6) << i;
		if ((*itr)->getnodetype() == node::NodeType::Department)
		{
			os << setw(10) << "部门" << (*itr)->getname() << endl;
		}
		else
		{
			os << setw(10);
			switch ((*itr)->getnodetype())
			{
			case node::NodeType::None:os << "人员"; break;
			case node::NodeType::Student:os << "学生"; break;
			case node::NodeType::Graduate:os << "研究生"; break;
			case node::NodeType::Teacher:os << "教师"; break;
			case node::NodeType::Prof:os << "教授"; break;
			case node::NodeType::Ta:os << "助教"; break;
			}
			os << (*itr)->getname() << endl;
		}
	}
}
