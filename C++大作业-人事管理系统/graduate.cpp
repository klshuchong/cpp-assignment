//graduate.cpp
//Created by Wang Jie (2022010554)
//graduate��ʵ��
#include <iostream>
#include <fstream>
#include "base.h"
#include "Derived_people.h"

const char graduate::default_name[node::NAME_MAX_LENGTH] = "default graduate name";

using std::ostream;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::shared_ptr;
using std::weak_ptr;

graduate::graduate(ifstream& ifs) :People(ifs, node::NodeType::Graduate)
{
	ifs.read((char*)&advisor_uid, sizeof(int));
	
	//��ʱ����ʦ��Ϊ��ָ��
	advisor = shared_ptr<prof>();
}

void graduate::print(ostream& os) const
{
	People::print(os);
	if (advisor.lock())
	{
		os << "��ʦ��" << advisor.lock()->getname() << endl;
	}
	else os << "��ʦ������" << endl;
}

void graduate::fwrite(ofstream& ofs) const
{
	People::fwrite(ofs);
	ofs.write((char*)&advisor_uid, sizeof(int));
}

bool graduate::assign_advisor(const shared_ptr<prof>& n_advisor)
{
	if (advisor.lock() || !n_advisor)return false;
	advisor = n_advisor;
	advisor_uid = advisor.lock()->getuid();

	//���µ�ʦ�˵���Ϣ
	advisor.lock()->stus.insert(std::dynamic_pointer_cast<graduate>(shared_from_this()));
	return true;
}

bool graduate::remove_advisor()
{
	if (!advisor.lock())return false;

	if (!advisor.expired())
	{
		//���µ�ʦ�˵���Ϣ
		advisor.lock()->stus.erase(std::dynamic_pointer_cast<graduate>(shared_from_this()));

		advisor = shared_ptr<prof>();
		advisor_uid = -1;
		return true;
	}
	else return false;
}