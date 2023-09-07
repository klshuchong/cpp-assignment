//userInterface.cpp
//Created by Wang Jie (2022010554)
//userInterface��ʵ��
#include <iostream>
#include <string>
#include <conio.h>
#include <vector>
#include <list>
#include <memory>
#include "userInterface.h"
#include "Exception.h"

userInterface::interface_type(userInterface::* const userInterface::itf[20])() =
{
	&userInterface::menu,
	&userInterface::view_structure,
	&userInterface::add_new_dept,
	&userInterface::rename_dept,
	&userInterface::move_dept,
	&userInterface::remove_dept,
	&userInterface::clear_structure,
	&userInterface::add_people,
	&userInterface::view_people,
	&userInterface::search_people,
	&userInterface::view_all_people,
	&userInterface::update_people,
	&userInterface::move_people,
	&userInterface::remove_people,
	&userInterface::set_advisor,
	&userInterface::remove_advisor,
	&userInterface::sort_people,
	&userInterface::clear_people,
	&userInterface::save_new,
	&userInterface::change_password
};

using std::endl;
using std::string;
using std::vector;
using std::list;
using std::set;
using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;

void userInterface::start()
{
	//��������
	os << "==========���¹���ϵͳ��ѧУ�棩==========" << endl;
	os << "����Ҫ�������ļ����Ǵ������ļ���" << endl;
	try
	{
		switch (choose(list<string>{"�������ļ�", "�������ļ�"}))
		{
		case 0:
			create();
			break;
		case 1:
			open();
			break;
		default:
			this->exit();
			return;
		}
	}
	//�����쳣
	catch (const MyException& ex)
	{
		errs << ex.info() << endl;
		return;
	}
	catch (const std::exception& ex)
	{
		errs << ex.what() << endl;
		return;
	}

	//��ʾ��ӭ����
	welcome();

	//��Ҫ��������
	interface_type current_itf = menu_t; //��ǰ�������ͣ���ֵΪ���˵�
	try
	{
		while (current_itf != exit_t)
		{
			os << string(32, '=') << endl;//���32��=
			current_itf = (this->*itf[(int)current_itf])();
		}
	}
	catch (const MyException& ex)
	{
		errs << ex.info() << endl;
		return;
	}
	catch (const std::exception& ex)
	{
		errs << ex.what() << endl;
		return;
	}

	//�˳�����
	this->exit();
}

int userInterface::choose(const list<string>& msg, const string& quit_msg)
{
	//��ӡѡ���б�
	int i = 0;
	for (auto itr = msg.begin(); itr != msg.end(); itr++, i++)
		os << i << ". " << *itr << endl;
	os << i << ". " << quit_msg << endl;
	os << "������ѡ��ǰ�ı��...";

	//�������룬ֱ������Ϸ�Ϊֹ
	int choice; //���������
	while (!(is >> choice) || choice<0 || choice>msg.size())
	{
		is.clear();
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		errs << "�������Ϊ������" << msg.size() << "�ķǸ����������������룺";
	}
	//���������������֮�����getline()ʱû���������
	is.clear();
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	if (choice == msg.size())return -1;
	else return choice;
}

string userInterface::input_password()
{
	string ans;
	char input;
	while ((input = _getch()) != '\r')
	{
		if (input == '\b')//������Ϊ�˸�
		{
			if (ans.length() > 0)
			{
				ans.pop_back();
				os << "\b \b";
			}
			else
			{
				os.put('\a');//����������Ϊ�ջ��˸�������
			}
		}
		else
		{
			ans.push_back(input);
			os << "*";
		}
	}
	return ans;
}

