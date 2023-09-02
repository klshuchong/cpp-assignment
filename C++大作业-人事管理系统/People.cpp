//People.cpp
//Created by Wang Jie (2022010554)
//People��ʵ��
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
	//������֤�ų���
	if (tested.length() != 18)return false;

	//���ǰ17λ�Ƿ�Ϊ���ֲ�����У��λ
	int sum = 0, i;
	for (i = 0; i < 17; i++)
	{
		if (!isdigit(tested[i]))return false;
		sum += id_weight[i] * (tested[i] - '0');
	}
	int test_digit = (12 - sum % 11) % 11;

	//������֤���а����������Ƿ�Ϊ�Ϸ�����
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

	//���У��λ
	return test_digit < 10 && test_digit == tested[17] - '0' || test_digit == 10 && tested[17] == 'X';
}

People::People(const string& n_name, const string& n_id, const shared_ptr<department>& n_father, node::NodeType n_people_type) :node(n_name, n_people_type, n_father)
{
	//�������֤��
	setidcard(n_id);

	//���ñ��
	people_uid = max_people_uid;
	max_people_uid++;
}

People::People(ifstream& ifs, node::NodeType n_people_type) :node(ifs, n_people_type)
{
	//��ȡ���֤��
	char id_buffer[19];
	ifs.read(id_buffer, 18);
	id_buffer[18] = '\0';
	
	//��֤���������֤��
	setidcard(id_buffer);

	//��ȡ��Ա���
	ifs.read((char*)&people_uid, sizeof(int));
}

void People::print(ostream& os) const
{
	os << "��ǰ���������Ա��" << name << endl;
	os << "���֤�ţ�" << id_card << endl;
	os << "�Ա�";
	switch (gender)
	{
	case Gender::Male:os << "��" << endl; break;
	case Gender::Female:os << "Ů" << endl; break;
	}
	os << "�������ڣ�" << birthday << endl;
	os << "��Ա���ͣ�";
	switch (node_type)
	{
	case node::NodeType::None:os << "��" << endl << "��ţ�";
	case node::NodeType::Student:os << "ѧ��" << endl << "ѧ�ţ�";
	case node::NodeType::Graduate:os << "�о���" << endl << "ѧ�ţ�";
	case node::NodeType::Teacher:os << "��ʦ" << endl << "����֤�ţ�";
	case node::NodeType::Prof:os << "����" << endl << "����֤�ţ�";
	case node::NodeType::Ta:os << "����" << endl << "ѧ�ţ�";
	}
	os << people_uid << endl;
	os << "�������ţ�";
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
	//������֤�ŵĺϷ���
	if (!is_valid_id(n_id))throw IDException(n_id);

	//�������֤���Ƿ��Ѵ���
	if (!(id_set.insert(n_id).second))throw SameIDException(n_id);
	strcpy_s(id_card, n_id.c_str());

	//��ȡ�Ա�
	if ((id_card[16] - '0') % 2 == 0)gender = Gender::Female;
	else gender = Gender::Male;

	//��ȡ�������ڣ�ǰ���Ѿ��������֤�ŵĺϷ��ԣ��������ﹹ��Date�ǲ������쳣��
	int y = (id_card[6] - '0') * 1000 + (id_card[7] - '0') * 100 + (id_card[8] - '0') * 10 + (id_card[9] - '0');
	int m = (id_card[10] - '0') * 10 + (id_card[11] - '0');
	int d = (id_card[12] - '0') * 10 + (id_card[13] - '0');
	birthday = Date(y, m, d);
}