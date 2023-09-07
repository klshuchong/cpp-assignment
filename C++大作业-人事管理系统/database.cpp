//database.cpp
//Created by Wang Jie (2022010554)
//database��ʵ��
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <direct.h>
#include "base.h"
#include "Derived_people.h"
#include "database.h"
#include "Exception.h"
#include "sha256.h"

using std::string;
using std::ios;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::list;
using std::set;
using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;

database::database(const string& n_filename, const string& password, rtype n_opentype) :filename(n_filename), opentype(n_opentype)
{
	//�����ļ�·��
	int last_slash_index = filename.find_last_of('\\', filename.length());
	string raw_filename;//������·�����ļ���
	if (last_slash_index == std::string::npos)
		raw_filename = filename;
	else raw_filename = string(filename, last_slash_index + 1);
	
	//�����ļ���
	if (opentype == rtype::Create)
	{
		if (_mkdir(filename.c_str()) != 0)//�������ļ���ʧ��
			throw FileException(filename);
	}
	
	string path_com = filename + "\\" + raw_filename;
	path.emplace_back(path_com + "_node.dat");
	path.emplace_back(path_com + "_department.dat");
	path.emplace_back(path_com + "_people.dat");
	path.emplace_back(path_com + "_student.dat");
	path.emplace_back(path_com + "_graduate.dat");
	path.emplace_back(path_com + "_teacher.dat");
	path.emplace_back(path_com + "_prof.dat");
	path.emplace_back(path_com + "_ta.dat");
	path.emplace_back(path_com + "_password.dat");

	if (opentype == rtype::Create)//���½��ļ�
	{
		//�½������ļ�
		ofstream wpassword(path[8], ios::binary);
		if (!wpassword.good())
		{
			wpassword.close();
			throw FileException(path[8]);
		}

		//����������ϣֵ
		auto digest = Ly::Sha256::getInstance().encrypt(std::vector<uint8_t>(password.begin(), password.end()));
		uint8_t* digest_arr = new uint8_t[32];
		for (int i = 0; i < 32; i++)
			digest_arr[i] = digest[i];

		//������д���ļ�
		wpassword.write((char*)digest_arr, digest.size() * sizeof(uint8_t));
		
		wpassword.close();
		delete[] digest_arr;

		//�������ڵ㲢��ʼ��current
		root = make_shared<department>(raw_filename);
		current = root;
	}
	else //���������ļ�
	{
		//�������ļ�
		ifstream rpassword(path[8], ios::binary);
		if (!rpassword.good())
		{
			rpassword.close();
			throw FileException(path[8]);
		}

		//��ȡ֮ǰ���������
		rpassword.read((char*)password_hash, 32);

		//��֤����
		if (!verify_password(password))//���������
		{
			rpassword.close();
			throw PasswordException();
		}
		rpassword.close();

		//��ʣ���ļ�
		vector<ifstream> readfile;
		for (int i = 0; i < 8; i++)
		{
			readfile.emplace_back(ifstream(path[i], ios::binary));
			if (!readfile[i].good())
			{
				for (int j = i; j >= 0; j--)
					readfile[j].close();
				throw FileException(path[i]);
			}
		}

		//��ȡnode::max_uid
		node::setmaxuid(readfile[0]);
		readfile[0].close();

		//����һ���������в��ŵ�����
		list<shared_ptr<department>> all_departments;

		//��ȡ����
		while (readfile[1].peek() != EOF)
			all_departments.emplace_back(make_shared<department>(readfile[1]));
		readfile[1].close();

		//�����ڵ��Ƿ������Ψһ���Լ��������ŵĸ��ڵ��Ƿ������Ψһ����������ṹ�Ĺ���
		bool find_root = false;
		bool find_father = false;
		for (auto itr = all_departments.begin(); itr != all_departments.end(); itr++)
		{
			if ((*itr)->getfatheruid() == -1)//���ڵ�uidΪ-1��˵���Ǹ��ڵ�
			{
				if (find_root)//���֮ǰ�ҵ��˸��ڵ㣬˵���ж�����ڵ㣬�����ƺ�����쳣
				{
					for (int i = 2; i < 8; i++)
						readfile[i].close();
					throw DataException("multiple roots in " + path[1]);
				}
				else//��֮ǰδ�ҵ����ڵ㣬��ǰ�ڵ�Ϊ���ڵ�
				{
					root = *itr;
					find_root = true;
				}
			}
			else//�ýڵ㲻�Ǹ��ڵ�
			{
				//�Ҹ��ڵ�
				if ((*itr)->getfatheruid() == (*itr)->getuid())//����ýڵ�ĸ��ڵ������Լ��������ƺ�����쳣
				{
					for (int i = 2; i < 8; i++)
						readfile[i].close();
					throw DataException("father of a department is itself");
				}
				find_father = false;
				for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
				{
					if ((*itr2)->getuid() == (*itr)->getfatheruid())//���ҵ����ڵ�
					{
						if (find_father)//���֮ǰ�ҵ��˸��ڵ㣬˵���ж�����ڵ㣬�����ƺ�����쳣
						{
							for (int i = 2; i < 8; i++)
								readfile[i].close();
							throw DataException("a department has multiple fathers");
						}
						else
						{
							(*itr)->setfather(*itr2);
							find_father = true;
						}
					}
				}
				if (!find_father)//��û�ҵ����ڵ㣬�����ƺ�����쳣
				{
					for (int i = 2; i < 8; i++)
						readfile[i].close();
					throw DataException("father of a department not found");
				}
			}
		}
		if (!find_root)//��û�ҵ����ڵ㣬�����ƺ�����쳣
		{
			for (int i = 2; i < 8; i++)
				readfile[i].close();
			throw DataException("root not found in " + path[1]);
		}

		//��ȡPeople::max_people_uid
		People::setmaxpeopleuid(readfile[2]);
		readfile[2].close();

		//��ȡѧ��
		list<shared_ptr<student>> all_students;
		while (readfile[3].peek() != EOF)
			all_students.emplace_back(make_shared<student>(readfile[3]));
		readfile[3].close();

		//���ѧ�������������Ƿ������Ψһ������ѧ����������֯�ܹ�
		for (auto itr = all_students.begin(); itr != all_students.end(); itr++)
		{
			//�Ҹ��ڵ�
			find_father = false;
			for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
			{
				if ((*itr2)->getuid() == (*itr)->getfatheruid())//���ҵ����ڵ�
				{
					if (find_father)//���֮ǰ�ҵ��˸��ڵ㣬˵���ж�����ڵ㣬�����ƺ�����쳣
					{
						for (int i = 4; i < 8; i++)
							readfile[i].close();
						throw DataException("a student has multiple fathers");
					}
					else
					{
						(*itr)->setfather(*itr2);
						find_father = true;
					}
				}
			}
			if (!find_father)//��û�ҵ����ڵ㣬�����ƺ�����쳣
			{
				for (int i = 4; i < 8; i++)
					readfile[i].close();
				throw DataException("father of a student not found");
			}
		}

		//��ȡ��ʦ
		list<shared_ptr<teacher>> all_teachers;
		while (readfile[5].peek() != EOF)
			all_teachers.emplace_back(make_shared<teacher>(readfile[5]));
		readfile[5].close();

		//����ʦ�����������Ƿ������Ψһ��������ʦ��������֯�ܹ�
		for (auto itr = all_teachers.begin(); itr != all_teachers.end(); itr++)
		{
			//�Ҹ��ڵ�
			find_father = false;
			for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
			{
				if ((*itr2)->getuid() == (*itr)->getfatheruid())//���ҵ����ڵ�
				{
					if (find_father)//���֮ǰ�ҵ��˸��ڵ㣬˵���ж�����ڵ㣬�����ƺ�����쳣
					{
						readfile[4].close();
						readfile[6].close();
						readfile[7].close();
						throw DataException("a teacher has multiple fathers");
					}
					else
					{
						(*itr)->setfather(*itr2);
						find_father = true;
					}
				}
			}
			if (!find_father)//��û�ҵ����ڵ㣬�����ƺ�����쳣
			{
				readfile[4].close();
				readfile[6].close();
				readfile[7].close();
				throw DataException("father of a teacher not found");
			}
		}

		//��ȡ����
		list<shared_ptr<prof>> all_profs;
		while (readfile[6].peek() != EOF)
			all_profs.emplace_back(make_shared<prof>(readfile[6]));
		readfile[6].close();

		//�����ڵ����������Ƿ������Ψһ������������������֯�ܹ�
		for (auto itr = all_profs.begin(); itr != all_profs.end(); itr++)
		{
			//�Ҹ��ڵ�
			find_father = false;
			for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
			{
				if ((*itr2)->getuid() == (*itr)->getfatheruid())//���ҵ����ڵ�
				{
					if (find_father)//���֮ǰ�ҵ��˸��ڵ㣬˵���ж�����ڵ㣬�����ƺ�����쳣
					{
						readfile[4].close();
						readfile[7].close();
						throw DataException("a prof has multiple fathers");
					}
					else
					{
						(*itr)->setfather(*itr2);
						find_father = true;
					}
				}
			}
			if (!find_father)//��û�ҵ����ڵ㣬�����ƺ�����쳣
			{
				readfile[4].close();
				readfile[7].close();
				throw DataException("father of a prof not found");
			}
		}

		//��ȡ�о���������
		list<shared_ptr<graduate>> all_graduate_and_ta;
		while (readfile[4].peek() != EOF)
			all_graduate_and_ta.emplace_back(make_shared<graduate>(readfile[4]));
		readfile[4].close();
		while (readfile[4].peek() != EOF)
			all_graduate_and_ta.emplace_back(make_shared<graduate>(readfile[7]));
		readfile[7].close();

		//����о��������̵����������Ƿ������Ψһ������������������֯�ܹ���ͬʱ��鱾���е�ʦ���о����ĵ�ʦ�Ƿ���Ĵ���
		bool find_advisor = false;
		for (auto itr = all_graduate_and_ta.begin(); itr != all_graduate_and_ta.end(); itr++)
		{
			//�Ҹ��ڵ�
			find_father = false;
			for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
			{
				if ((*itr2)->getuid() == (*itr)->getfatheruid())//���ҵ����ڵ�
				{
					if (find_father)//���֮ǰ�ҵ��˸��ڵ㣬˵���ж�����ڵ㣬�����ƺ�����쳣
					{
						throw DataException("a graduate/TA has multiple fathers");
					}
					else
					{
						(*itr)->setfather(*itr2);
						find_father = true;
					}
				}
			}
			if (!find_father)//��û�ҵ����ڵ㣬�����ƺ�����쳣
			{
				throw DataException("father of a prof not found");
			}
			if ((*itr)->getadvisoruid() != -1)//�����о���ԭ���е�ʦ
			{
				find_advisor = false;
				for (auto itr3 = all_profs.begin(); itr3 != all_profs.end(); itr3++)
				{
					if ((*itr3)->getuid() == (*itr)->getadvisoruid())//���ҵ���ʦ
					{
						if (find_advisor)//˵�����ֶ����ʦ�����쳣
							throw DataException("a graduate/TA has multiple advisors");
						else
						{
							(*itr)->assign_advisor(*itr3);
							find_advisor = true;
						}
					}
				}
				if (!find_advisor)//��û�ҵ���ʦ�����쳣
					throw DataException("advisor of a graduate/TA not found");
			}
		}

		//���ˣ��������ṹ�������
		//��ʼ��current
		current = root;
	}
}

