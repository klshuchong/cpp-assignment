//userInterface.cpp
//Created by Wang Jie (2022010554)
//userInterface��ʵ��
#include <iostream>
#include <string>
#include <conio.h>
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

using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::string;
using std::list;
using std::set;
using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;

void userInterface::start()
{
	//��������
	cout << "==========���¹���ϵͳ��ѧУ�棩==========" << endl;
	cout << "����Ҫ�������ļ����Ǵ������ļ���" << endl;
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
		cerr << ex.info() << endl;
		return;
	}

	//��ʾ��ӭ����
	welcome();

	//��Ҫ��������
	interface_type current_itf = menu_t; //��ǰ�������ͣ���ֵΪ���˵�
	try
	{
		cout << string(32, '=');//���32��=
		while (current_itf != exit_t)
			current_itf = (this->*itf[(int)current_itf])();
	}
	catch (const MyException& ex)
	{
		cerr << ex.info() << endl;
		return;
	}

	//�˳�����
	this->exit();
}

int userInterface::choose(const list<string>& msg, const string& quit_msg = "�˳�")
{
	//��ӡѡ���б�
	int i = 0;
	for (auto itr = msg.begin(); itr != msg.end(); itr++, i++)
		cout << i << ". " << *itr << endl;
	cout << i << ". " << quit_msg << endl;
	cout << "������ѡ��ǰ�ı��...";

	//�������룬ֱ������Ϸ�Ϊֹ
	int choice; //���������
	while (!(cin >> choice) || choice<0 || choice>msg.size())
	{
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		cerr << "�������Ϊ������" << msg.size() << "�ķǸ����������������룺";
	}

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
				cout << "\b \b";
			}
			else
			{
				std::putchar('\a');//����������Ϊ�ջ��˸�������
			}
		}
		else
		{
			ans.push_back(input);
			cout << "*";
		}
	}
	return ans;
}

void userInterface::create()
{
	cout << "�������ļ����湫˾���ƣ��ɰ���·��������ע�⣬��˾�������ú󲻿��޸ġ�";
	string filename;
	std::getline(cin, filename);
	cout << "���������루������8���ַ�����";
	string password = input_password();
	cout << "���ٴ��������룺";
	string password2 = input_password();
	//��������������벻һ�£���Ҫ����������
	while (password != password2)
	{
		cerr << "������������벻һ�¡�" << endl;
		cout << "�������������루������8���ַ�����";
		password = input_password();
		cout << "���ٴ��������룺";
		password2 = input_password();
	}

	//�������ݿ�
	db = make_shared<database>(filename, password, database::Create);
}

void userInterface::open()
{
	cout << "�������ļ������ɰ���·������";
	string filename;
	std::getline(cin, filename);
	cout << "���������룺";
	string password = input_password();

	while (1)
	{
		try
		{
			db = make_shared<database>(filename, password, database::Read);
		}
		catch (const MyException& ex)
		{
			cerr << ex.info() << endl;
			cout << "�ļ���ʧ�ܡ��Ƿ�����ѡ���ļ���";
			switch (choose(list<string>{"����ѡ���ļ�"}))
			{
			case -1:
				throw MyException("");//��һ���쳣���������ܽ���start()���쳣�������Ӷ���������
			case 0:
				cout << "�������ļ������ɰ���·������";
				string filename;
				std::getline(cin, filename);
				cout << "���������룺";
				string password = input_password();
				continue;
			}
		}
		break;
	}
}

void userInterface::welcome()
{
	cout << "��ӭʹ�����¹���ϵͳ��" << endl;
	cout << "Creator: ���ݣ�ѧ�ţ�2022010554��" << endl;
	cout << "��˾���ƣ�" << db->getcurrent()->getname() << endl << endl;
}

userInterface::interface_type userInterface::menu()
{
	switch (choose(list<string>{"�鿴��֯�ܹ�", "������Ա", "��ʾĳ����Ա", "��Ա����", "�����֯�ܹ�", "�����Ա��Ϣ", "�������ݸ���", "�޸�����"}, "���沢�˳�"))
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
	cout << *(db->getcurrent());//��ӡ�ò�����Ϣ
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
			cout << "��ѡ�� " << goal_child->getname() << "[����]" << endl;
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
				cout << "��ǰΪ���ڵ㣬�޷������ϼ����š�" << endl;
			return view_structure_t;
		default: return menu_t;
		}
	}
}

