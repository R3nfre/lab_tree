template<typename TKey, typename TValue>
class AssCon
{
public:
	virtual void insert(const TKey& key, const TValue& value) = 0;
	virtual void remove(const TKey& key) = 0;
	virtual TValue& find(const TKey& key) = 0;
	virtual ~Ass_Cont() = default;
};