void database::add_new_dept(std::shared_ptr<department>& n_dept)
{
	if (n_dept)
	{
		if (current->getnodetype() != node::NodeType::Department)
			throw NodeTypeException(node::NodeType::Department);
		auto cur_dept = dynamic_pointer_cast<department, node>(current);//currentǿתΪdepartment������ָ��
		if (!n_dept->setfather(cur_dept))
			throw SameNameDepartmentException(n_dept->getname());
	}
}

void database::rename_dept(shared_ptr<department>& n_dept, const string& n_name)
{
	if (n_dept)
	{
		auto all_child = current->get_child();
		if (all_child.find(n_dept) == all_child.end())//��n_dept����current���ӽڵ㣬���쳣
			throw NotAChildException(n_dept->getname());
		//����Ƿ����������ţ����У����쳣
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			if ((*itr)->getname() == n_name && (*itr) != n_dept)
				throw SameNameDepartmentException(n_name);
		}
		//����
		n_dept->setfather(shared_ptr<department>());
		n_dept->setname(n_name);
		n_dept->setfather(dynamic_pointer_cast<department, node>(current));
	}
}

void database::delete_dept(shared_ptr<department>& n_dept)
{
	if (n_dept)
	{
		if (current->get_child().find(n_dept) == current->get_child().end())//��n_dept����current���ӽڵ㣬���쳣
			throw NotAChildException(n_dept->getname());
		n_dept->setfather(shared_ptr<department>());
	}
}

