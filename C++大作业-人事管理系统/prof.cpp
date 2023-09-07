//prof.cpp
//Created by Wang Jie (2022010554)
//prof��ʵ��
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
		os << "�ý���û��ѧ����" << endl;
	}
	else
	{
		os << "�ý��ڹ���" << stus.size() << "��ѧ��������Ϊ��ѧ������Ϣ��" << endl << endl;
		for (auto itr = stus.begin(); itr != stus.end(); itr++)
		{
			os << **itr << endl;
		}
	}
}

prof::~prof()
{
	//��Ҫ�������ʦ������ѧ���ĵ�ʦ��Ϊ��
	while (!stus.empty())
	{
		shared_ptr<graduate> stu = *(stus.begin());
		stu->remove_advisor(); //����д*(stus.begin())->remove_advisor()����Ϊ����stus.begin()ɾ����
		stus.erase(stu);
	}
}