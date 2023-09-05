//base.h
//Created by Wang Jie (2022010554)
//包含node, department, People类声明
#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <set>
#include <memory>
#include "Date.h"

//department类前向声明
class department;

class defaultNodeCmp;

//部门和人员的共同抽象基类：节点
class node : public std::enable_shared_from_this<node>
{
public:
	//表示节点类型的枚举
	enum NodeType { None = 0, Department, People, Student, Graduate, Teacher, Prof, Ta };

	//构造函数
	node(const std::string& n_name = default_name, NodeType n_node_type = NodeType::None);

	//构造函数（从文件中读取）
	node(std::ifstream& ifs, NodeType n_node_type = NodeType::None);

	//不隐式生成复制构造函数和赋值函数（否则会造成树中成环）
	node(const node&) = delete;
	const node& operator=(const node&) = delete;

	//析构函数（需要更新一下父亲节点）
	virtual ~node() { setfather(nullptr); }

	//获得姓名
	std::string getname() const { return std::string(name); }

	//获得节点类型
	NodeType getnodetype() const { return node_type; }

	//获得节点uid
	int getuid() const { return uid; }

	//获取max_uid
	static unsigned int getmaxuid() { return max_uid; }

	//获得父节点uid
	int getfatheruid() const { return father_uid; }

	//获得父节点
	std::shared_ptr<department> getfather() const { return father.lock(); }

	//设置姓名
	void setname(const std::string& n_name) { strcpy_s(name, n_name.c_str()); }

	//从文件中读取最大uid值
	static void setmaxuid(std::ifstream& ifs) { ifs.read((char*)&max_uid, sizeof(unsigned int)); }

	//设置父节点（为department的友元函数），若设置成功则返回true，若不成功（一般是因为目标父节点下有重名部门）则不做任何操作并返回false
	bool setfather(const std::shared_ptr<department>& n_father);

	//返回孩子节点个数
	virtual unsigned int child_num() const = 0;

	//返回所有孩子节点的set
	virtual std::set<std::shared_ptr<node>, defaultNodeCmp> get_child() const = 0;

	//打印信息
	virtual void print(std::ostream& os = std::cout) const = 0;

	//写入到文件
	virtual void fwrite(std::ofstream& ofs) const;

protected:
	//节点名称最大长度
	static const unsigned int NAME_MAX_LENGTH = 64;
	
	//节点名称
	char name[NAME_MAX_LENGTH];

	//节点类型
	const NodeType node_type;

	//每个节点独一无二的编号
	int uid;

	//整个系统中存在过的最大的编号值+1
	static unsigned int max_uid;

	//父节点uid
	int father_uid;

	//父节点，为防止父子节点shared_ptr引用成环这里要换成weak_ptr
	std::weak_ptr<department> father;
private:
	//节点默认名称
	static const char default_name[NAME_MAX_LENGTH];
};

//<<运算符的重载
inline std::ostream& operator<<(std::ostream& os, const node& N)
{
	N.print(os);
	return os;
}

//节点默认比较类
//比较规则：部门小于人员，若都为部门则按名称字典序排序，若都为人员则按人员编号排序
class defaultNodeCmp
{
public:
	bool operator()(const std::shared_ptr<node>& left, const std::shared_ptr<node>& right) const;
};

//部门类
class department : public node
{
	//node类设置父节点的函数为department类的友元函数
	friend bool node::setfather(const std::shared_ptr<department>& n_father);

public:
	//构造函数
	department(const std::string& n_name = default_name) :node(n_name, node::NodeType::Department) {}

	//构造函数（从文件中读取）
	department(std::ifstream& ifs) :node(ifs, node::NodeType::Department) {}

	//不隐式生成复制构造函数和赋值函数
	department(const department&) = delete;
	const department& operator=(const department&) = delete;

	//析构函数（会清理所有下属部门和下属人员）
	//由于使用了智能指针，所以不需要任何手动清理工作，直接释放
	virtual ~department() {}

	//返回孩子节点个数
	virtual unsigned int child_num() const { return child.size(); }

	//返回所有孩子节点
	virtual std::set<std::shared_ptr<node>, defaultNodeCmp> get_child() const { return child; }

	//打印信息
	virtual void print(std::ostream& os = std::cout) const;

	//写入到文件
	virtual void fwrite(std::ofstream& ofs) const { node::fwrite(ofs); }

private:
	//部门默认名称
	static const char default_name[NAME_MAX_LENGTH];

	//储存孩子节点的set（用set是为了对节点自动排序，但无法对两个node*重载<运算符，STL容器中又不能存node的引用，所以只好用智能指针）
	std::set<std::shared_ptr<node>, defaultNodeCmp> child;
};

//人员类
class People : public node
{
public:
	//表示性别的枚举
	enum Gender { None = 0, Male, Female };

	//构造函数
	People(const std::string& n_name = default_name, const std::string& n_id = default_id, NodeType n_node_type = NodeType::People);

	//构造函数（从文件中读取）
	People(std::ifstream& ifs, NodeType n_node_type = NodeType::People);

	//不能有复制构造函数和赋值函数，否则会导致身份证号重复
	People(const People&) = delete;
	const People& operator=(const People&) = delete;

	//析构函数，需要把自己的身份证号从id_set中移除
	virtual ~People() { id_set.erase(std::string(id_card)); }

	//返回孩子节点个数（0）
	virtual unsigned int child_num() const { return 0; }

	//返回所有孩子节点（vector为空）
	virtual std::set<std::shared_ptr<node>, defaultNodeCmp> get_child() const { return std::set<std::shared_ptr<node>, defaultNodeCmp>(); }

	//打印信息
	virtual void print(std::ostream& os = std::cout) const;

	//写入到文件
	virtual void fwrite(std::ofstream& ofs) const;

	//返回身份证号
	std::string getidcard() const { return std::string(id_card); }

	//返回性别
	Gender getgender() const { return gender; }

	//返回出生日期
	Date getbirthday() const { return birthday; }

	//返回人员编号
	int getpeopleuid() const { return people_uid; }

	//返回max_people_uid
	static unsigned int getmaxpeopleuid() { return max_people_uid; }

	//设置身份证号（同时设定性别和出生日期）
	void setidcard(const std::string& n_id);

	//从文件读取人员最大编号值
	static void setmaxpeopleuid(std::ifstream& ifs) { ifs.read((char*)&max_people_uid, sizeof(unsigned int)); }
protected:
	//默认身份证号
	static const char default_id[19];

private:
	//默认姓名
	static const char default_name[NAME_MAX_LENGTH];

	//身份证号
	char id_card[19];

	//计算身份证号校验位时各位的权重
	static const int id_weight[17];

	//检查一个字符串是否为合法的身份证号
	static bool is_valid_id(const std::string& tested);

	//所有人员的身份证号合集，用于检验新人员的身份证号是否已存在
	static std::set<std::string> id_set;

	//性别
	Gender gender;

	//出生日期
	Date birthday;

	//每个人员独一无二的编号
	int people_uid;

	//出现过的人员编号的最大值+1
	static unsigned int max_people_uid;
};