void database::add_new_people(shared_ptr<People>& n_people)
{
	if (n_people)
	{
		if (current->getnodetype() != node::NodeType::Department)
			throw NodeTypeException(node::NodeType::Department);
		n_people->setfather(dynamic_pointer_cast<department, node>(current));
	}
}

void database::update_people_info(const string& n_name, const string& n_id_card, unsigned int n_pay)
{
	if ((current->getnodetype()) == node::NodeType::Department)
		throw NodeTypeException(node::NodeType::People);

	//������Ϣ
	auto cur_people = dynamic_pointer_cast<People, node>(current);
	if (n_id_card.length() > 0)cur_people->setidcard(n_id_card);//�˲����������֤���ظ����쳣
	if (n_name.length() > 0)cur_people->setname(n_name);
	if ((int)(cur_people->getnodetype()) >= 5)//��Ϊ��ʦ����Ա
	{
		auto cur_teacher = dynamic_pointer_cast<teacher, People>(cur_people);
		cur_teacher->setpay(n_pay);
	}
}

void database::delete_people()
{
	if (current->getnodetype() == node::NodeType::Department)
		throw NodeTypeException(node::NodeType::People);
	shared_ptr<node> to_be_deleted = current;
	current = current->getfather();
	to_be_deleted->setfather(shared_ptr<department>());
}

