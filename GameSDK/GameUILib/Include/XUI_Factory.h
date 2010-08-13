#ifndef UIFACTORY_H
#define UIFACTORY_H

#include <algorithm>
namespace XGC
{
	namespace ui
	{
		// UI工厂类
		class XUI_Wnd;
		class XUI_Factory
		{
		protected:
			XUI_Factory(void){};
			~XUI_Factory(void){};

		public:
			// 创建函数
			typedef XUI_Wnd* (*pfnCreator)(void);

			// 单件模式
			static XUI_Factory& GetInstance();

			// 注册一个类型
			void Regist( const type_info &ti, pfnCreator pfunc )
			{
				char name[1024];
				const char *pSub = strstr( ti.name(), "XUI_" );
				ASSERT( pSub );
				CUIFactoryMap::const_iterator citer = m_FactoryMap.find( pSub + 4 );
				if( citer != m_FactoryMap.end() )	return;
				std::transform( pSub + 4, pSub + strlen(pSub) + 1, name, tolower );
				m_FactoryMap[name] = pfunc;
			}

			size_t GetXUI_WndCount()const
			{
				return m_FactoryMap.size();
			}

			XUI_Wnd* Creator( _lpcstr name )
			{
				char realname[1024];
				std::transform( name, name + strlen(name) + 1, realname, tolower );

				CUIFactoryMap::const_iterator citer = m_FactoryMap.find( realname );
				if( citer != m_FactoryMap.end() )
				{
					return citer->second();
				}
				return NULL;
			}

		private:
			typedef std::map< _astring, pfnCreator >	CUIFactoryMap;
			CUIFactoryMap	m_FactoryMap;
		};

		template< typename UIType >
		class XUI_ControlRegister
		{
		public:
			explicit XUI_ControlRegister()
			{
				XUI_Factory::GetInstance().Regist( typeid( UIType ), CreateControl );
			}

			static XUI_Wnd* CreateControl()
			{
				return new UIType;
			}
		};
	}
};
#endif // UIFACTORY_H
