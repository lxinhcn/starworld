#ifndef _SINGLETON
#define _SINGLETON

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

namespace gof
{
	template< class T, int Int2Type = 0 >
	class singleton
	{
	private:
		explicit singleton(){}
		~singleton(){}

		explicit singleton( singleton& src ){}
		void operator=( singleton& rsh ){}

		enum { ValueType = Int2Type };

	public:
		static T& Instance();
	};

	template
	< 
		class T,
		int Int2Type
	>
	inline T& singleton< T, Int2Type >::Instance()
	{
		static T _inst;
		return _inst;
	}
}

#endif // _SINGLETON