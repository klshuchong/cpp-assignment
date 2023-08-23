//database.h
//Created by Wang Jie (2022010554)
//database������
#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <memory>
#include "base.h"
#include "Derived_people.h"

// ע�������ļ���֯��ʽ
// ���������ļ�������һ���ļ����У����ļ�������Ϊ[filename]
// ������������ļ�����Ϊ[filename]�����Ӧ����չ����.department, .student, .graduate, .teacher, .prof, .ta
// ��ÿ���ļ��У���Ӧ������������Դ洢��
// �������⣬.node�ļ��д洢node::max_uid
// .people�ļ��д洢People::max_people_uid
// .password�ļ��д洢��SHA256���ܺ������

//���ݿ��࣬���д洢������˾����֯�ܹ������ṹ��
class database
{
public:
	//��ʾ�ļ������͵�ö�٣�Create��ʾ�������ļ���Read��ʾ�������ļ�
	enum rtype { Create = 0, Read };

	//���캯�������½��ļ������������ļ������������ļ����������룬������������׳�PasswordException�쳣����������ȷ�������������
	database(const std::string& n_filename, const std::string& password, rtype n_opentype = rtype::Create);

	//�������в���Ҫ�õ����ƹ��캯���͸�ֵ��������˲���ʽ��������
	database(const database&) = delete;
	const database& operator=(const database&) = delete;

	//��������
	~database() = default;

	//��ȡ��ǰ�ڵ�
	std::shared_ptr<node> getcurrent() const { return current; }

	//��current�Ƶ��丸�ڵ㣬��currentΪ���ڵ��򷵻�false�������ƶ�current����true
	bool to_father()
	{
		if (!current->getfather())return false;
		current = current->getfather();
	}

	//��current�Ƶ����ӽڵ�n_child����currentָ��Ľڵ㲻�����ӽڵ�n_child��currentָ�����People�򷵻�false�������ƶ�current������true
	bool to_child(const std::shared_ptr<node>& n_child)
	{
		auto current_child = current->get_child();
		if (current_child.find(n_child) != current_child.end())//������ҵ�n_child
		{
			current = n_child;
			return true;
		}
		else return false;
	}

	//��currentλ�����½�����n_dept���������������֮һ�����쳣��
	//1.currentָ����Ա
	//2.n_dept��currentλ�������в�������
	//��n_deptΪ�գ������κ�����
	//�ú���Ҳ�������ƶ�����
	void add_new_dept(std::shared_ptr<department>& n_dept);

	//��currentλ���µ�n_dept����������Ϊn_name���������������֮һ�����쳣:
	//1.n_dept����current���ӽڵ�
	//2.n_name��current�������ӽڵ�����
	//��n_deptΪ�գ������κ�����
	void rename_dept(std::shared_ptr<department>& n_dept, const std::string& n_name);

	//ɾ��currentλ���µ�n_dept���ż��������������ź�������Ա���������������֮һ�����쳣:
	//1.n_dept����current���ӽڵ�
	//��n_deptΪ�գ������κ�����
	void delete_dept(std::shared_ptr<department>& n_dept);

	//��currentλ�����½���Աn_people���������������֮һ�����쳣��
	//1.currentָ����Ա
	//2.n_people��������Ա�����֤����ͬ
	//��n_peopleΪ�գ������κ�����
	//�ú���Ҳ�������ƶ���Ա
	void add_new_people(std::shared_ptr<People>& n_people);

	//����currentָ�����Ա��Ϣ���������������֮һ�����쳣��
	//1.currentָ����
	//2.�µ����֤����������Ա�����֤����ͬ
	void update_people_info(const std::string& n_name, const std::string& n_id_card, unsigned int n_pay = 0);

	//ɾ��currentָ�����Ա����currentָ���������쳣
	//���д˲�����current��Ϊԭ����ָ��ڵ�ĸ��ڵ�
	void delete_people();

