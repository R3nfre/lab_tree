#include "AssCon.h"
#include "Comparer.h"
#include <iostream>
#include<vector>
#include<string>
#include<stack>

template<typename TKey, typename TValue>
class BST : public AssCon<TKey, TValue>
{
protected:
	class Node
	{
	public:
		Node()
		{
			right = nullptr;
			left = nullptr;
		}

		virtual ~Node() {}

		virtual void set(Node* nd)
		{
			this->key = nd->key;
			this->value = nd->value;
		}

		TKey key;
		TValue value;
		Node* right, * left;
	};

	class Inserting;
	class Removing;
	class Finding;

	BST(const Comparer<TKey>* cmp, Inserting* insert, Removing* remove, Finding* find)
	{
		inserter_cl = insert;
		remover_cl = remove;
		finder_cl = finde;
		comp = cmp;
		root = nullptr;
	}

	Node* root;
	Inserting* inserter_cl;
	Removing* remover_cl;
	Finding* finder_cl;
	const Comparer<TKey>* comp;

public:
	virtual BST& operator=(const BST<TKey, TValue>& tree)
	{
		if (root != nullptr)
			clean(root);
		root = copy(this->root, tree.root);
		return *this;
	}

	BST(const BST<TKey, TValue>& tree)
	{
		comp = tree.comp;
		this->copyConstructor();
		copy(tree.root, this->root);
	}

	BST(const Comparer<TKey>* cmp)
	{
		inserter_cl = new Inserting(this);
		remover_cl = new Removing(this);
		finder_cl = new Finding(this);
		comp = cmp;
		root = nullptr;
	}

	virtual ~BST()
	{
		delete inserter_cl, remover_cl, finder_cl;
		if (root != nullptr)
			clean(root);
	}

	virtual void prefix(void (*func)(TKey&, TValue&, int&))
	{
		_prefix(func, root);
	}

	virtual void infix(void (*func)(TKey&, TValue&, int&))
	{
		_infix(func, root);
	}

	virtual void postfix(void (*func)(TKey&, TValue&, int&))
	{
		_postfix(func, root);
	}

	void insert(const TKey& key, const TValue& value) override
	{
		(*inserter_cl)(key, value);
	}

	void remove(const TKey& key) override
	{
		(*remover_cl)(key);
	}

	TValue& find(const TKey& key) override
	{
		return (*finder_cl)(key);
	}

protected:
	virtual void copyConstructor()
	{
		inserter_cl = new Inserting(this);
		remover_cl = new Removing(this);
		finder_cl = new Finding(this);
	}

	virtual Node* copy(Node* in, Node* out)
	{
		if (in == nullptr)
			return out;
		out = new Node;
		out->set(in);
		out->left = copy(in->left, out->left);
		out->right = copy(in->right, out->right);
		return out;
	}

	virtual void _prefix(void (*func)(TKey&, TValue&, int&), Node* cur_node, int deep = 0)
	{
		if (cur_node == nullptr)
			return;

		func((cur_node)->key, (cur_node)->value, deep);
		_prefix(func, cur_node->left, deep + 1);
		_prefix(func, cur_node->right, deep + 1);
	}

	virtual void _infix(void (*func)(TKey&, TValue&, int&), Node* cur_node, int deep = 0)
	{
		if (cur_node == nullptr)
			return;

		_infix(func, cur_node->left, deep + 1);
		func(cur_node->key, cur_node->value, deep);
		_infix(func, cur_node->right, deep + 1);
	}

	virtual void _postfix(void (*func)(TKey&, TValue&, int&), Node* cur_node, int deep = 0)
	{
		if (cur_node == nullptr)
			return;

		_postfix(func, cur_node->left, deep + 1);
		_postfix(func, cur_node->right, deep + 1);
		func(cur_node->key, cur_node->value, deep);
	}

	class Inserting
	{
	public:
		Inserting(BST<TKey, TValue>* obj)
		{
			this->obj = obj;
		}

		virtual int operator()(const TKey& key, const TValue& value)
		{
			return _insert(key, value);
		}

	protected:
		Inserting() {}
		BST<TKey, TValue>* obj = nullptr;

		virtual int _insert(const TKey& key, const TValue& value)
		{
			if (obj->root == nullptr)
			{
				obj->root = new Node;
				obj->root->key = key;
				obj->root->value = value;
				return 0;
			}
			stack<Node*> stack;

			int dir = obj->down(stack, key);
			Node* tmp = stack.top();
			if (dir == 1)
			{
				tmp->right = new Node;
				tmp->right->key = key;
				tmp->right->value = value;
				return 0;
			}
			else if (dir == -1)
			{
				tmp->left = new Node;
				tmp->left->key = key;
				tmp->left->value = value;
				return 0;
			}
			else
				tmp->value = value;
			return 0;
		}
	};

