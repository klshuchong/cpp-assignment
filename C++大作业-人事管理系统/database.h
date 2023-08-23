//database.h
//Created by Wang Jie (2022010554)
//database类声明
#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <memory>
#include "base.h"
#include "Derived_people.h"

// 注：数据文件组织形式
// 所有数据文件保存在一个文件夹中，该文件夹名称为[filename]
// 保存各类对象的文件名均为[filename]后加相应的扩展名：.department, .student, .graduate, .teacher, .prof, .ta
// 在每个文件中，对应的类对象是线性存储的
// 除此以外，.node文件中存储node::max_uid
// .people文件中存储People::max_people_uid
// .password文件中存储用SHA256加密后的密码

//数据库类，其中存储整个公司的组织架构（树结构）
class database
{
public:
	//表示文件打开类型的枚举，Create表示创建新文件，Read表示打开已有文件
	enum rtype { Create = 0, Read };

	//构造函数。若新建文件，则建立密码文件；若打开已有文件，则检查密码，若密码错误则抛出PasswordException异常，若密码正确则加载其余数据
	database(const std::string& n_filename, const std::string& password, rtype n_opentype = rtype::Create);

	//本程序中不需要用到复制构造函数和赋值函数，因此不隐式生成它们
	database(const database&) = delete;
	const database& operator=(const database&) = delete;

	//析构函数
	~database() = default;

	//获取当前节点
	std::shared_ptr<node> getcurrent() const { return current; }

	//将current移到其父节点，若current为根节点则返回false，否则移动current返回true
	bool to_father()
	{
		if (!current->getfather())return false;
		current = current->getfather();
	}

	//将current移到其子节点n_child，若current指向的节点不存在子节点n_child或current指向的是People则返回false，否则移动current并返回true
	bool to_child(const std::shared_ptr<node>& n_child)
	{
		auto current_child = current->get_child();
		if (current_child.find(n_child) != current_child.end())//如果查找到n_child
		{
			current = n_child;
			return true;
		}
		else return false;
	}

	//在current位置下新建部门n_dept，若出现下列情况之一则抛异常：
	//1.current指向人员
	//2.n_dept与current位置下已有部门重名
	//若n_dept为空，则不做任何事情
	//该函数也可用于移动部门
	void add_new_dept(std::shared_ptr<department>& n_dept);

	//对current位置下的n_dept部门重命名为n_name，若出现下列情况之一则抛异常:
	//1.n_dept不是current的子节点
	//2.n_name与current的其他子节点重名
	//若n_dept为空，则不做任何事情
	void rename_dept(std::shared_ptr<department>& n_dept, const std::string& n_name);

	//删除current位置下的n_dept部门及其所有下属部门和下属人员，若出现下列情况之一则抛异常:
	//1.n_dept不是current的子节点
	//若n_dept为空，则不做任何事情
	void delete_dept(std::shared_ptr<department>& n_dept);

	//在current位置下新建人员n_people，若出现下列情况之一则抛异常：
	//1.current指向人员
	//2.n_people与其他人员的身份证号相同
	//若n_people为空，则不做任何事情
	//该函数也可用于移动人员
	void add_new_people(std::shared_ptr<People>& n_people);

	//更新current指向的人员信息，若出现下列情况之一则抛异常：
	//1.current指向部门
	//2.新的身份证号与其他人员的身份证号相同
	void update_people_info(const std::string& n_name, const std::string& n_id_card, unsigned int n_pay = 0);

	//删除current指向的人员，若current指向部门则抛异常
	//进行此操作后，current变为原来所指向节点的父节点
	void delete_people();

	//设置current指向的研究生的导师为n_advisor，若出现下列情况之一则抛异常：
	//1.current指向的不是研究生或助教
	//若n_advisor为空或原有导师，则不做任何事情
	void assign_advisor(std::shared_ptr<prof> n_advisor);

	//设置current指向的研究生的导师为n_advisor，若出现下列情况之一则抛异常：
	//1.current指向的不是研究生或助教
	//若current原无导师，则不做任何事情
	void remove_advisor();

	//按姓名/身份证号/编号查找人员，返回所有查找结果
	std::list<std::shared_ptr<People>> search_people_by_name(const std::string& n_name) const { return search_people_by_name(n_name, root); }
	std::shared_ptr<People> search_people_by_id_card(const std::string& n_id_card) const { return search_people_by_id_card(n_id_card, root); }
	std::shared_ptr<People> search_people_by_people_uid(int n_people_uid) const { return search_people_by_people_uid(n_people_uid, root); }

	//返回按编号排序后的某类人员，若n_node_type不是人员类则抛异常
	std::list<std::shared_ptr<People>> sort_people_by_people_uid(node::NodeType n_node_type, bool ascending = true) const;

	//返回按月薪排序后的某教师类人员，若n_node_type不是教师类则抛异常
	std::list<std::shared_ptr<teacher>> sort_teacher_by_pay(node::NodeType n_node_type, bool ascending = true) const;

	//检测密码是否正确
	bool verify_password(const std::string& password) const;

	//修改密码
	void change_password(const std::string& n_password);

	//清除所有组织架构，仅保留根节点，并将current重置为根节点，须提供密码
	void clear_structure(const std::string& password);

	//清除所有人员，保留部门组织架构，并将current重置为根节点，须提供密码
	void clear_people(const std::string& password);

	//保存数据到n_filename文件夹，若缺省参数则保存到原文件夹
	void save(const std::string& n_filename = std::string()) const;

private:
	//与该数据库对应的文件夹名，在数据库创建之后不可修改
	const std::string filename;

	//储存各个文件的路径的vector
	//各分量的含义：0-node 1-department 2-people 3-student 4-graduate 5-teacher 6-prof 7-TA 8-password
	std::vector<std::string> path;

	//文件打开方式，在数据库创建之后不可修改
	const rtype opentype;

	//密码的哈希值
	uint8_t password_hash[32];

	//组织架构根节点，其名称与文件名相同
	std::shared_ptr<department> root;

	//当前浏览或操作的节点
	std::shared_ptr<node> current;

	//保存以subroot为根节点的子树到一系列文件writefile里面，该函数中有递归调用
	void save_by_subtree(const std::shared_ptr<node>& subroot, std::vector<std::ofstream>& writefile) const;

	//在某个子树中按姓名/身份证号/编号查找人员，返回所有查找结果
	std::list<std::shared_ptr<People>> search_people_by_name(const std::string& n_name, const std::shared_ptr<node>& subroot) const;
	std::shared_ptr<People> search_people_by_id_card(const std::string& n_id_card, const std::shared_ptr<node>& subroot) const;
	std::shared_ptr<People> search_people_by_people_uid(int n_people_uid, const std::shared_ptr<node>& subroot) const;

	//返回某个子树中所有某类人员按编号排序后的结果
	std::set<std::shared_ptr<People>, defaultNodeCmp> sort_people_by_people_uid(node::NodeType n_node_type, const std::shared_ptr<node>& subroot) const;
	std::set<std::shared_ptr<teacher>, teacherCmpByPay> sort_teacher_by_pay(node::NodeType n_node_type, const std::shared_ptr<node>& subroot) const;

	//清除子树所有人员，保留部门组织架构
	void clear_people(std::shared_ptr<node> subroot);
};