	//����currentָ����о����ĵ�ʦΪn_advisor���������������֮һ�����쳣��
	//1.currentָ��Ĳ����о���������
	//��n_advisorΪ�ջ�ԭ�е�ʦ�������κ�����
	void assign_advisor(std::shared_ptr<prof> n_advisor);

	//����currentָ����о����ĵ�ʦΪn_advisor���������������֮һ�����쳣��
	//1.currentָ��Ĳ����о���������
	//��currentԭ�޵�ʦ�������κ�����
	void remove_advisor();

	//������/���֤��/��Ų�����Ա���������в��ҽ��
	std::list<std::shared_ptr<People>> search_people_by_name(const std::string& n_name) const { return search_people_by_name(n_name, root); }
	std::shared_ptr<People> search_people_by_id_card(const std::string& n_id_card) const { return search_people_by_id_card(n_id_card, root); }
	std::shared_ptr<People> search_people_by_people_uid(int n_people_uid) const { return search_people_by_people_uid(n_people_uid, root); }

	//���ذ����������ĳ����Ա����n_node_type������Ա�������쳣
	std::list<std::shared_ptr<People>> sort_people_by_people_uid(node::NodeType n_node_type, bool ascending = true) const;

	//���ذ���н������ĳ��ʦ����Ա����n_node_type���ǽ�ʦ�������쳣
	std::list<std::shared_ptr<teacher>> sort_teacher_by_pay(node::NodeType n_node_type, bool ascending = true) const;

	//��������Ƿ���ȷ
	bool verify_password(const std::string& password) const;

	//�޸�����
	void change_password(const std::string& n_password);

	//���������֯�ܹ������������ڵ㣬����current����Ϊ���ڵ㣬���ṩ����
	void clear_structure(const std::string& password);

	//���������Ա������������֯�ܹ�������current����Ϊ���ڵ㣬���ṩ����
	void clear_people(const std::string& password);

	//�������ݵ�n_filename�ļ��У���ȱʡ�����򱣴浽ԭ�ļ���
	void save(const std::string& n_filename = std::string()) const;

private:
	//������ݿ��Ӧ���ļ������������ݿⴴ��֮�󲻿��޸�
	const std::string filename;

	//��������ļ���·����vector
	//�������ĺ��壺0-node 1-department 2-people 3-student 4-graduate 5-teacher 6-prof 7-TA 8-password
	std::vector<std::string> path;

	//�ļ��򿪷�ʽ�������ݿⴴ��֮�󲻿��޸�
	const rtype opentype;

	//����Ĺ�ϣֵ
	uint8_t password_hash[32];

	//��֯�ܹ����ڵ㣬���������ļ�����ͬ
	std::shared_ptr<department> root;

	//��ǰ���������Ľڵ�
	std::shared_ptr<node> current;

	//������subrootΪ���ڵ��������һϵ���ļ�writefile���棬�ú������еݹ����
	void save_by_subtree(const std::shared_ptr<node>& subroot, std::vector<std::ofstream>& writefile) const;

	//��ĳ�������а�����/���֤��/��Ų�����Ա���������в��ҽ��
	std::list<std::shared_ptr<People>> search_people_by_name(const std::string& n_name, const std::shared_ptr<node>& subroot) const;
	std::shared_ptr<People> search_people_by_id_card(const std::string& n_id_card, const std::shared_ptr<node>& subroot) const;
	std::shared_ptr<People> search_people_by_people_uid(int n_people_uid, const std::shared_ptr<node>& subroot) const;

	//����ĳ������������ĳ����Ա����������Ľ��
	std::set<std::shared_ptr<People>, defaultNodeCmp> sort_people_by_people_uid(node::NodeType n_node_type, const std::shared_ptr<node>& subroot) const;
	std::set<std::shared_ptr<teacher>, teacherCmpByPay> sort_teacher_by_pay(node::NodeType n_node_type, const std::shared_ptr<node>& subroot) const;

	//�������������Ա������������֯�ܹ�
	void clear_people(std::shared_ptr<node> subroot);
};