void userInterface::create()
{
	os << "�������ļ����湫˾���ƣ��ɰ���·��������ע�⣬��˾�������ú󲻿��޸ġ�";
	std::getline(is, filename);
	os << "���������룺";
	string password = input_password();
	os << endl << "���ٴ��������룺";
	string password2 = input_password();
	//��������������벻һ�£���Ҫ����������
	while (password != password2)
	{
		os << endl;
		errs << "������������벻һ�¡�" << endl;
		os << "�������������룺";
		password = input_password();
		os << endl << "���ٴ��������룺";
		password2 = input_password();
	}
	os << endl;

	//�������ݿ�
	db = make_shared<database>(filename, password, database::Create);
}

void userInterface::open()
{
	os << "�������ļ������ɰ���·������";
	std::getline(is, filename);
	os << "���������룺";
	string password = input_password();
	os << endl;

	while (1)
	{
		try
		{
			db = make_shared<database>(filename, password, database::Read);
		}
		catch (const MyException& ex)
		{
			errs << ex.info() << endl;
			os << "�ļ���ʧ�ܡ������ļ�·���������Ƿ���ȷ��" << endl;
			switch (choose(list<string>{"��������"}))
			{
			case -1:
				throw MyException("");//��һ���쳣���������ܽ���start()���쳣�������Ӷ���������
			case 0:
				os << "�������ļ������ɰ���·������";
				std::getline(is, filename);
				os << "���������룺";
				password = input_password();
				os << endl;
				continue;
			}
		}
		break;
	}
}

void userInterface::welcome()
{
	os << "��ӭʹ�����¹���ϵͳ��" << endl;
	os << "Creator: ���ݣ�ѧ�ţ�2022010554��" << endl;
	os << "��˾���ƣ�" << db->getcurrent()->getname() << endl << endl;
}

userInterface::interface_type userInterface::menu()
{
	switch (choose(list<string>{"�鿴��֯�ܹ�", "������Ա", "��ʾĳ����Ա", "��Ա����", "�����֯�ܹ�", "�����Ա��Ϣ", "����/���Ϊ", "�޸�����"}, "���沢�˳�"))
	{
	case 0: return view_structure_t;
	case 1: return search_people_t;
	case 2: return view_all_people_t;
	case 3: return sort_people_t;
	case 4: return clear_structure_t;
	case 5: return clear_people_t;
	case 6: return save_new_t;
	case 7: return change_password_t;
	default: return exit_t;
	}
}

userInterface::interface_type userInterface::view_structure()
{
	if (db->getcurrent()->getnodetype() != node::NodeType::Department)
		throw NodeTypeException(node::NodeType::Department);
	os << *(db->getcurrent());//��ӡ�ò�����Ϣ
	list<string> choice_list;
	auto all_child = db->getcurrent()->get_child();
	for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		if ((*itr)->getnodetype() == node::NodeType::Department)
		{
			choice_list.push_back("ѡ�� " + (*itr)->getname() + "...");
		}
		else choice_list.push_back("ת�� " + (*itr)->getname());
	choice_list.push_back("�½��¼�����");
	choice_list.push_back("�½�ֱ����Ա");
	choice_list.push_back("�����ϼ�����");
	int choice = choose(choice_list, "�������˵�");
	if (choice == -1)return menu_t;
	else if (choice < all_child.size())//���ѡ����ĳ�����Ż���Ա
	{
		//�ҵ���Ӧ�Ĳ��Ż���Ա
		shared_ptr<node> goal_child;
		int i = 0;
		for (auto itr = all_child.begin(); i <= choice; itr++, i++)
			goal_child = *itr;

		db->to_child(goal_child);
		if (goal_child->getnodetype() == node::NodeType::Department)//���ѡ����ǲ���
		{
			os << "��ѡ�� " << goal_child->getname() << "[����]" << endl;
			switch (choose(list<string>{"ת���ò�����ͼ", "�������ò���", "�ƶ��ò���", "ɾ���ò���"}, "���� " + goal_child->getfather()->getname()))
			{
			case 0: return view_structure_t;
			case 1: return rename_dept_t;
			case 2: return move_dept_t;
			case 3: return remove_dept_t;
			default: db->to_father(); return view_structure_t;
			}
		}
		else return view_people_t;
	}
	else
	{
		switch (choice - all_child.size())
		{
		case 0: return add_new_dept_t;
		case 1: return add_people_t;
		case 2: 
			if (!(db->to_father()))//��currentΪ���ڵ�
				os << "��ǰΪ���ڵ㣬�޷������ϼ����š�" << endl;
			return view_structure_t;
		default: return menu_t;
		}
	}
}

