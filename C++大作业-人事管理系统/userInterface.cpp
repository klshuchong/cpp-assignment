//userInterface.cpp
//Created by Wang Jie (2022010554)
//userInterface类实现
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

using std::endl;
using std::string;
using std::list;
using std::set;
using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;

void userInterface::start()
{
	//启动界面
	os << "==========人事管理系统（学校版）==========" << endl;
	os << "您想要创建新文件还是打开已有文件？" << endl;
	try
	{
		switch (choose(list<string>{"创建新文件", "打开已有文件"}))
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
	//处理异常
	catch (const MyException& ex)
	{
		errs << ex.info() << endl;
		return;
	}

	//显示欢迎界面
	welcome();

	//主要控制流程
	interface_type current_itf = menu_t; //当前界面类型，初值为主菜单
	try
	{
		os << string(32, '=');//输出32个=
		while (current_itf != exit_t)
			current_itf = (this->*itf[(int)current_itf])();
	}
	catch (const MyException& ex)
	{
		errs << ex.info() << endl;
		return;
	}

	//退出程序
	this->exit();
}

int userInterface::choose(const list<string>& msg, const string& quit_msg = "退出")
{
	//打印选项列表
	int i = 0;
	for (auto itr = msg.begin(); itr != msg.end(); itr++, i++)
		os << i << ". " << *itr << endl;
	os << i << ". " << quit_msg << endl;
	os << "请输入选项前的编号...";

	//处理输入，直到输入合法为止
	int choice; //输入的整数
	while (!(is >> choice) || choice<0 || choice>msg.size())
	{
		is.clear();
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		errs << "输入必须为不超过" << msg.size() << "的非负整数。请重新输入：";
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
		if (input == '\b')//若输入为退格
		{
			if (ans.length() > 0)
			{
				ans.pop_back();
				os << "\b \b";
			}
			else
			{
				os.put('\a');//若输入内容为空还退格，则响铃
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
	os << "请输入文件名兼公司名称（可包含路径）。请注意，公司名称设置后不可修改。";
	string filename;
	std::getline(is, filename);
	os << "请设置密码（不少于8个字符）：";
	string password = input_password();
	os << "请再次输入密码：";
	string password2 = input_password();
	//若两次输入的密码不一致，则要求重新输入
	while (password != password2)
	{
		errs << "两次输入的密码不一致。" << endl;
		os << "请重新设置密码（不少于8个字符）：";
		password = input_password();
		os << "请再次输入密码：";
		password2 = input_password();
	}

	//创建数据库
	db = make_shared<database>(filename, password, database::Create);
}

void userInterface::open()
{
	os << "请输入文件名（可包含路径）：";
	string filename;
	std::getline(is, filename);
	os << "请输入密码：";
	string password = input_password();

	while (1)
	{
		try
		{
			db = make_shared<database>(filename, password, database::Read);
		}
		catch (const MyException& ex)
		{
			errs << ex.info() << endl;
			os << "文件打开失败。是否重新选择文件？";
			switch (choose(list<string>{"重新选择文件"}))
			{
			case -1:
				throw MyException("");//抛一个异常，这样就能进入start()的异常处理程序从而结束程序
			case 0:
				os << "请输入文件名（可包含路径）：";
				string filename;
				std::getline(is, filename);
				os << "请输入密码：";
				string password = input_password();
				continue;
			}
		}
		break;
	}
}

void userInterface::welcome()
{
	os << "欢迎使用人事管理系统！" << endl;
	os << "Creator: 王捷（学号：2022010554）" << endl;
	os << "公司名称：" << db->getcurrent()->getname() << endl << endl;
}

userInterface::interface_type userInterface::menu()
{
	switch (choose(list<string>{"查看组织架构", "查找人员", "显示某类人员", "人员排序", "清空组织架构", "清空人员信息", "保存数据副本", "修改密码"}, "保存并退出"))
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
	os << *(db->getcurrent());//打印该部门信息
	list<string> choice_list;
	auto all_child = db->getcurrent()->get_child();
	for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		if ((*itr)->getnodetype() == node::NodeType::Department)
		{
			choice_list.push_back("选择 " + (*itr)->getname() + "...");
		}
		else choice_list.push_back("转到 " + (*itr)->getname());
	choice_list.push_back("新建下级部门");
	choice_list.push_back("新建直属人员");
	choice_list.push_back("返回上级部门");
	int choice = choose(choice_list, "返回主菜单");
	if (choice == -1)return menu_t;
	else if (choice < all_child.size())//如果选择了某个部门或人员
	{
		//找到对应的部门或人员
		shared_ptr<node> goal_child;
		int i = 0;
		for (auto itr = all_child.begin(); i <= choice; itr++, i++)
			goal_child = *itr;

		db->to_child(goal_child);
		if (goal_child->getnodetype() == node::NodeType::Department)//如果选择的是部门
		{
			os << "已选择 " << goal_child->getname() << "[部门]" << endl;
			switch (choose(list<string>{"转到该部门视图", "重命名该部门", "移动该部门", "删除该部门"}, "返回 " + goal_child->getfather()->getname()))
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
			if (!(db->to_father()))//若current为根节点
				os << "当前为根节点，无法返回上级部门。" << endl;
			return view_structure_t;
		default: return menu_t;
		}
	}
}

userInterface::interface_type userInterface::add_new_dept()
{
	os << "请输入新部门的名称：";
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
			os << db->getcurrent()->getname() << "下已存在部门“" << dept_name << "”。是否重新输入部门名称？" << endl;
			switch (choose(list<string>{"重新输入部门名称"}, "返回" + db->getcurrent()->getname()))
			{
			case 0: 
				os << "请重新输入新部门的名称：";
				std::getline(is, dept_name);
				continue;
			default: return view_structure_t;
			}
		}
		break;
	}
	os << "部门添加成功。" << endl;
	db->to_child(new_dept);
	return view_structure_t;
}

userInterface::interface_type userInterface::rename_dept()
{
	if (db->getcurrent()->getnodetype() != node::NodeType::Department)
		throw NodeTypeException(node::NodeType::Department);
	os << "输入新名称：";
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
			os << db->getcurrent()->getname() << "下已存在部门“" << new_dept_name << "”。是否重新输入部门名称？" << endl;
			switch (choose(list<string>{"重新输入部门名称"}, "返回" + db->getcurrent()->getname()))
			{
			case 0:
				os << "请重新输入新部门的名称：";
				std::getline(is,  new_dept_name);
				continue;
			default: return view_structure_t;
			}
		}
		break;
	}
	os << "部门重命名成功。" << endl;
	return view_structure_t;
}