userInterface::interface_type userInterface::add_new_dept()
{
	cout << "�������²��ŵ����ƣ�";
	string dept_name;
	std::getline(cin, dept_name);
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
			cerr << ex.info() << endl;
			cout << db->getcurrent()->getname() << "���Ѵ��ڲ��š�" << dept_name << "�����Ƿ��������벿�����ƣ�" << endl;
			switch (choose(list<string>{"�������벿������"}, "����" + db->getcurrent()->getname()))
			{
			case 0: 
				cout << "�����������²��ŵ����ƣ�";
				std::getline(cin, dept_name);
				continue;
			default: return view_structure_t;
			}
		}
		break;
	}
	cout << "������ӳɹ���" << endl;
	db->to_child(new_dept);
	return view_structure_t;
}

userInterface::interface_type userInterface::rename_dept()
{
	if (db->getcurrent()->getnodetype() != node::NodeType::Department)
		throw NodeTypeException(node::NodeType::Department);
	cout << "���������ƣ�";
	string new_dept_name;
	std::getline(cin, new_dept_name);
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
			cerr << ex.info() << endl;
			cout << db->getcurrent()->getname() << "���Ѵ��ڲ��š�" << new_dept_name << "�����Ƿ��������벿�����ƣ�" << endl;
			switch (choose(list<string>{"�������벿������"}, "����" + db->getcurrent()->getname()))
			{
			case 0:
				cout << "�����������²��ŵ����ƣ�";
				std::getline(cin,  new_dept_name);
				continue;
			default: return view_structure_t;
			}
		}
		break;
	}
	cout << "�����������ɹ���" << endl;
	return view_structure_t;
}

userInterface::interface_type userInterface::move_dept()
{
	cout << "��ѡ��Ҫ�ƶ����Ĳ��š�" << endl;
	auto n_dept = db->getcurrent();
	auto original_father = n_dept->getfather();
	n_dept->setfather(shared_ptr<department>());//�Ƚ�n_dept����֯���룬��ֹ��֯�ɻ�
	while (db->to_father());//��db��current�Ƶ����ڵ�
	set<shared_ptr<node>, defaultNodeCmp> all_child;
	list<string> choice_list;
	int choice, dept_num;
	while (1)
	{
		cout << "��ǰ������ţ�" << db->getcurrent()->getname() << endl;
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
				cerr << ex.info() << endl;
				cout << "���� " << db->getcurrent()->getname() << " ������ͬ�����ţ��ƶ�����ʧ�ܡ�" << endl;
				n_dept->setfather(original_father);//��n_dept�������ӻ�ԭλ��
				break;
			}
			cout << "�ƶ����ųɹ���" << endl;
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
	cout << "�Ƿ�Ҫɾ������ " << db->getcurrent()->getname() << " ���������������ź���Ա��" << endl;
	switch (choose(list<string>{"��"}, "��"))
	{
	case 0: 
		{
			auto temp = db->getcurrent();
			db->to_father();
			temp->setfather(shared_ptr<department>());
		}
		cout << "ɾ�����ųɹ���" << endl;
		break;
	default:break;
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::clear_structure()
{
	cout << "����Ҫִ�������֯�ܹ����������������룺";
	string password = input_password();
	try
	{
		db->clear_structure(password);
	}
	catch (const PasswordException& ex)//�������쳣��˵���������
	{
		cout << "�������ȡ�������֯�ܹ�������" << endl;
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::add_people()
{
	cout << "��ѡ������Ա�����ͣ�";
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
	cout << "����������Ա��������";
	string name;
	std::getline(cin, name);
	string id_card;
	shared_ptr<People> new_people;
	while (1)
	{
		cout << "����������Ա�����֤�ţ�";
		std::getline(cin, id_card);
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
			cout << ex.info() << endl;
			cout << "���֤�Ų��Ϸ����Ѵ��ڡ��Ƿ��������룿";
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
		cout << "����������Ա����н��Ԫ����";
		unsigned int pay;
		while (!(cin >> pay))
		{
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			cerr << "�������Ϊ�Ǹ����������������룺";
		}
		auto new_teacher = dynamic_pointer_cast<teacher, People>(new_people);
		new_teacher->setpay(pay);
	}
	auto cur_dept = dynamic_pointer_cast<department, node>(db->getcurrent());
	new_people->setfather(cur_dept);
	cout << "��Ա��ӳɹ���" << endl;
	db->to_child(new_people);
	return view_people_t;
}