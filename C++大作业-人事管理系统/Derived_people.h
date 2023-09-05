//Derived_people.h
//Created by Wang Jie (2022010554)
//����student, graduate, teacher, prof, TA������
#pragma once
#include <iostream>
#include <set>
#include <memory>
#include "base.h"

//ѧ����
class student : virtual public People
{
public:
	//���캯��
	student(const std::string& n_name = default_name, const std::string& n_id = default_id) : People(n_name, n_id, node::NodeType::Student) {}

	//���캯�������ļ��ж�ȡ��
	student(std::ifstream& ifs) :People(ifs, node::NodeType::Student) {}

	//�����и��ƹ��캯���͸�ֵ����������ᵼ�����֤���ظ�
	student(const student&) = delete;
	const student& operator=(const student&) = delete;

	//��ӡ��Ϣ
	virtual void print(std::ostream& os = std::cout) const { People::print(os); }

	//д�뵽�ļ�
	virtual void fwrite(std::ofstream& ofs) const { People::fwrite(ofs); }

private:
	//ѧ��Ĭ������
	static const char default_name[NAME_MAX_LENGTH];
};

//prof��ǰ������
class prof;

//�о�����
class graduate : public student
{
public:
	//���캯��(��ʼ��ʦΪ��ָ�룬��ʦuidΪ-1)
	graduate(const std::string& n_name = default_name, const std::string& n_id = default_id) : People(n_name, n_id, node::NodeType::Graduate), advisor_uid(-1), advisor() {}

	//���캯�������ļ��ж�ȡ����ʼ��ʦΪ��ָ�룬��ʦuidΪ-1��
	graduate(std::ifstream& ifs);

	//�����и��ƹ��캯���͸�ֵ����������ᵼ�����֤���ظ�
	graduate(const graduate&) = delete;
	const graduate& operator=(const graduate&) = delete;

	//��ӡ��Ϣ
	virtual void print(std::ostream& os = std::cout) const;

	//д�뵽�ļ�
	virtual void fwrite(std::ofstream& ofs) const;

	//��������
	virtual ~graduate() { remove_advisor(); }

	//��ȡ��ʦuid
	int getadvisoruid() const { return advisor_uid; }

	//��ȡ��ʦ
	std::shared_ptr<prof> getadvisor() const { return advisor.lock(); }

	//���õ�ʦ���˺���Ϊprof����Ԫ������ԭ�е�ʦ���µ�ʦΪ��ָ���򷵻�false�����򷵻�true
	bool assign_advisor(const std::shared_ptr<prof>& n_advisor);

	//�����ʦ���˺���Ϊprof����Ԫ������ԭ�޵�ʦ�򷵻�false�����򷵻�true
	bool remove_advisor();

private:
	//�о���Ĭ������
	static const char default_name[NAME_MAX_LENGTH];

	//��ʦuid
	int advisor_uid;

	//��ʦ��Ϊ��ֹ��ʦ���о�����shared_ptr���óɻ���������weak_ptr��
	std::weak_ptr<prof> advisor;
};

//��ʦ��
class teacher : virtual public People
{
public:
	//���캯��
	teacher(const std::string& n_name = default_name, const std::string& n_id = default_id, unsigned int n_pay = 0) : People(n_name, n_id, node::NodeType::Teacher), pay(n_pay) {}

	//���캯�������ļ��ж�ȡ��
	teacher(std::ifstream& ifs);

	//�����и��ƹ��캯���͸�ֵ����������ᵼ�����֤���ظ�
	teacher(const teacher&) = delete;
	const teacher& operator=(const teacher&) = delete;

	//��ӡ��Ϣ
	virtual void print(std::ostream& os = std::cout) const;

	//д�뵽�ļ�
	virtual void fwrite(std::ofstream& ofs) const;

	//������н
	unsigned int getpay() const { return pay; }

	//������н
	void setpay(unsigned int n_pay) { pay = n_pay; }

private:
	//��ʦĬ������
	static const char default_name[NAME_MAX_LENGTH];

	//��н
	unsigned int pay;

};

//��ʦ����н�Ƚ���
class teacherCmpByPay
{
public:
	bool operator()(const std::shared_ptr<teacher>& left, const std::shared_ptr<teacher>& right) const
	{
		return left->getpay() < right->getpay();
	}
};

//������
class prof : public teacher
{
	//graduate���뵼ʦ�йصĳ�Ա����Ϊprof����Ԫ����
	friend bool graduate::assign_advisor(const std::shared_ptr<prof>& n_advisor);
	friend bool graduate::remove_advisor();

public:
	//���캯��
	prof(const std::string& n_name = default_name, const std::string& n_id = default_id, unsigned int n_pay = 0) : People(n_name, n_id, node::NodeType::Prof), teacher(n_name, n_id, n_pay) {}

	//���캯�������ļ��ж�ȡ��
	prof(std::ifstream& ifs) : People(ifs, node::NodeType::Prof), teacher(ifs) {}

	//�����и��ƹ��캯���͸�ֵ����������ᵼ�����֤���ظ�
	prof(const prof&) = delete;
	const prof& operator=(const prof&) = delete;

	//��ӡ��Ϣ�����������ε�ʦ���о�����Ϣ��
	virtual void print(std::ostream& os = std::cout) const { teacher::print(os); }

	//��ӡ���ε�ʦ���о�����Ϣ
	void print_stus(std::ostream& os = std::cout) const;

	//д�뵽�ļ�
	virtual void fwrite(std::ofstream& ofs) const { teacher::fwrite(ofs); }

	//��������
	virtual ~prof();

	//��ȡ�о�������
	unsigned int stu_num() const { return stus.size(); }

	//��ȡ���ε�ʦ���о���set
	std::set<std::shared_ptr<graduate>, defaultNodeCmp> getstus() const { return stus; }

private:
	//����Ĭ������
	static const char default_name[NAME_MAX_LENGTH];

	//���ε�ʦ���о���
	std::set<std::shared_ptr<graduate>, defaultNodeCmp> stus;
};

//������
class TA : public graduate, public teacher
{
public:
	//���캯��
	TA(const std::string& n_name = default_name, const std::string& n_id = default_id, unsigned int n_pay = 0) : People(n_name, n_id, node::NodeType::Ta), graduate(n_name, n_id), teacher(n_name, n_id, n_pay) {}

	//���캯�������ļ��ж�ȡ��
	TA(std::ifstream& ifs) : People(ifs, node::NodeType::Ta), graduate(ifs), teacher(ifs) {}

	//�����и��ƹ��캯���͸�ֵ����������ᵼ�����֤���ظ�
	TA(const TA&) = delete;
	const TA& operator=(const TA&) = delete;

	//��ӡ��Ϣ
	virtual void print(std::ostream& os = std::cout) const;

	//д�뵽�ļ�
	virtual void fwrite(std::ofstream& ofs) const;

private:
	//����Ĭ������
	static const char default_name[NAME_MAX_LENGTH];
};