userInterface::interface_type userInterface::move_dept()
{
	os << "请选择要移动到的部门。" << endl;
	auto n_dept = db->getcurrent();
	auto original_father = n_dept->getfather();
	n_dept->setfather(shared_ptr<department>());//先将n_dept从组织脱离，防止组织成环
	while (db->to_father());//将db的current移到根节点
	set<shared_ptr<node>, defaultNodeCmp> all_child;
	list<string> choice_list;
	int choice, dept_num;
	while (1)
	{
		os << "当前浏览部门：" << db->getcurrent()->getname() << endl;
		all_child = db->getcurrent()->get_child();
		dept_num = 0;
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			if ((*itr)->getnodetype() == node::NodeType::Department)
			{
				choice_list.push_back("转到 " + (*itr)->getname());
				dept_num++;
			}
			else break;
		}
		choice_list.push_back("移动到此处");
		if(db->getcurrent()->getfather())
			choice_list.push_back("返回 " + db->getcurrent()->getfather()->getname());
		choice = choose(choice_list, "取消移动部门");
		if (choice == -1)
		{
			n_dept->setfather(original_father);//将n_dept重新链接回组织
			break;
		}
		else if (choice < dept_num)//若选择转到下级部门
		{
			auto itr = all_child.begin();
			int i = 0;
			while (i < choice)//将itr移到要转到的下级部门
			{
				i++;
				itr++;
			}
			db->to_child(*itr);
			continue;
		}
		else if (choice == dept_num)//若选择移动到该部门
		{
			try
			{
				auto n_dept_dept = dynamic_pointer_cast<department, node>(n_dept);
				db->add_new_dept(n_dept_dept);
			}
			catch (const SameNameDepartmentException& ex)//若发生部门重名
			{
				errs << ex.info() << endl;
				os << "部门 " << db->getcurrent()->getname() << " 下已有同名部门，移动部门失败。" << endl;
				n_dept->setfather(original_father);//将n_dept重新链接回原位置
				break;
			}
			os << "移动部门成功。" << endl;
			break;
		}
		else if (choice == dept_num + 1)//若选择转到上级部门
		{
			db->to_father();
			continue;
		}
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::remove_dept()
{
	os << "是否要删除部门 " << db->getcurrent()->getname() << " 及其所有下属部门和人员？" << endl;
	switch (choose(list<string>{"是"}, "否"))
	{
	case 0: 
		{
			auto temp = db->getcurrent();
			db->to_father();
			temp->setfather(shared_ptr<department>());
		}
		os << "删除部门成功。" << endl;
		break;
	default:break;
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::clear_structure()
{
	os << "您将要执行清除组织架构操作，请输入密码：";
	string password = input_password();
	try
	{
		db->clear_structure(password);
	}
	catch (const PasswordException& ex)//若捕获异常，说明密码错误
	{
		os << "密码错误，取消清除组织架构操作。" << endl;
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::add_people()
{
	os << "请选择新人员的类型：";
	node::NodeType p_type;
	switch (choose(list<string>{"学生", "研究生", "教师", "教授", "助教"}))
	{
	case 0: p_type = node::Student; break;
	case 1: p_type = node::Graduate; break;
	case 2: p_type = node::Teacher; break;
	case 3: p_type = node::Prof; break;
	case 4: p_type = node::Ta; break;
	default: return view_structure_t;
	}
	os << "请输入新人员的姓名：";
	string name;
	std::getline(is, name);
	string id_card;
	shared_ptr<People> new_people;
	while (1)
	{
		os << "请输入新人员的身份证号：";
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
			os << ex.info() << endl;
			os << "身份证号不合法或已存在。是否重新输入？";
			switch (choose(list<string>{"是"}))
			{
			case 0: continue;
			default: return view_structure_t;
			}
		}
		break;
	}
	if ((int)p_type >= 5)//如果人员为教师类
	{
		os << "请输入新人员的月薪（元）：";
		unsigned int pay;
		while (!(is >> pay))
		{
			is.clear();
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			errs << "输入必须为非负整数。请重新输入：";
		}
		auto new_teacher = dynamic_pointer_cast<teacher, People>(new_people);
		new_teacher->setpay(pay);
	}
	auto cur_dept = dynamic_pointer_cast<department, node>(db->getcurrent());
	new_people->setfather(cur_dept);
	os << "人员添加成功。" << endl;
	db->to_child(new_people);
	return view_people_t;
}