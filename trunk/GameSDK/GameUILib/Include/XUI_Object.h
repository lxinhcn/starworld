#ifndef _UIOBJECT_
#define _UIOBJECT_
#include "XUI_Header.h"
#include "XUI_Object.h"
/************************************************************************/
/* 列表系统，属性系统，以及对象基础类的实现                               */
/************************************************************************/
//#include <Windows.h>
//#define DECLARE_UI_PROPERTY()\
//	static uiProperty _uiProperty[];\
//	static uiPropertyTable _uiPropertyTable;\
//	static uiPropertyTable &GetPropertyTable()const{ return _uiPropertyTable; }\
//
//#define BEGIN_DECLARE_PROPERTY( BASE_CLASS, THIS_CLASS )\
//	THIS_CLASS##::PropertyTable _uiPropertyTable = { &##BASE_CLASS##::_uiPropertyTable, THIS_CLASS##::_uiProperty };\
//	THIS_CLASS##::Property _uiProperty[] = {\
//	
//#define DECLARE_PROPERTY( PROPERTY_NAME, PROPERTY_OFFSET )\
//	{ string(PROPERTY_NAME), (_uint16)THIS_CLASS::##PROPERTY_OFFSET },\
//
//#define END_DECLARE_PROPERTY()\
//	};\

namespace XGC
{
	typedef _uint32 uiIdentity;

	struct uiProperty
	{
		_string	name;
		_uint16	offset;
	};

	struct uiPropertyTable
	{
		uiPropertyTable *pParent;
		uiProperty*	pProperty;
	};

	namespace ui
	{
		class XUI_Object
		{
		protected:
			XUI_Object();
			virtual ~XUI_Object();

		public:
			uiIdentity GetID()const{ return _idcode; }

		private:
			uiIdentity _idcode;
		};
	}
}
#endif //_UIOBJECT_