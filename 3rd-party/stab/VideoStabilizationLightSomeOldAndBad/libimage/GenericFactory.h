#include <map>
#include <boost/shared_ptr.hpp>

template <class AncestorType, class ManufacturedType, typename ClassIDKey=std::string>
class RegisterInFactory;


template <class ManufacturedType, typename ClassIDKey=std::string>
class GenericFactory
{
private:
	typedef boost::shared_ptr<ManufacturedType> (*InstanseComposerFunction)();
	typedef std::map<ClassIDKey, InstanseComposerFunction> FactoryMap;
	FactoryMap _factoryMap;

public:
	static GenericFactory & getInstance()
	{
		static GenericFactory instance;
		return instance;
	}

	void registerFactoryItem(const ClassIDKey  & aString, InstanseComposerFunction aInstanseComposerFunction)
	{
		_factoryMap[aString] = aInstanseComposerFunction;
	}

	boost::shared_ptr<ManufacturedType> create(const ClassIDKey & aString) const
	{
		FactoryMap::const_iterator item = _factoryMap.find(aString);
		if (item == _factoryMap.end()) 
		{
			throw new std::runtime_error("class not found in factory");
		}
		return item->second();
	}
};

template <class AncestorType, class ManufacturedType, typename ClassIDKey>
class RegisterInFactory
{
public:
	static boost::shared_ptr<AncestorType> CreateInstance()
	{
		return boost::shared_ptr<AncestorType>(new ManufacturedType);
	}

	RegisterInFactory(const ClassIDKey &id)

	{
		GenericFactory<AncestorType>::getInstance().registerFactoryItem(id, CreateInstance);
	}
};