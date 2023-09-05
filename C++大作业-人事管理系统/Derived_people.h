//Derived_people.h
//Created by Wang Jie (2022010554)
//包含student, graduate, teacher, prof, TA类声明
#pragma once
#include <iostream>
#include <set>
#include <memory>
#include "base.h"

//学生类
class student : virtual public People
{
public:
	//构造函数
	student(const std::string& n_name = default_name, const std::string& n_id = default_id) : People(n_name, n_id, node::NodeType::Student) {}

	//构造函数（从文件中读取）
	student(std::ifstream& ifs) :People(ifs, node::NodeType::Student) {}

	//不能有复制构造函数和赋值函数，否则会导致身份证号重复
	student(const student&) = delete;
	const student& operator=(const student&) = delete;

	//打印信息
	virtual void print(std::ostream& os = std::cout) const { People::print(os); }

	//写入到文件
	virtual void fwrite(std::ofstream& ofs) const { People::fwrite(ofs); }

private:
	//学生默认名称
	static const char default_name[NAME_MAX_LENGTH];
};

//prof类前向声明
class prof;

//研究生类
class graduate : public student
{
public:
	//构造函数(初始导师为空指针，导师uid为-1)
	graduate(const std::string& n_name = default_name, const std::string& n_id = default_id) : People(n_name, n_id, node::NodeType::Graduate), advisor_uid(-1), advisor() {}

	//构造函数（从文件中读取，初始导师为空指针，导师uid为-1）
	graduate(std::ifstream& ifs);

	//不能有复制构造函数和赋值函数，否则会导致身份证号重复
	graduate(const graduate&) = delete;
	const graduate& operator=(const graduate&) = delete;

	//打印信息
	virtual void print(std::ostream& os = std::cout) const;

	//写入到文件
	virtual void fwrite(std::ofstream& ofs) const;

	//析构函数
	virtual ~graduate() { remove_advisor(); }

	//获取导师uid
	int getadvisoruid() const { return advisor_uid; }

	//获取导师
	std::shared_ptr<prof> getadvisor() const { return advisor.lock(); }

	//设置导师（此函数为prof的友元），若原有导师或新导师为空指针则返回false，否则返回true
	bool assign_advisor(const std::shared_ptr<prof>& n_advisor);

	//解除导师（此函数为prof的友元），若原无导师则返回false，否则返回true
	bool remove_advisor();

private:
	//研究生默认名称
	static const char default_name[NAME_MAX_LENGTH];

	//导师uid
	int advisor_uid;

	//导师（为防止导师和研究生的shared_ptr引用成环，这里用weak_ptr）
	std::weak_ptr<prof> advisor;
};

//教师类
class teacher : virtual public People
{
public:
	//构造函数
	teacher(const std::string& n_name = default_name, const std::string& n_id = default_id, unsigned int n_pay = 0) : People(n_name, n_id, node::NodeType::Teacher), pay(n_pay) {}

	//构造函数（从文件中读取）
	teacher(std::ifstream& ifs);

	//不能有复制构造函数和赋值函数，否则会导致身份证号重复
	teacher(const teacher&) = delete;
	const teacher& operator=(const teacher&) = delete;

	//打印信息
	virtual void print(std::ostream& os = std::cout) const;

	//写入到文件
	virtual void fwrite(std::ofstream& ofs) const;

	//返回月薪
	unsigned int getpay() const { return pay; }

	//设置月薪
	void setpay(unsigned int n_pay) { pay = n_pay; }

private:
	//教师默认名称
	static const char default_name[NAME_MAX_LENGTH];

	//月薪
	unsigned int pay;

};

//教师按月薪比较类
class teacherCmpByPay
{
public:
	bool operator()(const std::shared_ptr<teacher>& left, const std::shared_ptr<teacher>& right) const
	{
		return left->getpay() < right->getpay();
	}
};

//教授类
class prof : public teacher
{
	//graduate的与导师有关的成员函数为prof的友元函数
	friend bool graduate::assign_advisor(const std::shared_ptr<prof>& n_advisor);
	friend bool graduate::remove_advisor();

public:
	//构造函数
	prof(const std::string& n_name = default_name, const std::string& n_id = default_id, unsigned int n_pay = 0) : People(n_name, n_id, node::NodeType::Prof), teacher(n_name, n_id, n_pay) {}

	//构造函数（从文件中读取）
	prof(std::ifstream& ifs) : People(ifs, node::NodeType::Prof), teacher(ifs) {}

	//不能有复制构造函数和赋值函数，否则会导致身份证号重复
	prof(const prof&) = delete;
	const prof& operator=(const prof&) = delete;

	//打印信息（不包括担任导师的研究生信息）
	virtual void print(std::ostream& os = std::cout) const { teacher::print(os); }

	//打印担任导师的研究生信息
	void print_stus(std::ostream& os = std::cout) const;

	//写入到文件
	virtual void fwrite(std::ofstream& ofs) const { teacher::fwrite(ofs); }

	//析构函数
	virtual ~prof();

	//获取研究生数量
	unsigned int stu_num() const { return stus.size(); }

	//获取担任导师的研究生set
	std::set<std::shared_ptr<graduate>, defaultNodeCmp> getstus() const { return stus; }

private:
	//教授默认名称
	static const char default_name[NAME_MAX_LENGTH];

	//担任导师的研究生
	std::set<std::shared_ptr<graduate>, defaultNodeCmp> stus;
};

//助教类
class TA : public graduate, public teacher
{
public:
	//构造函数
	TA(const std::string& n_name = default_name, const std::string& n_id = default_id, unsigned int n_pay = 0) : People(n_name, n_id, node::NodeType::Ta), graduate(n_name, n_id), teacher(n_name, n_id, n_pay) {}

	//构造函数（从文件中读取）
	TA(std::ifstream& ifs) : People(ifs, node::NodeType::Ta), graduate(ifs), teacher(ifs) {}

	//不能有复制构造函数和赋值函数，否则会导致身份证号重复
	TA(const TA&) = delete;
	const TA& operator=(const TA&) = delete;

	//打印信息
	virtual void print(std::ostream& os = std::cout) const;

	//写入到文件
	virtual void fwrite(std::ofstream& ofs) const;

private:
	//助教默认名称
	static const char default_name[NAME_MAX_LENGTH];
};