#include "_BST.h"

template<class TKey, class TValue>
class Splay : public BST<TKey, TValue>
{
public:
	Splay(const Comparer<TKey>* cmp) : BST<TKey, TValue>(cmp, new Inserting(this), new Removing(this), new Finding(this)) {}

private:
	enum ZZ { ZIG = -1, ZAG = 1, NONE = 0 };

	void copyConstructor() override
	{
		this->inserter_cl = new Inserting(this);
		this->remover_cl = new Removing(this);
		this->finder_cl = new Finding(this);
	}

	Splay(const Splay<TKey, TValue>& tree) : BST<TKey, TValue>(tree)
	{
		copyConstructor();
		this->copy(tree.root, this->root);
	}

	class Inserting : public BST<TKey, TValue>::Inserting
	{
	public:
		Inserting(Splay<TKey, TValue>* obj)
		{
			this->obj = obj;
		}

		int operator()(const TKey& key, const TValue& value) override
		{
			return _insert(key, value);
		}

	private:
		Splay<TKey, TValue>* obj;

		int _insert(const TKey& key, const TValue& value) override
		{
			if (obj->root == nullptr)
			{
				obj->root = new typename BST<TKey, TValue>::Node;
				obj->root->key = key;
				obj->root->value = value;
				return 0;
			}

			typename BST<TKey, TValue>::Node* cur_node = obj->root;
			stack<typename BST<TKey, TValue>::Node**> stack;
			stack.push(&obj->root);

			while (true)
			{
				if (obj->comp->compare(key, cur_node->key) == 1)
				{
					stack.push(&cur_node->right);
					if (cur_node->right == nullptr)
					{
						cur_node->right = new typename BST<TKey, TValue>::Node;
						cur_node->right->key = key;
						cur_node->right->value = value;
						break;
					}
					cur_node = cur_node->right;
				}
				else if (obj->comp->compare(key, cur_node->key) == -1)
				{
					stack.push(&cur_node->left);
					if (cur_node->left == nullptr)
					{
						cur_node->left = new typename BST<TKey, TValue>::Node;
						cur_node->left->key = key;
						cur_node->left->value = value;
						break;
					}
					cur_node = cur_node->left;
				}
				else
					throw typename BST<TKey, TValue>::Insert_Err();
			}
			obj->splay_f(stack);
			return 0;
		}
	};

	class Removing : public BST<TKey, TValue>::Removing
	{
	public:
		Removing(Splay<TKey, TValue>* obj)
		{
			this->obj = obj;
		}

		int operator()(const TKey& key) override
		{
			return _remove(key);
		}

	private:
		Splay<TKey, TValue>* obj;

		int _remove(const TKey& key) override
		{
			stack<typename BST<TKey, TValue>::Node**> stack;
			typename BST<TKey, TValue>::Node* tmp = obj->root;
			stack.push(&obj->root);

			while (true)
			{
				if (tmp == nullptr)
					throw typename BST<TKey, TValue>::Remove_Err();
				if (obj->comp->compare(key, tmp->key) == 1)
				{
					stack.push(&tmp->right);
					tmp = tmp->right;
				}
				else if (obj->comp->compare(key, tmp->key) == -1)
				{
					stack.push(&tmp->left);
					tmp = tmp->left;
				}
				else
				{
					obj->splay_f(stack);
					break;
				}
			}

			tmp = obj->root->right;
			if (tmp != nullptr)
			{
				stack.push(&obj->root->right);
				while (tmp->left != nullptr)
				{
					stack.push(&tmp->left);
					tmp = tmp->left;
				}

				obj->splay_f(stack);
				tmp->left = obj->root->left;
				delete obj->root;
				obj->root = tmp;
			}
			else
			{
				tmp = obj->root;
				obj->root = tmp->left;
				delete tmp;
			}
			return 0;
		}
	};

	class Finding : public BST<TKey, TValue>::Finding
	{
	public:
		Finding(Splay<TKey, TValue>* obj)
		{
			this->obj = obj;
		}

		TValue& operator()(const TKey& key) override
		{
			return _find(key);
		}

	private:
		Splay<TKey, TValue>* obj;

		TValue& _find(const TKey& key) override
		{
			stack<typename BST<TKey, TValue>::Node**> stack;
			typename BST<TKey, TValue>::Node* tmp = obj->root;

			stack.push(&obj->root);

			while (true)
			{
				if (tmp == nullptr)
					throw typename BST<TKey, TValue>::Find_Err();
				if (obj->comp->compare(key, tmp->key) == 1)
				{
					stack.push(&tmp->right);
					tmp = tmp->right;
				}
				else if (obj->comp->compare(key, tmp->key) == -1)
				{
					stack.push(&tmp->left);
					tmp = tmp->left;
				}
				else
				{
					obj->splay_f(stack);
					return tmp->value;
				}
			}
		}
	};

	void splay_f(stack<typename BST<TKey, TValue>::Node**>& stack)
	{
		typename BST<TKey, TValue>::Node* tmp, ** tmp_2, ** tmp_3, * cur_node = *(stack.top());
		stack.pop();
		ZZ a_1, a_2;

		while (stack.size() > 1)
		{
			tmp_3 = stack.top();
			tmp = *tmp_3;
			stack.pop();
			tmp_2 = stack.top();
			stack.pop();
			if ((tmp)->right == cur_node)
				a_1 = ZAG;
			else if ((tmp)->left == cur_node)
				a_1 = ZIG;
			if ((*tmp_2)->right == tmp)
				a_2 = ZAG;
			else if ((*tmp_2)->left == tmp)
				a_2 = ZIG;

			if (a_1 == a_2)
			{
				this->get(a_2, *tmp_2) = this->get(-a_2, tmp);
				this->get(-a_2, tmp) = *tmp_2;
				*tmp_2 = tmp;

				this->get(a_1, tmp) = this->get(-a_1, cur_node);
				this->get(-a_1, cur_node) = tmp;
			}
			else
			{
				this->get(a_1, tmp) = this->get(-a_1, cur_node);
				this->get(-a_1, cur_node) = tmp;
				*tmp_3 = cur_node;

				this->get(a_2, *tmp_2) = this->get(-a_2, cur_node);
				this->get(-a_2, cur_node) = *tmp_2;
			}
			*tmp_2 = cur_node;
		}

		if (!stack.empty())
		{
			tmp_2 = stack.top();
			stack.pop();
			if ((*tmp_2)->right == cur_node)
				a_1 = ZAG;
			else if ((*tmp_2)->left == cur_node)
				a_1 = ZIG;
			this->get(a_1, *tmp_2) = this->get(-a_1, cur_node);
			this->get(-a_1, cur_node) = *tmp_2;
			*tmp_2 = cur_node;
		}
	}
};