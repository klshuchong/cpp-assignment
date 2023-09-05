//base.h
//Created by Wang Jie (2022010554)
//����node, department, People������
#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <set>
#include <memory>
#include "Date.h"

//department��ǰ������
class department;

class defaultNodeCmp;

//���ź���Ա�Ĺ�ͬ������ࣺ�ڵ�
class node : public std::enable_shared_from_this<node>
{
public:
	//��ʾ�ڵ����͵�ö��
	enum NodeType { None = 0, Department, People, Student, Graduate, Teacher, Prof, Ta };

	//���캯��
	node(const std::string& n_name = default_name, NodeType n_node_type = NodeType::None);

	//���캯�������ļ��ж�ȡ��
	node(std::ifstream& ifs, NodeType n_node_type = NodeType::None);

	//����ʽ���ɸ��ƹ��캯���͸�ֵ�����������������гɻ���
	node(const node&) = delete;
	const node& operator=(const node&) = delete;

	//������������Ҫ����һ�¸��׽ڵ㣩
	virtual ~node() { setfather(nullptr); }

	//�������
	std::string getname() const { return std::string(name); }

	//��ýڵ�����
	NodeType getnodetype() const { return node_type; }

	//��ýڵ�uid
	int getuid() const { return uid; }

	//��ȡmax_uid
	static unsigned int getmaxuid() { return max_uid; }

	//��ø��ڵ�uid
	int getfatheruid() const { return father_uid; }

	//��ø��ڵ�
	std::shared_ptr<department> getfather() const { return father.lock(); }

	//��������
	void setname(const std::string& n_name) { strcpy_s(name, n_name.c_str()); }

	//���ļ��ж�ȡ���uidֵ
	static void setmaxuid(std::ifstream& ifs) { ifs.read((char*)&max_uid, sizeof(unsigned int)); }

	//���ø��ڵ㣨Ϊdepartment����Ԫ�������������óɹ��򷵻�true�������ɹ���һ������ΪĿ�길�ڵ������������ţ������κβ���������false
	bool setfather(const std::shared_ptr<department>& n_father);

	//���غ��ӽڵ����
	virtual unsigned int child_num() const = 0;

	//�������к��ӽڵ��set
	virtual std::set<std::shared_ptr<node>, defaultNodeCmp> get_child() const = 0;

	//��ӡ��Ϣ
	virtual void print(std::ostream& os = std::cout) const = 0;

	//д�뵽�ļ�
	virtual void fwrite(std::ofstream& ofs) const;

protected:
	//�ڵ�������󳤶�
	static const unsigned int NAME_MAX_LENGTH = 64;
	
	//�ڵ�����
	char name[NAME_MAX_LENGTH];

	//�ڵ�����
	const NodeType node_type;

	//ÿ���ڵ��һ�޶��ı��
	int uid;

	//����ϵͳ�д��ڹ������ı��ֵ+1
	static unsigned int max_uid;

	//���ڵ�uid
	int father_uid;

	//���ڵ㣬Ϊ��ֹ���ӽڵ�shared_ptr���óɻ�����Ҫ����weak_ptr
	std::weak_ptr<department> father;
private:
	//�ڵ�Ĭ������
	static const char default_name[NAME_MAX_LENGTH];
};

//<<�����������
inline std::ostream& operator<<(std::ostream& os, const node& N)
{
	N.print(os);
	return os;
}

//�ڵ�Ĭ�ϱȽ���
//�ȽϹ��򣺲���С����Ա������Ϊ�����������ֵ�����������Ϊ��Ա����Ա�������
class defaultNodeCmp
{
public:
	bool operator()(const std::shared_ptr<node>& left, const std::shared_ptr<node>& right) const;
};

//������
class department : public node
{
	//node�����ø��ڵ�ĺ���Ϊdepartment�����Ԫ����
	friend bool node::setfather(const std::shared_ptr<department>& n_father);

public:
	//���캯��
	department(const std::string& n_name = default_name) :node(n_name, node::NodeType::Department) {}

