//userInterface.h
//Created by Wang Jie (2022010554)
//userInterface类声明
#pragma once
#include <list>
#include <string>
#include <memory>
#include "database.h"

//用户界面类
class userInterface
{
public:
	//构造函数，将数据库指针初始化为空指针
	userInterface() : db() {}

	//不要自动生成下列函数
	userInterface(const userInterface&) = delete;
	const userInterface& operator=(const userInterface&) = delete;

	//自动生成析构函数
	~userInterface() = default;

	//启动界面
	void start();

private:
	//选择界面：显示msg中的各个选项及quit_msg表示的退出消息
	//返回的非负整数为用户选择的编号，返回-1表示用户选择退出
	//例如传入参数为：msg = std::list("option 0", "option 1"), quit_msg = "quit"时，函数显示以下信息：
	//   0. option 0
	//   1. option 1
	//   q. quit
	//   请输入选项前的编号...
	//然后用户可输入选项前的编号做出选择，若输入的是无效编号，则要求用户重新输入，直到输入合法编号为止
	int choose(const std::list<std::string>& msg, const std::string& quit_msg = "退出");

	//从键盘输入密码，返回输入的密码，该函数对输入内容加*保护且支持退格，防止窥屏
	std::string input_password();

	//创建新的数据文件
	void create();

	//打开已有数据文件
	void open();

	//欢迎界面
	void welcome();

	//界面类型
	enum interface_type {
		menu_t = 0,
		view_structure_t = 1,
		add_new_dept_t = 2,
		rename_dept_t = 3,
		move_dept_t = 4,
		remove_dept_t = 5,
		clear_structure_t = 6,
		add_people_t = 7,
		view_people_t = 8,
		search_people_t = 9,
		view_all_people_t = 10,
		update_people_t = 11,
		move_people_t = 12,
		remove_people_t = 13,
		set_advisor_t = 14,
		remove_advisor_t = 15,
		sort_people_t = 16,
		clear_people_t = 17,
		save_new_t = 18,
		change_password_t = 19,
		exit_t = 20
	};

	//以下为各界面交互函数，均返回下一步要转到的界面类型
	//主菜单
	interface_type menu();

	//查看组织架构
	interface_type view_structure();

	//新建直属部门
	interface_type add_new_dept();

	//部门重命名
	interface_type rename_dept();

	//移动部门
	interface_type move_dept();

	//删除部门
	interface_type remove_dept();

	//清除组织架构
	interface_type clear_structure();

	//新建直属人员
	interface_type add_people();

	//查看人员信息
	interface_type view_people();

	//查找人员
	interface_type search_people();

	//查看某类所有人员
	interface_type view_all_people();

	//更新人员信息
	interface_type update_people();

	//移动人员到其他部门
	interface_type move_people();

	//删除人员
	interface_type remove_people();

	//设置导师和研究生关系
	interface_type set_advisor();

	//解除导师和研究生关系
	interface_type remove_advisor();

	//对某类人员排序
	interface_type sort_people();

	//清除所有人员
	interface_type clear_people();

	//将组织架构另存为新文件
	interface_type save_new();

	//修改密码
	interface_type change_password();

	//退出提示
	void exit();

	//指向各个界面函数的函数指针数组
	static interface_type(userInterface::* const itf[20])();

	//数据库指针
	std::shared_ptr<database> db;
};