//userInterface.h
//Created by Wang Jie (2022010554)
//userInterface������
#pragma once
#include <list>
#include <string>
#include <memory>
#include "database.h"

//�û�������
class userInterface
{
public:
	//���캯���������ݿ�ָ���ʼ��Ϊ��ָ��
	userInterface() : db() {}

	//��Ҫ�Զ��������к���
	userInterface(const userInterface&) = delete;
	const userInterface& operator=(const userInterface&) = delete;

	//�Զ�������������
	~userInterface() = default;

	//��������
	void start();

private:
	//ѡ����棺��ʾmsg�еĸ���ѡ�quit_msg��ʾ���˳���Ϣ
	//���صķǸ�����Ϊ�û�ѡ��ı�ţ�����-1��ʾ�û�ѡ���˳�
	//���紫�����Ϊ��msg = std::list("option 0", "option 1"), quit_msg = "quit"ʱ��������ʾ������Ϣ��
	//   0. option 0
	//   1. option 1
	//   q. quit
	//   ������ѡ��ǰ�ı��...
	//Ȼ���û�������ѡ��ǰ�ı������ѡ�������������Ч��ţ���Ҫ���û��������룬ֱ������Ϸ����Ϊֹ
	int choose(const std::list<std::string>& msg, const std::string& quit_msg = "�˳�");

	//�Ӽ����������룬������������룬�ú������������ݼ�*������֧���˸񣬷�ֹ����
	std::string input_password();

	//�����µ������ļ�
	void create();

	//�����������ļ�
	void open();

	//��ӭ����
	void welcome();

	//��������
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

	//����Ϊ�����潻����������������һ��Ҫת���Ľ�������
	//���˵�
	interface_type menu();

	//�鿴��֯�ܹ�
	interface_type view_structure();

	//�½�ֱ������
	interface_type add_new_dept();

	//����������
	interface_type rename_dept();

	//�ƶ�����
	interface_type move_dept();

	//ɾ������
	interface_type remove_dept();

	//�����֯�ܹ�
	interface_type clear_structure();

	//�½�ֱ����Ա
	interface_type add_people();

	//�鿴��Ա��Ϣ
	interface_type view_people();

	//������Ա
	interface_type search_people();

	//�鿴ĳ��������Ա
	interface_type view_all_people();

	//������Ա��Ϣ
	interface_type update_people();

	//�ƶ���Ա����������
	interface_type move_people();

	//ɾ����Ա
	interface_type remove_people();

	//���õ�ʦ���о�����ϵ
	interface_type set_advisor();

	//�����ʦ���о�����ϵ
	interface_type remove_advisor();

	//��ĳ����Ա����
	interface_type sort_people();

	//���������Ա
	interface_type clear_people();

	//����֯�ܹ����Ϊ���ļ�
	interface_type save_new();

	//�޸�����
	interface_type change_password();

	//�˳���ʾ
	void exit();

	//ָ��������溯���ĺ���ָ������
	static interface_type(userInterface::* const itf[20])();

	//���ݿ�ָ��
	std::shared_ptr<database> db;
};