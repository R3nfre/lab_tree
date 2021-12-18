
template<typename T>
class Comparer
{
public:
	virtual int compare(const T& left, const T& right) const = 0;
	virtual ~Comparer() = default;
};