void database::assign_advisor(shared_ptr<prof> n_advisor)
{
	if (current->getnodetype() != node::NodeType::Graduate && current->getnodetype() != node::NodeType::Ta)
		throw NodeTypeException(node::NodeType::Graduate);
	auto cur_graduate = dynamic_pointer_cast<graduate, node>(current);
	cur_graduate->assign_advisor(n_advisor);
}

void database::remove_advisor()
{
	if (current->getnodetype() != node::NodeType::Graduate && current->getnodetype() != node::NodeType::Ta)
		throw NodeTypeException(node::NodeType::Graduate);
	auto cur_graduate = dynamic_pointer_cast<graduate, node>(current);
	cur_graduate->remove_advisor();
}

list<shared_ptr<People>> database::sort_people_by_people_uid(node::NodeType n_node_type, bool ascending) const
{
	if ((int)n_node_type < 2)throw NodeTypeException(node::NodeType::People);
	auto ans_set = sort_people_by_people_uid(n_node_type, root);
	auto ans = list<shared_ptr<People>>();
	if (ascending)
		ans.insert(ans.begin(), ans_set.begin(), ans_set.end());
	else
		ans.insert(ans.begin(), ans_set.rbegin(), ans_set.rend());
	return ans;
}

list<shared_ptr<teacher>> database::sort_teacher_by_pay(node::NodeType n_node_type, bool ascending) const
{
	if ((int)n_node_type < 5)throw NodeTypeException(node::NodeType::Teacher);
	auto ans_set = sort_teacher_by_pay(n_node_type, root);
	auto ans = list<shared_ptr<teacher>>();
	if (ascending)
		ans.insert(ans.begin(), ans_set.begin(), ans_set.end());
	else
		ans.insert(ans.begin(), ans_set.rbegin(), ans_set.rend());
	return ans;
}