	class Removing
	{
	public:
		Removing(BST<TKey, TValue>* obj)
		{
			this->obj = obj;
		}

		virtual int operator()(const TKey& key)
		{
			return _remove(key);
		}

	protected:
		Removing() {}
		BST<TKey, TValue>* obj = nullptr;

		virtual int _remove(const TKey& key)
		{
			Node* tmp = obj->root, * tmp_2 = obj->root;
			stack<Node*> stack;

			if ((tmp == nullptr) || obj->down(stack, key))
				throw typename BST<TKey, TValue>::Remove_Err();
			tmp = stack.top();
			stack.pop();
			if (!stack.empty())
				tmp_2 = stack.top();
			else
				tmp_2 = tmp;
			stack.push(tmp);

			deleting(stack, tmp, tmp_2);
			delete stack.top();
			return 0;
		}

		void deleting(stack<Node*>& stack, Node* tmp, Node* tmp_2)
		{
			int a = 0;

			if ((tmp->right == nullptr) && (tmp->left == nullptr))
			{
				if (tmp_2->right == tmp)
					tmp_2->right = nullptr;
				else if (tmp_2 == tmp)
					obj->root = nullptr;
				else
					tmp_2->left = nullptr;
			}
			else if ((tmp->right != nullptr) != (tmp->left != nullptr))
			{
				if (tmp->right != nullptr)
					a = 1;
				else
					a = -1;
				if (tmp == obj->root)
					obj->root = obj->get(a, tmp);
				else if (tmp_2->right == tmp)
					tmp_2->right = obj->get(a, tmp);
				else
					tmp_2->left = obj->get(a, tmp);
			}
			else if ((tmp->right != nullptr) && (tmp->left != nullptr))
			{
				tmp_2 = tmp;
				Node* tmp_3 = tmp_2;
				tmp = tmp->right;
				stack.push(tmp);
				while (tmp->left != nullptr)
				{
					tmp_3 = tmp;
					tmp = tmp->left;
					stack.push(tmp);
				}
				if (tmp_3->right == tmp)
					tmp_3->right = tmp->right;
				else
					tmp_3->left = tmp->right;

				tmp_2->key = tmp->key;
				tmp_2->value = tmp->value;
			}
		}
	};

	class Finding
	{
	public:
		Finding(BST<TKey, TValue>* obj)
		{
			this->obj = obj;
		}

		virtual TValue& operator()(const TKey& key)
		{
			return _find(key);
		}

	protected:
		Finding() {}
		BST<TKey, TValue>* obj = nullptr;

		virtual TValue& _find(const TKey& key)
		{
			Node* tmp = obj->root;
			while (true)
			{
				if (tmp == nullptr)
					throw Find_Err();
				switch (obj->comp->compare(key, tmp->key))
				{
				case 1:
					tmp = tmp->right;
					break;
				case -1:
					tmp = tmp->left;
					break;
				default:
					return tmp->value;
					break;
				}
			}
		}
	};

	int down(stack<Node*>& stack, const TKey& key)
	{
		Node* tmp = root;
		int dir = 0;
		stack.push(root);
		while (dir = comp->compare(key, tmp->key))
		{
			if (dir == 1)
			{
				if (tmp->right == nullptr)
					return 1;
				tmp = tmp->right;
			}
			else
			{
				if (tmp->left == nullptr)
					return -1;
				tmp = tmp->left;
			}
			stack.push(tmp);
		}
		return 0;
	}

	int clean(Node* cur_node)
	{
		if (cur_node->left != nullptr)
			clean(cur_node->left);
		if (cur_node->right != nullptr)
			clean(cur_node->right);
		delete cur_node;

		return 0;
	}

	Node*& get(int a, Node* node)
	{
		if (a == 1)
			return node->right;
		else if (a == -1)
			return node->left;
	}

public:
	class Tree_Ex : public exception
	{
	public:
		Tree_Ex() {}
		Tree_Ex(const string& about)
		{
			inf = about;
		}

		const char* what() const noexcept
		{
			return inf.c_str();
		}

	protected:
		string inf;
	};

	class Insert_Err : public Tree_Ex
	{
	public:
		Insert_Err()
		{
			Tree_Ex::inf = "ќшибка вставки: элемент с таким ключом уже существует!";
		}
	};

	class Remove_Err : public Tree_Ex
	{
	public:
		Remove_Err()
		{
			Tree_Ex::inf = "ќшибка удалени¤: элемента с таким ключом не существует!";
		}
	};

	class Find_Err : public Tree_Ex
	{
	public:
		Find_Err()
		{
			Tree_Ex::inf = "ќшибка поиска: элемента с таким ключом не существует!";
		}
	};
};