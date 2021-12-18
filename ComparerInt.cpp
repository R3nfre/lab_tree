#include "Comparer.h"

class IntComparer : public Comparer<int>
{
public:
	int compare(const int& left, const int& right) const override
	{
		if (left > right)
			return 1;
		else if (left < right)
			return -1;
		return 0;
	}

	~IntComparer() override {}
};