void database::change_password(const string& n_password)
{
	auto digest = Ly::Sha256::getInstance().encrypt(std::vector<uint8_t>(n_password.begin(), n_password.end()));
	for (int i = 0; i < 32; i++)password_hash[i] = digest[i];
	ofstream write_password(path[8], ios::binary);
	if (!write_password.good())
	{
		write_password.close();
		throw FileException(path[8]);
	}
	write_password.write((char*)password_hash, 32 * sizeof(uint8_t));
	write_password.close();
}

void database::clear_structure(const string& password)
{
	if (!verify_password(password))
		throw PasswordException();
	std::cout << "Warning: clearing all structure" << std::endl;
	auto all_child = root->get_child();
	for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		(*itr)->setfather(shared_ptr<department>()); //�����ڵ�����к��ӽڵ����룬�������������֮���ӽڵ�ͻ��Զ����ͷ�
	current = root;//����current
}

void database::clear_people(const std::string& password)
{
	if (!verify_password(password))
		throw PasswordException();
	std::cout << "Warning: clearing all people" << std::endl;
	clear_people(root);
	current = root;//����current
}

void database::save(const string& n_filename) const
{
	vector<string> n_path;
	if (n_filename.empty())
	{
		n_path = path;
	}
	else
	{
		//�����ļ�·��
		int last_slash_index = n_filename.find_last_of('\\', n_filename.length());
		string raw_filename;//������·�����ļ���
		if (last_slash_index == std::string::npos)
			raw_filename = n_filename;
		else raw_filename = string(n_filename, last_slash_index + 1);

		//�����ļ���
		if (_mkdir(n_filename.c_str()) != 0)//�������ļ���ʧ��
			throw FileException(n_filename);

		string path_com = n_filename + "\\" + raw_filename;
		n_path.emplace_back(path_com + "_node.dat");
		n_path.emplace_back(path_com + "_department.dat");
		n_path.emplace_back(path_com + "_people.dat");
		n_path.emplace_back(path_com + "_student.dat");
		n_path.emplace_back(path_com + "_graduate.dat");
		n_path.emplace_back(path_com + "_teacher.dat");
		n_path.emplace_back(path_com + "_prof.dat");
		n_path.emplace_back(path_com + "_ta.dat");
		n_path.emplace_back(path_com + "_password.dat");
		
		//��������
		ofstream writepwd(n_path[8], ios::binary);
		if (!writepwd.good())
		{
			writepwd.close();
			throw FileException(n_path[8]);
		}
		writepwd.write((char*)password_hash, 32 * sizeof(uint8_t));
		writepwd.close();
	}

	//����ofstream������vector
	vector<ofstream> writefile;
	for (int i = 0; i < 8; i++)
	{
		writefile.emplace_back(ofstream(n_path[i], ios::binary));
		if (!writefile[i].good())
		{
			for (int j = i; j >= 0; j--)
				writefile[j].close();
			throw FileException(n_path[i]);
		}
	}

	//дnode::max_uid��People::max_people_uid
	unsigned int max_uid = node::getmaxuid();
	unsigned int max_people_uid = People::getmaxpeopleuid();
	writefile[0].write((char*)&max_uid, sizeof(unsigned int));
	writefile[2].write((char*)&max_people_uid, sizeof(unsigned int));

	//��������д���ļ���
	save_by_subtree(root, writefile);

	for (int i = 0; i < 8; i++)
		writefile[i].close();
}

bool database::verify_password(const string& password) const
{
	auto digest = Ly::Sha256::getInstance().encrypt(std::vector<uint8_t>(password.begin(), password.end()));
	uint8_t* digest_arr = new uint8_t[32];
	for (int i = 0; i < 32; i++)digest_arr[i] = digest[i];
	bool ans = (memcmp(digest_arr, password_hash, 32) == 0);
	delete[] digest_arr;
	return ans;
}