userInterface::interface_type userInterface::add_new_dept()
{
	os << "�������²��ŵ����ƣ�";
	string dept_name;
	std::getline(is, dept_name);
	auto new_dept = make_shared<department>(dept_name);
	while (1)
	{
		try
		{
			new_dept->setname(dept_name);
			db->add_new_dept(new_dept);
		}
		catch (const SameNameDepartmentException& ex)
		{
			errs << ex.info() << endl;
			os << db->getcurrent()->getname() << "���Ѵ��ڲ��š�" << dept_name << "�����Ƿ��������벿�����ƣ�" << endl;
			switch (choose(list<string>{"�������벿������"}, "����" + db->getcurrent()->getname()))
			{
			case 0: 
				os << "�����������²��ŵ����ƣ�";
				std::getline(is, dept_name);
				continue;
			default: return view_structure_t;
			}
		}
		break;
	}
	os << "������ӳɹ���" << endl;
	db->to_child(new_dept);
	return view_structure_t;
}

userInterface::interface_type userInterface::rename_dept()
{
	if (db->getcurrent()->getnodetype() != node::NodeType::Department)
		throw NodeTypeException(node::NodeType::Department);
	os << "���������ƣ�";
	string new_dept_name;
	std::getline(is, new_dept_name);
	auto n_dept = dynamic_pointer_cast<department, node>(db->getcurrent());
	db->to_father();
	while (1)
	{
		try
		{
			db->rename_dept(n_dept, new_dept_name);
		}
		catch (const SameNameDepartmentException& ex)
		{
			errs << ex.info() << endl;
			os << db->getcurrent()->getname() << "���Ѵ��ڲ��š�" << new_dept_name << "�����Ƿ��������벿�����ƣ�" << endl;
			switch (choose(list<string>{"�������벿������"}, "����" + db->getcurrent()->getname()))
			{
			case 0:
				os << "�����������²��ŵ����ƣ�";
				std::getline(is,  new_dept_name);
				continue;
			default: return view_structure_t;
			}
		}
		break;
	}
	os << "�����������ɹ���" << endl;
	return view_structure_t;
}

