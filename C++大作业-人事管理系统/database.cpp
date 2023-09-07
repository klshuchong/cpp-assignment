//database.cpp
//Created by Wang Jie (2022010554)
//database类实现
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
	//生成文件路径
	int last_slash_index = filename.find_last_of('\\', filename.length());
	string raw_filename;//不包含路径的文件名
	if (last_slash_index == std::string::npos)
		raw_filename = filename;
	else raw_filename = string(filename, last_slash_index + 1);
	
	//创建文件夹
	if (opentype == rtype::Create)
	{
		if (_mkdir(filename.c_str()) != 0)//若创建文件夹失败
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

	if (opentype == rtype::Create)//若新建文件
	{
		//新建密码文件
		ofstream wpassword(path[8], ios::binary);
		if (!wpassword.good())
		{
			wpassword.close();
			throw FileException(path[8]);
		}

		//由密码计算哈希值
		auto digest = Ly::Sha256::getInstance().encrypt(std::vector<uint8_t>(password.begin(), password.end()));
		uint8_t* digest_arr = new uint8_t[32];
		for (int i = 0; i < 32; i++)
			digest_arr[i] = digest[i];

		//将密码写入文件
		wpassword.write((char*)digest_arr, digest.size() * sizeof(uint8_t));
		
		wpassword.close();
		delete[] digest_arr;

		//创建根节点并初始化current
		root = make_shared<department>(raw_filename);
		current = root;
	}
	else //若打开已有文件
	{
		//打开密码文件
		ifstream rpassword(path[8], ios::binary);
		if (!rpassword.good())
		{
			rpassword.close();
			throw FileException(path[8]);
		}

		//读取之前储存的密码
		rpassword.read((char*)password_hash, 32);

		//验证密码
		if (!verify_password(password))//若密码错误
		{
			rpassword.close();
			throw PasswordException();
		}
		rpassword.close();

		//打开剩余文件
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

		//读取node::max_uid
		node::setmaxuid(readfile[0]);
		readfile[0].close();

		//创建一个储存所有部门的链表
		list<shared_ptr<department>> all_departments;

		//读取部门
		while (readfile[1].peek() != EOF)
			all_departments.emplace_back(make_shared<department>(readfile[1]));
		readfile[1].close();

		//检查根节点是否存在且唯一，以及各个部门的父节点是否存在且唯一，并完成树结构的构建
		bool find_root = false;
		bool find_father = false;
		for (auto itr = all_departments.begin(); itr != all_departments.end(); itr++)
		{
			if ((*itr)->getfatheruid() == -1)//父节点uid为-1，说明是根节点
			{
				if (find_root)//如果之前找到了根节点，说明有多个根节点，处理善后后抛异常
				{
					for (int i = 2; i < 8; i++)
						readfile[i].close();
					throw DataException("multiple roots in " + path[1]);
				}
				else//若之前未找到根节点，则当前节点为根节点
				{
					root = *itr;
					find_root = true;
				}
			}
			else//该节点不是根节点
			{
				//找父节点
				if ((*itr)->getfatheruid() == (*itr)->getuid())//如果该节点的父节点是它自己，处理善后后抛异常
				{
					for (int i = 2; i < 8; i++)
						readfile[i].close();
					throw DataException("father of a department is itself");
				}
				find_father = false;
				for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
				{
					if ((*itr2)->getuid() == (*itr)->getfatheruid())//若找到父节点
					{
						if (find_father)//如果之前找到了父节点，说明有多个父节点，处理善后后抛异常
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
				if (!find_father)//若没找到父节点，处理善后后抛异常
				{
					for (int i = 2; i < 8; i++)
						readfile[i].close();
					throw DataException("father of a department not found");
				}
			}
		}
		if (!find_root)//若没找到根节点，处理善后后抛异常
		{
			for (int i = 2; i < 8; i++)
				readfile[i].close();
			throw DataException("root not found in " + path[1]);
		}

		//读取People::max_people_uid
		People::setmaxpeopleuid(readfile[2]);
		readfile[2].close();

		//读取学生
		list<shared_ptr<student>> all_students;
		while (readfile[3].peek() != EOF)
			all_students.emplace_back(make_shared<student>(readfile[3]));
		readfile[3].close();

		//检查学生的所属部门是否存在且唯一，并将学生链接入组织架构
		for (auto itr = all_students.begin(); itr != all_students.end(); itr++)
		{
			//找父节点
			find_father = false;
			for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
			{
				if ((*itr2)->getuid() == (*itr)->getfatheruid())//若找到父节点
				{
					if (find_father)//如果之前找到了父节点，说明有多个父节点，处理善后后抛异常
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
			if (!find_father)//若没找到父节点，处理善后后抛异常
			{
				for (int i = 4; i < 8; i++)
					readfile[i].close();
				throw DataException("father of a student not found");
			}
		}

		//读取教师
		list<shared_ptr<teacher>> all_teachers;
		while (readfile[5].peek() != EOF)
			all_teachers.emplace_back(make_shared<teacher>(readfile[5]));
		readfile[5].close();

		//检查教师的所属部门是否存在且唯一，并将教师链接入组织架构
		for (auto itr = all_teachers.begin(); itr != all_teachers.end(); itr++)
		{
			//找父节点
			find_father = false;
			for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
			{
				if ((*itr2)->getuid() == (*itr)->getfatheruid())//若找到父节点
				{
					if (find_father)//如果之前找到了父节点，说明有多个父节点，处理善后后抛异常
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
			if (!find_father)//若没找到父节点，处理善后后抛异常
			{
				readfile[4].close();
				readfile[6].close();
				readfile[7].close();
				throw DataException("father of a teacher not found");
			}
		}

		//读取教授
		list<shared_ptr<prof>> all_profs;
		while (readfile[6].peek() != EOF)
			all_profs.emplace_back(make_shared<prof>(readfile[6]));
		readfile[6].close();

		//检查教授的所属部门是否存在且唯一，并将教授链接入组织架构
		for (auto itr = all_profs.begin(); itr != all_profs.end(); itr++)
		{
			//找父节点
			find_father = false;
			for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
			{
				if ((*itr2)->getuid() == (*itr)->getfatheruid())//若找到父节点
				{
					if (find_father)//如果之前找到了父节点，说明有多个父节点，处理善后后抛异常
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
			if (!find_father)//若没找到父节点，处理善后后抛异常
			{
				readfile[4].close();
				readfile[7].close();
				throw DataException("father of a prof not found");
			}
		}

		//读取研究生和助教
		list<shared_ptr<graduate>> all_graduate_and_ta;
		while (readfile[4].peek() != EOF)
			all_graduate_and_ta.emplace_back(make_shared<graduate>(readfile[4]));
		readfile[4].close();
		while (readfile[4].peek() != EOF)
			all_graduate_and_ta.emplace_back(make_shared<graduate>(readfile[7]));
		readfile[7].close();

		//检查研究生和助教的所属部门是否存在且唯一，并将他们链接入组织架构，同时检查本来有导师的研究生的导师是否真的存在
		bool find_advisor = false;
		for (auto itr = all_graduate_and_ta.begin(); itr != all_graduate_and_ta.end(); itr++)
		{
			//找父节点
			find_father = false;
			for (auto itr2 = all_departments.begin(); itr2 != all_departments.end(); itr2++)
			{
				if ((*itr2)->getuid() == (*itr)->getfatheruid())//若找到父节点
				{
					if (find_father)//如果之前找到了父节点，说明有多个父节点，处理善后后抛异常
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
			if (!find_father)//若没找到父节点，处理善后后抛异常
			{
				throw DataException("father of a prof not found");
			}
			if ((*itr)->getadvisoruid() != -1)//若该研究生原来有导师
			{
				find_advisor = false;
				for (auto itr3 = all_profs.begin(); itr3 != all_profs.end(); itr3++)
				{
					if ((*itr3)->getuid() == (*itr)->getadvisoruid())//若找到导师
					{
						if (find_advisor)//说明发现多个导师，抛异常
							throw DataException("a graduate/TA has multiple advisors");
						else
						{
							(*itr)->assign_advisor(*itr3);
							find_advisor = true;
						}
					}
				}
				if (!find_advisor)//若没找到导师，抛异常
					throw DataException("advisor of a graduate/TA not found");
			}
		}

		//至此，整个树结构构建完毕
		//初始化current
		current = root;
	}
}

void database::add_new_dept(std::shared_ptr<department>& n_dept)
{
	if (n_dept)
	{
		if (current->getnodetype() != node::NodeType::Department)
			throw NodeTypeException(node::NodeType::Department);
		auto cur_dept = dynamic_pointer_cast<department, node>(current);//current强转为department型智能指针
		if (!n_dept->setfather(cur_dept))
			throw SameNameDepartmentException(n_dept->getname());
	}
}

void database::rename_dept(shared_ptr<department>& n_dept, const string& n_name)
{
	if (n_dept)
	{
		auto all_child = current->get_child();
		if (all_child.find(n_dept) == all_child.end())//若n_dept不是current的子节点，抛异常
			throw NotAChildException(n_dept->getname());
		//检查是否有重名部门，若有，抛异常
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			if ((*itr)->getname() == n_name && (*itr) != n_dept)
				throw SameNameDepartmentException(n_name);
		}
		//更名
		n_dept->setfather(shared_ptr<department>());
		n_dept->setname(n_name);
		n_dept->setfather(dynamic_pointer_cast<department, node>(current));
	}
}

void database::delete_dept(shared_ptr<department>& n_dept)
{
	if (n_dept)
	{
		if (current->get_child().find(n_dept) == current->get_child().end())//若n_dept不是current的子节点，抛异常
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

	//更改信息
	auto cur_people = dynamic_pointer_cast<People, node>(current);
	if (n_id_card.length() > 0)cur_people->setidcard(n_id_card);//此步可能因身份证号重复抛异常
	if (n_name.length() > 0)cur_people->setname(n_name);
	if ((int)(cur_people->getnodetype()) >= 5)//若为教师类人员
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
		(*itr)->setfather(shared_ptr<department>()); //将根节点的所有孩子节点脱离，这样函数体结束之后孩子节点就会自动被释放
	current = root;//重置current
}

void database::clear_people(const std::string& password)
{
	if (!verify_password(password))
		throw PasswordException();
	std::cout << "Warning: clearing all people" << std::endl;
	clear_people(root);
	current = root;//重置current
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
		//生成文件路径
		int last_slash_index = n_filename.find_last_of('\\', n_filename.length());
		string raw_filename;//不包含路径的文件名
		if (last_slash_index == std::string::npos)
			raw_filename = n_filename;
		else raw_filename = string(n_filename, last_slash_index + 1);

		//创建文件夹
		if (_mkdir(n_filename.c_str()) != 0)//若创建文件夹失败
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
		
		//保存密码
		ofstream writepwd(n_path[8], ios::binary);
		if (!writepwd.good())
		{
			writepwd.close();
			throw FileException(n_path[8]);
		}
		writepwd.write((char*)password_hash, 32 * sizeof(uint8_t));
		writepwd.close();
	}

	//构建ofstream类对象的vector
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

	//写node::max_uid和People::max_people_uid
	unsigned int max_uid = node::getmaxuid();
	unsigned int max_people_uid = People::getmaxpeopleuid();
	writefile[0].write((char*)&max_uid, sizeof(unsigned int));
	writefile[2].write((char*)&max_people_uid, sizeof(unsigned int));

	//把整个树写到文件里
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
	//先写子树根节点本身
	subroot->fwrite(writefile[(unsigned int)subroot->getnodetype()]);

	//再写各个子树（若有）
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
		ans.insert(ans.end(), temp.begin(), temp.end());//在ans的最后插入子树查找的结果
	}
	if (int(subroot->getnodetype()) >= 2)//如果subroot是人员
	{
		if (subroot->getname() == n_name)//如果姓名符合
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