void database::save_by_subtree(const shared_ptr<node>& subroot, vector<ofstream>& writefile) const
{
	//��д�������ڵ㱾��
	subroot->fwrite(writefile[(unsigned int)subroot->getnodetype()]);

	//��д�������������У�
	auto next_subroot = subroot->get_child();
	if (!next_subroot.empty())
	{
		for (auto itr = next_subroot.begin(); itr != next_subroot.end(); itr++)
			save_by_subtree(*itr, writefile);
	}
}

list<shared_ptr<People>> database::search_people_by_name(const string& n_name, const shared_ptr<node>& subroot) const
{
	list<shared_ptr<People>> ans;
	auto all_child = subroot->get_child();
	list<shared_ptr<People>> temp;
	for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
	{
		temp = search_people_by_name(n_name, *itr);
		ans.insert(ans.end(), temp.begin(), temp.end());//��ans���������������ҵĽ��
	}
	if (int(subroot->getnodetype()) >= 2)//���subroot����Ա
	{
		if (subroot->getname() == n_name)//�����������
			ans.emplace_back(dynamic_pointer_cast<People, node>(subroot));
	}
	return ans;
}

shared_ptr<People> database::search_people_by_id_card(const string& n_id_card, const shared_ptr<node>& subroot) const
{
	shared_ptr<People> ans;
	auto all_child = subroot->get_child();
	if (subroot->getnodetype() == node::NodeType::Department)
	{
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			ans = search_people_by_id_card(n_id_card, *itr);
			if (ans) break;
		}
		return ans;
	}
	else
	{
		auto cur_people = dynamic_pointer_cast<People, node>(subroot);
		if (n_id_card == cur_people->getidcard())return cur_people;
		else return shared_ptr<People>();
	}
}

shared_ptr<People> database::search_people_by_people_uid(int n_people_uid, const shared_ptr<node>& subroot) const
{
	shared_ptr<People> ans;
	auto all_child = subroot->get_child();
	if (subroot->getnodetype() == node::NodeType::Department)
	{
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			ans = search_people_by_people_uid(n_people_uid, *itr);
			if (ans) break;
		}
		return ans;
	}
	else
	{
		auto cur_people = dynamic_pointer_cast<People, node>(subroot);
		if (n_people_uid == cur_people->getpeopleuid())return cur_people;
		else return shared_ptr<People>();
	}
}

set<shared_ptr<People>, defaultNodeCmp> database::sort_people_by_people_uid(node::NodeType n_node_type, const shared_ptr<node>& subroot) const
{
	if (subroot->getnodetype() == node::NodeType::Department)
	{
		auto all_child = subroot->get_child();
		auto ans = set<shared_ptr<People>, defaultNodeCmp>();
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			auto temp = sort_people_by_people_uid(n_node_type, *itr);
			ans.insert(temp.begin(), temp.end());
		}
		return ans;
	}
	else if (subroot->getnodetype() == n_node_type)
	{
		auto ans = set<shared_ptr<People>, defaultNodeCmp>();
		ans.insert(dynamic_pointer_cast<People, node>(subroot));
		return ans;
	}
	else return set<shared_ptr<People>, defaultNodeCmp>();
}

set<shared_ptr<teacher>, teacherCmpByPay> database::sort_teacher_by_pay(node::NodeType n_node_type, const shared_ptr<node>& subroot) const
{
	if (subroot->getnodetype() == node::NodeType::Department)
	{
		auto all_child = subroot->get_child();
		auto ans = set<shared_ptr<teacher>, teacherCmpByPay>();
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			auto temp = sort_teacher_by_pay(n_node_type, *itr);
			ans.insert(temp.begin(), temp.end());
		}
		return ans;
	}
	else if (subroot->getnodetype() == n_node_type)
	{
		auto ans = set<shared_ptr<teacher>, teacherCmpByPay>();
		ans.insert(dynamic_pointer_cast<teacher, node>(subroot));
		return ans;
	}
	else return set<shared_ptr<teacher>, teacherCmpByPay>();
}

void database::clear_people(std::shared_ptr<node> subroot)
{
	auto all_child = subroot->get_child();
	for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
	{
		if ((*itr)->getnodetype() == node::NodeType::Department)
			clear_people(*itr);
		else (*itr)->setfather(shared_ptr<department>());
	}
}