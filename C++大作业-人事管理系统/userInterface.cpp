//userInterface.cpp
//Created by Wang Jie (2022010554)
//userInterface类实现
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
	catch (const std::exception& ex)
	{
		errs << ex.what() << endl;
		return;
	}

	//显示欢迎界面
	welcome();

	//主要控制流程
	interface_type current_itf = menu_t; //当前界面类型，初值为主菜单
	try
	{
		while (current_itf != exit_t)
		{
			os << string(32, '=') << endl;//输出32个=
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

	//退出程序
	this->exit();
}

int userInterface::choose(const list<string>& msg, const string& quit_msg)
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
	//清除缓冲区，否则之后调用getline()时没有输入机会
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
	std::getline(is, filename);
	os << "请设置密码：";
	string password = input_password();
	os << endl << "请再次输入密码：";
	string password2 = input_password();
	//若两次输入的密码不一致，则要求重新输入
	while (password != password2)
	{
		os << endl;
		errs << "两次输入的密码不一致。" << endl;
		os << "请重新设置密码：";
		password = input_password();
		os << endl << "请再次输入密码：";
		password2 = input_password();
	}
	os << endl;

	//创建数据库
	db = make_shared<database>(filename, password, database::Create);
}

void userInterface::open()
{
	os << "请输入文件名（可包含路径）：";
	std::getline(is, filename);
	os << "请输入密码：";
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
			os << "文件打开失败。请检查文件路径和密码是否正确。" << endl;
			switch (choose(list<string>{"重新输入"}))
			{
			case -1:
				throw MyException("");//抛一个异常，这样就能进入start()的异常处理程序从而结束程序
			case 0:
				os << "请输入文件名（可包含路径）：";
				std::getline(is, filename);
				os << "请输入密码：";
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
	os << "欢迎使用人事管理系统！" << endl;
	os << "Creator: 王捷（学号：2022010554）" << endl;
	os << "公司名称：" << db->getcurrent()->getname() << endl << endl;
}

userInterface::interface_type userInterface::menu()
{
	switch (choose(list<string>{"查看组织架构", "查找人员", "显示某类人员", "人员排序", "清空组织架构", "清空人员信息", "保存/另存为", "修改密码"}, "保存并退出"))
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
	while (db->to_father());//将db的current移到根节点
	n_dept->setfather(shared_ptr<department>());//将n_dept从组织脱离，防止组织成环
	set<shared_ptr<node>, defaultNodeCmp> all_child;
	list<string> choice_list;
	int choice, dept_num;
	while (1)
	{
		choice_list.clear();
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
	catch (const PasswordException&)//若捕获异常，说明密码错误
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
			errs << ex.info() << endl;
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

userInterface::interface_type userInterface::view_people()
{
	if (db->getcurrent()->getnodetype() == node::Department)
		throw NodeTypeException(node::People);
	os << *(db->getcurrent());
	list<string> choice_list{ "更新人员信息", "改变所属部门", "查看人员所在部门", "删除人员" };
	if (db->getcurrent()->getnodetype() == node::Graduate || db->getcurrent()->getnodetype() == node::Ta)//如果该人员为研究生或助教
	{
		choice_list.push_back("查看导师信息");
		choice_list.push_back("设置导学关系");
		choice_list.push_back("解除导学关系");
		switch (choose(choice_list, "返回主菜单"))
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
			os << "请按任意键继续...";
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
		choice_list.push_back("查看学生信息");
		switch (choose(choice_list, "返回主菜单"))
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
			os << "请按任意键继续...";
			is.get();
			return view_people_t;
		}
		default: return menu_t;
		}
	}
	else
	{
		switch (choose(choice_list, "返回主菜单"))
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
	os << "选择查找方式：" << endl;
	switch (choose(list<string>{"按姓名查找", "按身份证号查找", "按编号查找"}, "返回主菜单"))
	{
	case 0:
	{
		os << "输入姓名：";
		string name;
		std::getline(is, name);
		auto result = db->search_people_by_name(name);
		if (result.empty())
		{
			os << "系统中未查找到名为 " << name << " 的人员。" << endl;
			
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
		os << "输入身份证号：";
		string id_card;
		std::getline(is, id_card);
		auto result = db->search_people_by_id_card(id_card);
		if (result)os << *result << endl;
		else os << "系统中找不到身份证号为 " << id_card << " 的人员。" << endl;
	}
	break;
	case 2:
	{
		os << "输入人员编号：";
		int people_uid;
		while (!(is >> people_uid))
		{
			is.clear();
			is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			errs << "输入的必须为整数。请重新输入：";
		}
		auto result = db->search_people_by_people_uid(people_uid);
		if (result)os << *result << endl;
		else os << "系统中找不到编号为 " << people_uid << " 的人员。" << endl;
	}
	break;
	default: return menu_t;
	}
	os << "请按任意键继续...";
	is.get();
	return menu_t;
}

userInterface::interface_type userInterface::view_all_people()
{
	os << "请选择要查看的人员类型：" << endl;
	list<shared_ptr<People>> people_list;
	switch (choose(list<string>{"学生", "研究生", "教师", "教授", "助教"}, "返回主菜单"))
	{
	case 0: people_list = db->sort_people_by_people_uid(node::Student); break;
	case 1: people_list = db->sort_people_by_people_uid(node::Graduate); break;
	case 2: people_list = db->sort_people_by_people_uid(node::Teacher); break;
	case 3: people_list = db->sort_people_by_people_uid(node::Prof); break;
	case 4: people_list = db->sort_people_by_people_uid(node::Ta); break;
	default: return menu_t;
	}
	if (people_list.empty())
		os << "系统中暂无属于您查找的类型的人员。" << endl;
	else
	{
		for (auto itr = people_list.begin(); itr != people_list.end(); itr++)
			os << **itr << endl;
	}
	os << "请按任意键继续...";
	is.get();
	return menu_t;
}

userInterface::interface_type userInterface::update_people()
{
	os << "请按照系统提示输入新的人员信息，若空白则表示不更改。" << endl;
	string original_name = db->getcurrent()->getname();
	os << "新的姓名：";
	string name;
	std::getline(is, name);
	string id_card;
	while (1)
	{
		os << "新的身份证号：";
		std::getline(is, id_card);
		try
		{
			db->update_people_info(name, id_card);
		}
		catch (const IDException& ex)
		{
			errs << ex.info() << endl;
			os << "身份证号不合法或已存在。是否重新输入？" << endl;
			switch (choose(list<string>{"是"}, "撤销更改并返回人员视图"))
			{
			case 0: continue;
			default: 
				db->update_people_info(original_name);
				return view_people_t;
			}
		}
		break;
	}
	if ((int)(db->getcurrent()->getnodetype()) >= 5)//如果当前人员为教师类
	{
		os << "月薪：";
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
	os << "人员信息修改成功。" << endl;
	return view_people_t;
}

userInterface::interface_type userInterface::move_people()
{
	os << "请选择要移动到的部门。" << endl;
	auto n_people = db->getcurrent();
	auto original_father = n_people->getfather();
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
		if (db->getcurrent()->getfather())
			choice_list.push_back("返回 " + db->getcurrent()->getfather()->getname());
		choice = choose(choice_list, "取消移动人员");
		if (choice == -1)break;
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
			auto n_people_people = dynamic_pointer_cast<People, node>(n_people);
			db->add_new_people(n_people_people);
			os << "移动人员成功。" << endl;
			break;
		}
		else if (choice == dept_num + 1)//若选择转到上级部门
		{
			db->to_father();
			continue;
		}
	}
	return view_people_t;
}

userInterface::interface_type userInterface::remove_people()
{
	os << "您确定要删除人员 " << db->getcurrent()->getname() << " 吗？";
	switch (choose(list<string>{"是"}))
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
		os << "该研究生已有导师，请先解除与原导师的关系后再设置导师。" << endl;
		return view_people_t;
	}

	os << "请选择导师。" << endl;
	while (db->to_father());//将db的current移到根节点
	set<shared_ptr<node>, defaultNodeCmp> all_child;
	list<string> choice_list;
	vector<shared_ptr<prof>> prof_vec;
	int choice, dept_num;
	while (1)
	{
		os << "当前浏览部门：" << db->getcurrent()->getname() << endl;
		all_child = db->getcurrent()->get_child();
		dept_num = 0;
		prof_vec.clear();
		for (auto itr = all_child.begin(); itr != all_child.end(); itr++)
		{
			if ((*itr)->getnodetype() == node::NodeType::Department)
			{
				choice_list.push_back("转到 " + (*itr)->getname());
				dept_num++;
			}
			else if ((*itr)->getnodetype() == node::NodeType::Prof)
			{
				choice_list.push_back("设置导师为 " + (*itr)->getname());
				prof_vec.push_back(dynamic_pointer_cast<prof, node>(*itr));
			}
		}
		if (db->getcurrent()->getfather())
			choice_list.push_back("返回 " + db->getcurrent()->getfather()->getname());
		choice = choose(choice_list, "取消设置导师");
		if (choice == -1)break;
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
		else if (choice < dept_num + prof_vec.size())//若选择了某个导师
		{
			db->assign_advisor(prof_vec[choice - dept_num]);
			os << "设置导师成功。" << endl;
			break;
		}
		else if (choice == dept_num + prof_vec.size() + 1)//若选择转到上级部门
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
		os << "该研究生暂无导师。" << endl;
		return view_people_t;
	}
	os << "您确定要解除 " << cur_gra->getname() << " 与 " << cur_gra->getadvisor()->getname() <<" 的导学关系吗？";
	switch (choose(list<string>{"是"}))
	{
	case 0:
		db->remove_advisor();
		os << "解除导学关系成功。" << endl;
	}
	return view_people_t;
}

userInterface::interface_type userInterface::sort_people()
{
	os << "请选择排序方式。";
	node::NodeType n_type;
	int choice;
	switch (choose(list<string>{"按编号排序", "按月薪排序"}))
	{
	case 0: 
	{
		list<shared_ptr<People>> result;
		os << "请选择被排序人员类型。";
		choice = choose(list<string>{"学生", "研究生", "教师", "教授", "助教"});
		if (choice >= 0)
		{
			n_type = (node::NodeType)(choice + 3);
			os << "请选择排序顺序。";
			switch (choose(list<string>{"升序", "降序"}))
			{
			case 0: 
				result = db->sort_people_by_people_uid(n_type, true);
				if (result.size() > 0)
				{
					for (auto itr = result.begin(); itr != result.end(); itr++)
						os << **itr << endl;
				}
				else os << "系统中找不到该类人员。" << endl;
				break;
			case 1:
				result = db->sort_people_by_people_uid(n_type, false);
				if (result.size() > 0)
				{
					for (auto itr = result.begin(); itr != result.end(); itr++)
						os << **itr << endl;
				}
				else os << "系统中找不到该类人员。" << endl;
				break;
			}
		}
	}
	break;
	case 1:
	{
		list<shared_ptr<teacher>> result;
		os << "请选择被排序人员类型。";
		choice = choose(list<string>{"教师", "教授", "助教"});
		if (choice >= 0)
		{
			n_type = (node::NodeType)(choice + 5);
			os << "请选择排序顺序。";
			switch (choose(list<string>{"升序", "降序"}))
			{
			case 0:
				result = db->sort_teacher_by_pay(n_type, true);
				if (result.size() > 0)
				{
					for (auto itr = result.begin(); itr != result.end(); itr++)
						os << **itr << endl;
				}
				else os << "系统中找不到该类人员。" << endl;
				break;
			case 1:
				result = db->sort_teacher_by_pay(n_type, false);
				if (result.size() > 0)
				{
					for (auto itr = result.begin(); itr != result.end(); itr++)
						os << **itr << endl;
				}
				else os << "系统中找不到该类人员。" << endl;
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
	os << "您将要执行清除人员操作，请输入密码：";
	string password = input_password();
	try
	{
		db->clear_people(password);
	}
	catch (const PasswordException&)//若捕获异常，说明密码错误
	{
		os << "密码错误，取消清除组织架构操作。" << endl;
	}
	return view_structure_t;
}

userInterface::interface_type userInterface::save_new()
{
	os << "请输入新的文件名（可包含路径），若输入为空表示保存到原文件夹：";
	string n_filename;
	std::getline(is, n_filename);
	try
	{
		if (n_filename == filename)db->save();
		else db->save(n_filename);
	}
	catch (const FileException& ex)
	{
		errs << ex.info() << endl << "数据保存失败。" << endl;
		return menu_t;
	}
	os << "数据已成功保存到文件夹" << n_filename << "。" << endl;
	return menu_t;
}

userInterface::interface_type userInterface::change_password()
{
	os << "请输入原来的密码：";
	string original_pwd = input_password();
	if (!(db->verify_password(original_pwd)))
	{
		errs << "密码错误，修改密码操作结束。" << endl;
		return menu_t;
	}
	os << "请输入新密码：";
	string new_pwd = input_password();
	os << "请再次输入新密码：";
	string new_pwd2 = input_password();
	if (new_pwd2 != new_pwd)
	{
		errs << "两次输入的密码不一致，修改密码操作结束。" << endl;
		return menu_t;
	}
	db->change_password(new_pwd);
	os << "修改密码成功。" << endl;
	return menu_t;
}

void userInterface::exit()
{
	if (db)//如果已经创建了数据库
	{
		os << "正在保存数据..." << endl;
		try
		{
			db->save();
		}
		catch (const FileException& ex)
		{
			errs << ex.info() << endl;
			errs << "数据保存失败。" << endl;
			return;
		}
		os << "数据已成功保存到文件夹" << filename << "。" << endl;
	}
	os << "感谢您使用本系统，下次再见！" << endl;
}