//department.cpp
//Created by Wang Jie (2022010554)
//department��ʵ��
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
	os << "��ǰ����������ţ�";
	auto print_dept = father.lock();
	string path = name;
	while (print_dept) //��ָ�벻Ϊ��
	{
		path = (string)print_dept->name + "/" + path;
		print_dept = print_dept->father.lock();
	}
	os << path << endl;

	using std::setw;
	os << setw(6) << "���" << setw(10) << "���" << "����" << endl;
	int i = 0;
	for (auto itr = child.begin(); itr != child.end(); itr++, i++)
	{
		os << setw(6) << i;
		if ((*itr)->getnodetype() == node::NodeType::Department)
		{
			os << setw(10) << "����" << (*itr)->getname() << endl;
		}
		else
		{
			os << setw(10);
			switch ((*itr)->getnodetype())
			{
			case node::NodeType::None:os << "��Ա"; break;
			case node::NodeType::Student:os << "ѧ��"; break;
			case node::NodeType::Graduate:os << "�о���"; break;
			case node::NodeType::Teacher:os << "��ʦ"; break;
			case node::NodeType::Prof:os << "����"; break;
			case node::NodeType::Ta:os << "����"; break;
			}
			os << (*itr)->getname() << endl;
		}
	}
}