userInterface::interface_type userInterface::move_dept()
{
	os << "��ѡ��Ҫ�ƶ����Ĳ��š�" << endl;
	auto n_dept = db->getcurrent();
	auto original_father = n_dept->getfather();
	while (db->to_father());//��db��current�Ƶ����ڵ�
	n_dept->setfather(shared_ptr<department>());//��n_dept����֯���룬��ֹ��֯�ɻ�
	set<shared_ptr<node>, defaultNodeCmp> all_child;
	list<string> choice_list;
	int choice, dept_num;
	while (1)
	{
		choice_list.clear();
		os << "��ǰ������ţ�" << db->getcurrent()->getname() << endl;
		all_child = db->getcurrent()->get_child();
		dept_num = 0;
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			if ((*itr)->getnodetype() == node::NodeType::Department)
			{
				choice_list.push_back("ת�� " + (*itr)->getname());
				dept_num++;
			}
			else break;
		}
		choice_list.push_back("�ƶ����˴�");
		if(db->getcurrent()->getfather())
			choice_list.push_back("���� " + db->getcurrent()->getfather()->getname());
		choice = choose(choice_list, "ȡ���ƶ�����");
		if (choice == -1)
		{
			n_dept->setfather(original_father);//��n_dept�������ӻ���֯
			break;
		}
		else if (choice < dept_num)//��ѡ��ת���¼�����
		{
			auto itr = all_child.begin();
			int i = 0;
			while (i < choice)//��itr�Ƶ�Ҫת�����¼�����
			{
				i++;
				itr++;
			}
			db->to_child(*itr);
			continue;
		}
		else if (choice == dept_num)//��ѡ���ƶ����ò���
		{
			try
			{
				auto n_dept_dept = dynamic_pointer_cast<department, node>(n_dept);
				db->add_new_dept(n_dept_dept);
			}
			catch (const SameNameDepartmentException& ex)//��������������
			{
				errs << ex.info() << endl;
				os << "���� " << db->getcurrent()->getname() << " ������ͬ�����ţ��ƶ�����ʧ�ܡ�" << endl;
				n_dept->setfather(original_father);//��n_dept�������ӻ�ԭλ��
				break;
			}
			os << "�ƶ����ųɹ���" << endl;
			break;
		}
		else if (choice == dept_num + 1)//��ѡ��ת���ϼ�����
		{
			db->to_father();
			continue;
		}
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::remove_dept()
{
	os << "�Ƿ�Ҫɾ������ " << db->getcurrent()->getname() << " ���������������ź���Ա��" << endl;
	switch (choose(list<string>{"��"}, "��"))
	{
	case 0: 
		{
			auto temp = db->getcurrent();
			db->to_father();
			temp->setfather(shared_ptr<department>());
		}
		os << "ɾ�����ųɹ���" << endl;
		break;
	default:break;
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::clear_structure()
{
	os << "����Ҫִ�������֯�ܹ����������������룺";
	string password = input_password();
	try
	{
		db->clear_structure(password);
	}
	catch (const PasswordException&)//�������쳣��˵���������
	{
		os << "�������ȡ�������֯�ܹ�������" << endl;
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::add_people()
{
	os << "��ѡ������Ա�����ͣ�";
	node::NodeType p_type;
	switch (choose(list<string>{"ѧ��", "�о���", "��ʦ", "����", "����"}))
	{
	case 0: p_type = node::Student; break;
	case 1: p_type = node::Graduate; break;
	case 2: p_type = node::Teacher; break;
	case 3: p_type = node::Prof; break;
	case 4: p_type = node::Ta; break;
	default: return view_structure_t;
	}
	os << "����������Ա��������";
	string name;
	std::getline(is, name);
	string id_card;
	shared_ptr<People> new_people;
	while (1)
	{
		os << "����������Ա�����֤�ţ�";
		std::getline(is, id_card);
		try
		{
			switch (p_type)
			{
			case node::Student: new_people = make_shared<student>(name, id_card); break;
			case node::Graduate: new_people = make_shared<graduate>(name, id_card); break;
			case node::Teacher: new_people = make_shared<teacher>(name, id_card); break;
			case node::Prof: new_people = make_shared<prof>(name, id_card); break;
			case node::Ta: new_people = make_shared<TA>(name, id_card); break;
			}
		}
		catch (const MyException& ex)
		{
			errs << ex.info() << endl;
			os << "���֤�Ų��Ϸ����Ѵ��ڡ��Ƿ��������룿";
			switch (choose(list<string>{"��"}))
			{
			case 0: continue;
			default: return view_structure_t;
			}
		}
		break;
	}
	if ((int)p_type >= 5)//�����ԱΪ��ʦ��
	{
		os << "����������Ա����н��Ԫ����";
		unsigned int pay;
		while (!(is >> pay))
		{
			is.clear();
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			errs << "�������Ϊ�Ǹ����������������룺";
		}
		auto new_teacher = dynamic_pointer_cast<teacher, People>(new_people);
		new_teacher->setpay(pay);
	}
	auto cur_dept = dynamic_pointer_cast<department, node>(db->getcurrent());
	new_people->setfather(cur_dept);
	os << "��Ա��ӳɹ���" << endl;
	db->to_child(new_people);
	return view_people_t;
}

userInterface::interface_type userInterface::view_people()
{
	if (db->getcurrent()->getnodetype() == node::Department)
		throw NodeTypeException(node::People);
	os << *(db->getcurrent());
	list<string> choice_list{ "������Ա��Ϣ", "�ı���������", "�鿴��Ա���ڲ���", "ɾ����Ա" };
	if (db->getcurrent()->getnodetype() == node::Graduate || db->getcurrent()->getnodetype() == node::Ta)//�������ԱΪ�о���������
	{
		choice_list.push_back("�鿴��ʦ��Ϣ");
		choice_list.push_back("���õ�ѧ��ϵ");
		choice_list.push_back("�����ѧ��ϵ");
		switch (choose(choice_list, "�������˵�"))
		{
		case 0: return update_people_t;
		case 1: return move_people_t;
		case 2:
			db->to_father();
			return view_structure_t;
		case 3: return remove_people_t;
		case 4:
		{
			auto cur_gra = dynamic_pointer_cast<graduate, node>(db->getcurrent());
			os << *(cur_gra->getadvisor());
			os << "�밴���������...";
			is.get();
			return view_people_t;
		}
		case 5: return set_advisor_t;
		case 6: return remove_advisor_t;
		default: return menu_t;
		}
	}
	else if (db->getcurrent()->getnodetype() == node::Prof)
	{
		choice_list.push_back("�鿴ѧ����Ϣ");
		switch (choose(choice_list, "�������˵�"))
		{
		case 0: return update_people_t;
		case 1: return move_people_t;
		case 2:
			db->to_father();
			return view_structure_t;
		case 3: return remove_people_t;
		case 4: 
		{
			auto cur_prof = dynamic_pointer_cast<prof, node>(db->getcurrent());
			cur_prof->print_stus(os);
			os << "�밴���������...";
			is.get();
			return view_people_t;
		}
		default: return menu_t;
		}
	}
	else
	{
		switch (choose(choice_list, "�������˵�"))
		{
		case 0: return update_people_t;
		case 1: return move_people_t;
		case 2:
			db->to_father();
			return view_structure_t;
		case 3: return remove_people_t;
		default: return menu_t;
		}
	}
}

userInterface::interface_type userInterface::search_people()
{
	os << "ѡ����ҷ�ʽ��" << endl;
	switch (choose(list<string>{"����������", "�����֤�Ų���", "����Ų���"}, "�������˵�"))
	{
	case 0:
	{
		os << "����������";
		string name;
		std::getline(is, name);
		auto result = db->search_people_by_name(name);
		if (result.empty())
		{
			os << "ϵͳ��δ���ҵ���Ϊ " << name << " ����Ա��" << endl;
			
		}
		else
		{
			for (auto itr = result.begin(); itr != result.end(); itr++)
				os << **itr << endl;
		}
	}
	break;
	case 1:
	{
		os << "�������֤�ţ�";
		string id_card;
		std::getline(is, id_card);
		auto result = db->search_people_by_id_card(id_card);
		if (result)os << *result << endl;
		else os << "ϵͳ���Ҳ������֤��Ϊ " << id_card << " ����Ա��" << endl;
	}
	break;
	case 2:
	{
		os << "������Ա��ţ�";
		int people_uid;
		while (!(is >> people_uid))
		{
			is.clear();
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			errs << "����ı���Ϊ���������������룺";
		}
		auto result = db->search_people_by_people_uid(people_uid);
		if (result)os << *result << endl;
		else os << "ϵͳ���Ҳ������Ϊ " << people_uid << " ����Ա��" << endl;
	}
	break;
	default: return menu_t;
	}
	os << "�밴���������...";
	is.get();
	return menu_t;
}

userInterface::interface_type userInterface::view_all_people()
{
	os << "��ѡ��Ҫ�鿴����Ա���ͣ�" << endl;
	list<shared_ptr<People>> people_list;
	switch (choose(list<string>{"ѧ��", "�о���", "��ʦ", "����", "����"}, "�������˵�"))
	{
	case 0: people_list = db->sort_people_by_people_uid(node::Student); break;
	case 1: people_list = db->sort_people_by_people_uid(node::Graduate); break;
	case 2: people_list = db->sort_people_by_people_uid(node::Teacher); break;
	case 3: people_list = db->sort_people_by_people_uid(node::Prof); break;
	case 4: people_list = db->sort_people_by_people_uid(node::Ta); break;
	default: return menu_t;
	}
	if (people_list.empty())
		os << "ϵͳ���������������ҵ����͵���Ա��" << endl;
	else
	{
		for (auto itr = people_list.begin(); itr != people_list.end(); itr++)
			os << **itr << endl;
	}
	os << "�밴���������...";
	is.get();
	return menu_t;
}

userInterface::interface_type userInterface::update_people()
{
	os << "�밴��ϵͳ��ʾ�����µ���Ա��Ϣ�����հ����ʾ�����ġ�" << endl;
	string original_name = db->getcurrent()->getname();
	os << "�µ�������";
	string name;
	std::getline(is, name);
	string id_card;
	while (1)
	{
		os << "�µ����֤�ţ�";
		std::getline(is, id_card);
		try
		{
			db->update_people_info(name, id_card);
		}
		catch (const IDException& ex)
		{
			errs << ex.info() << endl;
			os << "���֤�Ų��Ϸ����Ѵ��ڡ��Ƿ��������룿" << endl;
			switch (choose(list<string>{"��"}, "�������Ĳ�������Ա��ͼ"))
			{
			case 0: continue;
			default: 
				db->update_people_info(original_name);
				return view_people_t;
			}
		}
		break;
	}
	if ((int)(db->getcurrent()->getnodetype()) >= 5)//�����ǰ��ԱΪ��ʦ��
	{
		os << "��н��";
		unsigned int pay;
		if (!(is >> pay))
		{
			is.clear();
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else
		{
			db->update_people_info(name, id_card, pay);
		}
	}
	os << "��Ա��Ϣ�޸ĳɹ���" << endl;
	return view_people_t;
}

userInterface::interface_type userInterface::move_people()
{
	os << "��ѡ��Ҫ�ƶ����Ĳ��š�" << endl;
	auto n_people = db->getcurrent();
	auto original_father = n_people->getfather();
	while (db->to_father());//��db��current�Ƶ����ڵ�
	set<shared_ptr<node>, defaultNodeCmp> all_child;
	list<string> choice_list;
	int choice, dept_num;
	while (1)
	{
		os << "��ǰ������ţ�" << db->getcurrent()->getname() << endl;
		all_child = db->getcurrent()->get_child();
		dept_num = 0;
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			if ((*itr)->getnodetype() == node::NodeType::Department)
			{
				choice_list.push_back("ת�� " + (*itr)->getname());
				dept_num++;
			}
			else break;
		}
		choice_list.push_back("�ƶ����˴�");
		if (db->getcurrent()->getfather())
			choice_list.push_back("���� " + db->getcurrent()->getfather()->getname());
		choice = choose(choice_list, "ȡ���ƶ���Ա");
		if (choice == -1)break;
		else if (choice < dept_num)//��ѡ��ת���¼�����
		{
			auto itr = all_child.begin();
			int i = 0;
			while (i < choice)//��itr�Ƶ�Ҫת�����¼�����
			{
				i++;
				itr++;
			}
			db->to_child(*itr);
			continue;
		}
		else if (choice == dept_num)//��ѡ���ƶ����ò���
		{
			auto n_people_people = dynamic_pointer_cast<People, node>(n_people);
			db->add_new_people(n_people_people);
			os << "�ƶ���Ա�ɹ���" << endl;
			break;
		}
		else if (choice == dept_num + 1)//��ѡ��ת���ϼ�����
		{
			db->to_father();
			continue;
		}
	}
	return view_people_t;
}

userInterface::interface_type userInterface::remove_people()
{
	os << "��ȷ��Ҫɾ����Ա " << db->getcurrent()->getname() << " ��";
	switch (choose(list<string>{"��"}))
	{
	case 0: 
		db->delete_people();
		return view_structure_t;
	default: return view_people_t;
	}
}

userInterface::interface_type userInterface::set_advisor()
{
	if (db->getcurrent()->getnodetype() != node::Graduate && db->getcurrent()->getnodetype() != node::Ta)
		throw NodeTypeException(node::Graduate);
	auto cur_gra = dynamic_pointer_cast<graduate, node>(db->getcurrent());
	if (cur_gra->getadvisor())
	{
		os << "���о������е�ʦ�����Ƚ����ԭ��ʦ�Ĺ�ϵ�������õ�ʦ��" << endl;
		return view_people_t;
	}

	os << "��ѡ��ʦ��" << endl;
	while (db->to_father());//��db��current�Ƶ����ڵ�
	set<shared_ptr<node>, defaultNodeCmp> all_child;
	list<string> choice_list;
	vector<shared_ptr<prof>> prof_vec;
	int choice, dept_num;
	while (1)
	{
		os << "��ǰ������ţ�" << db->getcurrent()->getname() << endl;
		all_child = db->getcurrent()->get_child();
		dept_num = 0;
		prof_vec.clear();
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			if ((*itr)->getnodetype() == node::NodeType::Department)
			{
				choice_list.push_back("ת�� " + (*itr)->getname());
				dept_num++;
			}
			else if ((*itr)->getnodetype() == node::NodeType::Prof)
			{
				choice_list.push_back("���õ�ʦΪ " + (*itr)->getname());
				prof_vec.push_back(dynamic_pointer_cast<prof, node>(*itr));
			}
		}
		if (db->getcurrent()->getfather())
			choice_list.push_back("���� " + db->getcurrent()->getfather()->getname());
		choice = choose(choice_list, "ȡ�����õ�ʦ");
		if (choice == -1)break;
		else if (choice < dept_num)//��ѡ��ת���¼�����
		{
			auto itr = all_child.begin();
			int i = 0;
			while (i < choice)//��itr�Ƶ�Ҫת�����¼�����
			{
				i++;
				itr++;
			}
			db->to_child(*itr);
			continue;
		}
		else if (choice < dept_num + prof_vec.size())//��ѡ����ĳ����ʦ
		{
			db->assign_advisor(prof_vec[choice - dept_num]);
			os << "���õ�ʦ�ɹ���" << endl;
			break;
		}
		else if (choice == dept_num + prof_vec.size() + 1)//��ѡ��ת���ϼ�����
		{
			db->to_father();
			continue;
		}
	}
	return view_people_t;
}

userInterface::interface_type userInterface::remove_advisor()
{
	if (db->getcurrent()->getnodetype() != node::Graduate && db->getcurrent()->getnodetype() != node::Ta)
		throw NodeTypeException(node::Graduate);
	auto cur_gra = dynamic_pointer_cast<graduate, node>(db->getcurrent());
	if (!(cur_gra->getadvisor()))
	{
		os << "���о������޵�ʦ��" << endl;
		return view_people_t;
	}
	os << "��ȷ��Ҫ��� " << cur_gra->getname() << " �� " << cur_gra->getadvisor()->getname() <<" �ĵ�ѧ��ϵ��";
	switch (choose(list<string>{"��"}))
	{
	case 0:
		db->remove_advisor();
		os << "�����ѧ��ϵ�ɹ���" << endl;
	}
	return view_people_t;
}

userInterface::interface_type userInterface::sort_people()
{
	os << "��ѡ������ʽ��";
	node::NodeType n_type;
	int choice;
	switch (choose(list<string>{"���������", "����н����"}))
	{
	case 0: 
	{
		list<shared_ptr<People>> result;
		os << "��ѡ��������Ա���͡�";
		choice = choose(list<string>{"ѧ��", "�о���", "��ʦ", "����", "����"});
		if (choice >= 0)
		{
			n_type = (node::NodeType)(choice + 3);
			os << "��ѡ������˳��";
			switch (choose(list<string>{"����", "����"}))
			{
			case 0: 
				result = db->sort_people_by_people_uid(n_type, true);
				if (result.size() > 0)
				{
					for (auto itr = result.begin(); itr != result.end(); itr++)
						os << **itr << endl;
				}
				else os << "ϵͳ���Ҳ���������Ա��" << endl;
				break;
			case 1:
				result = db->sort_people_by_people_uid(n_type, false);
				if (result.size() > 0)
				{
					for (auto itr = result.begin(); itr != result.end(); itr++)
						os << **itr << endl;
				}
				else os << "ϵͳ���Ҳ���������Ա��" << endl;
				break;
			}
		}
	}
	break;
	case 1:
	{
		list<shared_ptr<teacher>> result;
		os << "��ѡ��������Ա���͡�";
		choice = choose(list<string>{"��ʦ", "����", "����"});
		if (choice >= 0)
		{
			n_type = (node::NodeType)(choice + 5);
			os << "��ѡ������˳��";
			switch (choose(list<string>{"����", "����"}))
			{
			case 0:
				result = db->sort_teacher_by_pay(n_type, true);
				if (result.size() > 0)
				{
					for (auto itr = result.begin(); itr != result.end(); itr++)
						os << **itr << endl;
				}
				else os << "ϵͳ���Ҳ���������Ա��" << endl;
				break;
			case 1:
				result = db->sort_teacher_by_pay(n_type, false);
				if (result.size() > 0)
				{
					for (auto itr = result.begin(); itr != result.end(); itr++)
						os << **itr << endl;
				}
				else os << "ϵͳ���Ҳ���������Ա��" << endl;
				break;
			}
		}
	}
	break;
	}
	return menu_t;
}

userInterface::interface_type userInterface::clear_people()
{
	os << "����Ҫִ�������Ա���������������룺";
	string password = input_password();
	try
	{
		db->clear_people(password);
	}
	catch (const PasswordException&)//�������쳣��˵���������
	{
		os << "�������ȡ�������֯�ܹ�������" << endl;
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::save_new()
{
	os << "�������µ��ļ������ɰ���·������������Ϊ�ձ�ʾ���浽ԭ�ļ��У�";
	string n_filename;
	std::getline(is, n_filename);
	try
	{
		if (n_filename == filename)db->save();
		else db->save(n_filename);
	}
	catch (const FileException& ex)
	{
		errs << ex.info() << endl << "���ݱ���ʧ�ܡ�" << endl;
		return menu_t;
	}
	os << "�����ѳɹ����浽�ļ���" << n_filename << "��" << endl;
	return menu_t;
}

userInterface::interface_type userInterface::change_password()
{
	os << "������ԭ�������룺";
	string original_pwd = input_password();
	if (!(db->verify_password(original_pwd)))
	{
		errs << "��������޸��������������" << endl;
		return menu_t;
	}
	os << "�����������룺";
	string new_pwd = input_password();
	os << "���ٴ����������룺";
	string new_pwd2 = input_password();
	if (new_pwd2 != new_pwd)
	{
		errs << "������������벻һ�£��޸��������������" << endl;
		return menu_t;
	}
	db->change_password(new_pwd);
	os << "�޸�����ɹ���" << endl;
	return menu_t;
}

void userInterface::exit()
{
	if (db)//����Ѿ����������ݿ�
	{
		os << "���ڱ�������..." << endl;
		try
		{
			db->save();
		}
		catch (const FileException& ex)
		{
			errs << ex.info() << endl;
			errs << "���ݱ���ʧ�ܡ�" << endl;
			return;
		}
		os << "�����ѳɹ����浽�ļ���" << filename << "��" << endl;
	}
	os << "��л��ʹ�ñ�ϵͳ���´��ټ���" << endl;
}