	//���캯�������ļ��ж�ȡ��
	department(std::ifstream& ifs) :node(ifs, node::NodeType::Department) {}

	//����ʽ���ɸ��ƹ��캯���͸�ֵ����
	department(const department&) = delete;
	const department& operator=(const department&) = delete;

	//���������������������������ź�������Ա��
	//����ʹ��������ָ�룬���Բ���Ҫ�κ��ֶ���������ֱ���ͷ�
	virtual ~department() {}

	//���غ��ӽڵ����
	virtual unsigned int child_num() const { return child.size(); }

	//�������к��ӽڵ�
	virtual std::set<std::shared_ptr<node>, defaultNodeCmp> get_child() const { return child; }

	//��ӡ��Ϣ
	virtual void print(std::ostream& os = std::cout) const;

	//д�뵽�ļ�
	virtual void fwrite(std::ofstream& ofs) const { node::fwrite(ofs); }

private:
	//����Ĭ������
	static const char default_name[NAME_MAX_LENGTH];

	//���溢�ӽڵ��set����set��Ϊ�˶Խڵ��Զ����򣬵��޷�������node*����<�������STL�������ֲ��ܴ�node�����ã�����ֻ��������ָ�룩
	std::set<std::shared_ptr<node>, defaultNodeCmp> child;
};

//��Ա��
class People : public node
{
public:
	//��ʾ�Ա��ö��
	enum Gender { None = 0, Male, Female };

	//���캯��
	People(const std::string& n_name = default_name, const std::string& n_id = default_id, NodeType n_node_type = NodeType::People);

	//���캯�������ļ��ж�ȡ��
	People(std::ifstream& ifs, NodeType n_node_type = NodeType::People);

	//�����и��ƹ��캯���͸�ֵ����������ᵼ�����֤���ظ�
	People(const People&) = delete;
	const People& operator=(const People&) = delete;

	//������������Ҫ���Լ������֤�Ŵ�id_set���Ƴ�
	virtual ~People() { id_set.erase(std::string(id_card)); }

	//���غ��ӽڵ������0��
	virtual unsigned int child_num() const { return 0; }

	//�������к��ӽڵ㣨vectorΪ�գ�
	virtual std::set<std::shared_ptr<node>, defaultNodeCmp> get_child() const { return std::set<std::shared_ptr<node>, defaultNodeCmp>(); }

	//��ӡ��Ϣ
	virtual void print(std::ostream& os = std::cout) const;

	//д�뵽�ļ�
	virtual void fwrite(std::ofstream& ofs) const;

	//�������֤��
	std::string getidcard() const { return std::string(id_card); }

	//�����Ա�
	Gender getgender() const { return gender; }

	//���س�������
	Date getbirthday() const { return birthday; }

	//������Ա���
	int getpeopleuid() const { return people_uid; }

	//����max_people_uid
	static unsigned int getmaxpeopleuid() { return max_people_uid; }

	//�������֤�ţ�ͬʱ�趨�Ա�ͳ������ڣ�
	void setidcard(const std::string& n_id);

	//���ļ���ȡ��Ա�����ֵ
	static void setmaxpeopleuid(std::ifstream& ifs) { ifs.read((char*)&max_people_uid, sizeof(unsigned int)); }
protected:
	//Ĭ�����֤��
	static const char default_id[19];

private:
	//Ĭ������
	static const char default_name[NAME_MAX_LENGTH];

	//���֤��
	char id_card[19];

	//�������֤��У��λʱ��λ��Ȩ��
	static const int id_weight[17];

	//���һ���ַ����Ƿ�Ϊ�Ϸ������֤��
	static bool is_valid_id(const std::string& tested);

	//������Ա�����֤�źϼ������ڼ�������Ա�����֤���Ƿ��Ѵ���
	static std::set<std::string> id_set;

	//�Ա�
	Gender gender;

	//��������
	Date birthday;

	//ÿ����Ա��һ�޶��ı��
	int people_uid;

	//���ֹ�����Ա��ŵ����ֵ+1
	static unsigned int max_people_uid;
};