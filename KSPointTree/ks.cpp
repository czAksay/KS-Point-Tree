// ks.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>
using namespace std;


void Output(int value);
void Output(string text);

enum nodeType { type1 = 1, type2, type3 };

string double_to_str(double d)
{
	ostringstream streamObj;
	streamObj << d;
	return streamObj.str();
}

struct Point
{
	double x;
	double y;
	Point()
	{
		x = 0;
		y = 0;
	}

	Point(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
};

//абстрактный класс узла
class Node
{
protected:
	nodeType type;
protected:
public:
	int key;
	Node * left;
	Node *right;
	Node(int _key) : key(_key) {}
	virtual string GetFields() = 0;

	string getType()
	{
		return to_string(type);
	}
};

//типы узлов
class NodeT1 : public Node
{
private:
	string s1;
	string s2;
	int i1;
public:
	NodeT1(int key, string s1, string s2, int i1) : Node(key)
	{
		this->s1 = s1;
		this->s2 = s2;
		this->i1 = i1;
		type = type1;
	}

	string GetFields() override
	{
		return s1 + " " + s2 + " " + to_string(i1);
	}
};

class NodeT3 : public Node
{
private:
	string s1;
	string s2;
	int i1;
	int i2;
	int i3;
	double d1;
public:
	NodeT3(int key, string s1, string s2, int i1, int i2, int i3, double d1) : Node(key)
	{
		this->s1 = s1;
		this->s2 = s2;
		this->i1 = i1;
		this->i2 = i2;
		this->i3 = i3;
		this->d1 = d1;
		type = type3;
	}

	string GetFields() override
	{
		return s1 + " " + s2 + " " + to_string(i1) + " " + to_string(i2) + " " + to_string(i3) + " " + double_to_str(d1);
	}
};

class NodeT2 : public Node
{
private:
	string s1;
	string s2;
	int i1;
	int n;
	Point *p;
public:
	template<typename... T>
	NodeT2(int key, string s1, string s2, int i1, int n, T ... args) : Node(key)
	{
		this->s1 = s1;
		this->s2 = s2;
		this->i1 = i1;
		this->n = n;
		if (n <= 0)
		{
			throw invalid_argument("n shouldn't be negative!");
		}
		this->p = new Point[n];
		int i = 0;
		for (auto&& pt : std::initializer_list<Point>{ args... })
		{
			this->p[i] = pt;
			i++;
		}
		type = type2;
	}

	string GetFields() override
	{
		string points = "";
		for (int i = 0; i < n; i++)
		{
			points += "(" + double_to_str(p[i].x) + "," + double_to_str(p[i].y) + ") ";
		}
		return s1 + " " + s2 + " " + to_string(i1) + " "+ points;
	}
};

class Tree
{
private:
	Node *root;

	//кол-во узлов
	int nodeCount(Node *target)
	{
		if (target == NULL)
			return 0;
		return nodeCount(target->left) + nodeCount(target->right) + 1;
	}

	//кол-во терминальных узлов
	int nodeCountTermin(Node *target)
	{
		if (target == NULL)
			return 0;
		return nodeCount(target->left) + nodeCount(target->right) + (target->left == NULL && target->right == NULL)?1:0;
	}

	void Add(Node *_node, Node **target)
	{
		if (*target == NULL)
		{
			*target = _node;
		}
		else
		{
			if ((*target)->key == _node->key)
			{
				Node *l = (*target)->left;
				Node *r = (*target)->right;
				*target = _node;
				(*target)->left = l;
				(*target)->right = r;
			}
			else if (_node->key > (*target)->key)
			{
				Add(_node, &(*target)->right);
			}
			else
			{
				Add(_node, &(*target)->left);
			}
		}
	}

	//самый левый узел
	Node *getLeft(Node *target)
	{
		if (target == NULL)
			return target;
		if (target->left == NULL)
			return target;
		else
			return getLeft(target->left);
	}

	//самый правый узел
	Node *getRight(Node *target)
	{
		if (target == NULL)
			return target;
		if (target->right == NULL)
			return target;
		else
			return getRight(target->right);
	}

	void Remove(int key, Node **target)
	{
		if ((*target) == NULL)
			return;
		if ((*target)->key == key)
		{
			Node *del = *target;
			if (nodeCount((*target)->right) >= nodeCount((*target)->left))	//на место удаляемого узла будем крепить
																			//узел, имеющий большее кол-во потомков
			{
				Node * tempLeft = (*target)->left;	//сохраняем все узлы слева
				*target = (*target)->right;			//заменяем удаленный узел его дочерним узлом справа
				Node *l = getLeft(*target);			//получаем самый левый узел этого самого дочернего узла
				if (l != NULL)						//к нему слева будем крепить раннее сохраненные узлы
					l->left = tempLeft;
			}
			else
			{
				Node * tempRight = (*target)->right;
				*target = (*target)->left;
				Node *r = getRight(*target);
				if (r != NULL)
					r->right = tempRight;
			}
			delete del;
		}
		else if (key > (*target)->key)
		{
			Remove(key, &(*target)->right);
		}
		else
		{
			Remove(key, &(*target)->left);
		}
	}

	void Print(int num, Node *target)
	{
		if (target != NULL)
		{
			Output("#" + to_string(num) + "\t" + target->getType() + "\t" + to_string(target->key) + "\t" + target->GetFields());
		}
		else
			return;
		if (target->left != NULL)
		{
			Print(num + 1, target->left);
		}
		if (target->right != NULL)
		{
			Print(num + 1, target->right);
		}
	}

public:

	void Remove(int key)
	{
		Remove(key, &root);
	}

	int Count()
	{
		return nodeCount(root);
	}

	void Add(Node *_node)
	{
		Add(_node, &root);
		Output("--------\tAdded with key "+ to_string(_node->key)+"\t--------");
	}

	//добавить узел типа 1
	void Add(int key, string s1, string s2, int i1)
	{
		Add(new NodeT1(key, s1, s2, i1));
	}

	//добавить узел типа 2
	template<typename... T>
	void Add(int key, string s1, string s2, int i1, int n, T ... args)
	{
		Add(new NodeT2(key, s1, s2, i1, n, args ...));
	}

	//добавить узел типа 3
	void Add(int key, string s1, string s2, int i1, int i2, int i3, double d1)
	{
		Add(new NodeT3(key, s1, s2, i1, i2, i3, d1));
	}

	void Print()
	{
		Output("Level\tType\tKey\tFields");
		int i = 1;
		Print(i, root);
	}
};


int main()
{
	Tree *tr = new Tree();
	
	tr->Add(new NodeT1(12, "str12", "yeah", 45));
	tr->Add(new NodeT2(9,"ff","ko", 513, 2, Point(1.1, 23.09), Point(5.5, -2)));
	tr->Add(new NodeT1(19, "rand", "btw", 11));
	tr->Add(new NodeT3(15, "something wrong?", "tf", 98, 2, -4, 2.4));
	tr->Add(3, "test", "qwe", 0);
	tr->Add(1, "most", "wanted", 88, 2, Point(2.4,4.8), Point(8.4,4.2));
	tr->Add(11, "wow", "ow", 32);
	tr->Add(new NodeT3(6, "look what you did", ".", 408, 9, -11, 0.13));
	tr->Print();
	Output("Removing 6 and 9");
	tr->Remove(6);
	tr->Remove(9);
	tr->Print();
	Output(tr->Count());
}


void Output(string text)
{
	cout << text << endl;
}

void Output(int value)
{
	Output(to_string(value));
}