#include "_BST.h"

template <class TKey, class TValue>
class AVL : public BST<TKey, TValue>
{
private:
	class AVL_Node : public  BST<TKey, TValue>::Node
	{
	public:
		void set(typename BST<TKey, TValue>::Node* nd) 
		{
			AVL_Node* tmp = dynamic_cast<AVL_Node*>(nd);
			this->key = tmp->key;
			this->value = tmp->value;
			this->level = tmp->level;
		}

		int level = 0;
	};

public:
	AVL(const Comparer<TKey>* cmp) : BST<TKey, TValue>(cmp, new Inserting(this), new Removing(this), new Finding(this)) {}
	AVL(const AVL<TKey, TValue>& tree) : BST<TKey, TValue>(tree)
	{
		copyConstructor();
		this->copy(tree.root, this->root);
	}
	~AVL() {}

private:
	void copyConstructor() override
	{
		this->inserter_cl = new Inserting(this);
		this->remover_cl = new Removing(this);
		this->finder_cl = new Finding(this);
	}

	class Inserting : public BST<TKey, TValue>::Inserting
	{
	public:
		Inserting(AVL<TKey, TValue>* obj)
		{
			this->obj = obj;
		}

		int operator()(const TKey& key, const TValue& value) override
		{
			return _insert(key, value);
		}

	protected:
		AVL<TKey, TValue>* obj;

		int _insert(const TKey& key, const TValue& value) override
		{
			stack<typename BST<TKey, TValue>::Node*> stack;
			AVL_Node* P, * T;

			if (obj->root == nullptr)
			{
				obj->root = new AVL_Node;
				obj->root->value = value;
				obj->root->key = key;
				return 0;
			}

			int dir = obj->down(stack, key);
			P = dynamic_cast<AVL_Node*>(stack.top());
			if (dir == 1)
			{
				P->right = new AVL_Node;
				P->right->value = value;
				P->right->key = key;
			}
			else if (dir == -1)
			{
				P->left = new AVL_Node;
				P->left->value = value;
				P->left->key = key;
			}
			else
				P->value = value;

			do
			{
				if (stack.empty())
					break;
				P = dynamic_cast<AVL_Node*>(stack.top());
				stack.pop();
				if (obj->comp->compare(key, P->key) == -1)
					P->level -= 1;
				else if (obj->comp->compare(key, P->key) == 1)
					P->level += 1;
			} while (abs(P->level) == 1);

			if (stack.empty())
				T = P;
			else
				T = dynamic_cast<AVL_Node*>(stack.top());
			if (P == dynamic_cast<AVL_Node*>(T->right))
				T->right = obj->balance(P);
			else if (P == dynamic_cast<AVL_Node*>(T->left))
				T->left = obj->balance(P);
			else
				obj->root = obj->balance(P);
			return 0;
		}
	};

	class Removing : public BST<TKey, TValue>::Removing
	{
	public:
		Removing(AVL<TKey, TValue>* obj) : BST<TKey, TValue>::Removing(obj)
		{
			this->obj = obj;
		}

		int operator()(const TKey& key) override
		{
			return _remove(key);
		}

	protected:
		AVL<TKey, TValue>* obj;

		int _remove(const TKey& key) override
		{
			AVL_Node* tmp = dynamic_cast<AVL_Node*>(obj->root), * tmp_2 = tmp, * tmp_3 = nullptr;
			TKey deleted_key = key;
			AVL_Node* S = tmp, * P = tmp, * T = tmp;
			stack<typename BST<TKey, TValue>::Node*> stack;

			int cur_bf = 0;

			if ((tmp == nullptr) || obj->down(stack, key))
				throw typename BST<TKey, TValue>::Remove_Err();

			tmp = dynamic_cast<AVL_Node*>(stack.top());
			stack.pop();
			if (!stack.empty())
				tmp_2 = dynamic_cast<AVL_Node*>(stack.top());
			else
				tmp_2 = tmp;
			stack.push(tmp);

			this->deleting(stack, tmp, tmp_2);
			deleted_key = stack.top()->key;
			delete stack.top();
			stack.pop();

			do
			{
				if (stack.empty())
					break;
				P = dynamic_cast<AVL_Node*>(stack.top());
				stack.pop();
				cur_bf = P->level;

				if (obj->comp->compare(deleted_key, P->key) == 0)
					P->level -= 1;
				else if (obj->comp->compare(key, P->key) == -1)
					P->level += 1;
				else if (obj->comp->compare(key, P->key) == 1)
					P->level -= 1;

				if (abs(P->level) == 2)
				{
					if (!stack.empty())
					{
						bool break_flag = false;
						T = dynamic_cast<AVL_Node*>(stack.top());
						if (T->right == P)
							T->right = obj->balance(P, &break_flag);
						else
							T->left = obj->balance(P, &break_flag);
						if (break_flag)
							break;
					}
					else
						obj->root = obj->balance(P);
				}
			} while (abs(cur_bf) == 1);
			return 0;
		}
	};

	class Finding : public BST<TKey, TValue>::Finding
	{
	public:
		Finding(AVL<TKey, TValue>* obj)
		{
			this->obj = obj;
		}

		TValue& operator()(const TKey& key) override
		{
			return _find(key);
		}

	protected:
		AVL<TKey, TValue>* obj;

		TValue& _find(const TKey& key) override
		{
			AVL_Node* tmp = dynamic_cast<AVL_Node*>(obj->root);
			while (true)
			{
				if (tmp == nullptr)
					throw typename BST<TKey, TValue>::Find_Err();
				switch (obj->comp->compare(key, tmp->key))
				{
				case 1:
					tmp = dynamic_cast<AVL_Node*>(tmp->right);
					break;
				case -1:
					tmp = dynamic_cast<AVL_Node*>(tmp->left);
					break;
				default:
					return tmp->value;
					break;
				}
			}
		}
	};

	AVL_Node* balance(AVL_Node* S, bool* breaker = nullptr)
	{
		AVL_Node* R = nullptr, * P = nullptr;
		int a;

		if (S->level == -2)
			a = -1;
		else if (S->level == 2)
			a = 1;
		else
			return S;

		R = dynamic_cast<AVL_Node*>(this->get(a, S));
		if ((breaker != nullptr) && (R->level == 0))
			*breaker = true;
		if (R->level != -a)
		{
			P = R;
			this->get(a, S) = this->get(-a, R);
			this->get(-a, R) = S;
			if (R->level == 0)
			{
				S->level = a;
				R->level = -a;
			}
			else
			{
				S->level = 0;
				R->level = 0;
			}
		}
		else if (R->level == -a)
		{
			P = dynamic_cast<AVL_Node*>(this->get(-a, R));
			this->get(-a, R) = this->get(a, P);
			this->get(a, P) = R;
			this->get(a, S) = this->get(-a, P);
			this->get(-a, P) = S;
			if (P->level == a)
			{
				S->level = -a;
				R->level = 0;
			}
			else if (!P->level)
				S->level = R->level = 0;
			else if (P->level == -a)
			{
				S->level = 0;
				R->level = a;
			}
		}
		return P;
	}
};