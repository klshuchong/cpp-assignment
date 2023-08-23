//Exception.h
//Created by Wang Jie (2022010554)
#pragma once
#include <string>
#include "base.h"

using std::string;

//�쳣�������
class MyException
{
protected:
	string message;
public:
	MyException(const string& str = "undefined problem") :message(str) {}
	virtual string info() const { return message; }
};

//�ļ���д�쳣��
class FileException :public MyException
{
public:
	FileException(const string& n_filename) :MyException("File error"), filename(n_filename) {}
	virtual string info() const { return message + ": cannot open file " + filename; }
protected:
	string filename;
};

//�����쳣�ࣺ����ڵ㲻Ψһ���ڵ�ĸ��ڵ㲻���ڡ��о����ĵ�ʦ�Ҳ�����
class DataException : public MyException
{
public:
	DataException(const string& n_detail) : MyException("Data error"), detail(n_detail) {}
	virtual string info() const { return message + ": " + detail; }
protected:
	string detail;
};

//�����쳣��
class DateException :public MyException
{
protected:
	//��¼�Ƿ�����
	int y;
	int m;
	int d;
public:
	DateException(int yy, int mm, int dd) :MyException("Date error"), y(yy), m(mm), d(dd) {}
	virtual string info() const
	{
		return message + ": " + std::to_string(y) + "/" + std::to_string(m) + "/" + std::to_string(d) + " is not a valid date";
	}
};

//���֤���쳣��
class IDException :public MyException
{
public:
	IDException(const string& n_id) :MyException("ID error"), bad_id(n_id) {}
	virtual string info() const { return message + ": \"" + bad_id + "\" is not a valid id"; }
protected:
	string bad_id;
};

//�ڵ������쳣�ࣺ������ͼ��node*ָ��ķ�graduate��������graduate��������еķ���
class NodeTypeException : public MyException
{
public:
	//���캯��
	NodeTypeException(node::NodeType n_node_type) :MyException("Node type error"), node_type(n_node_type) {}

	virtual string info() const
	{
		string str = message + ": carrying out operations specific to ";
		switch (node_type)
		{
		case node::NodeType::Department: str += "department"; break;
		case node::NodeType::People: str += "People"; break;
		case node::NodeType::Student: str += "student"; break;
		case node::NodeType::Graduate: str += "graduate"; break;
		case node::NodeType::Teacher: str += "teacher"; break;
		case node::NodeType::Prof: str += "prof"; break;
		case node::NodeType::Ta: str += "TA"; break;
		}
		str += " on its base type";
		return str;
	}
protected:
	node::NodeType node_type;
};

//��������ࣺ�����������
class PasswordException : public MyException
{
public:
	PasswordException(): MyException("Password error"){}
	virtual string info() const { return MyException::info(); }
};

//���������ࣺ������Ĳ��������в�������
class SameNameDepartmentException : public MyException
{
public:
	SameNameDepartmentException(const string& n_dept_name): MyException("Same name department error"), dept_name(n_dept_name) {}
	virtual string info() const { return message + ": name of department " + dept_name + " is duplicated"; }
protected:
	string dept_name;
};

//���֤���ظ��ࣺ���������Ա��������Ա�����֤����ͬ
class SameIDException :public MyException
{
public:
	SameIDException(const string& n_id_card) : MyException("Same ID error"), id_card(n_id_card) {}
	virtual string info() const { return message + ": ID " + id_card + " is duplicated"; }
protected:
	string id_card;
};

//���ӽڵ��ࣺ�������Ľڵ㲻��database::current���ӽڵ�
class NotAChildException : public MyException
{
public:
	NotAChildException(const string& n_node_name) : MyException("Not a child error"), node_name(n_node_name) {}
	virtual string info() const { return message + ": \"" + node_name + "\" is not a child of database::current"; }
protected:
	string node_name;
};