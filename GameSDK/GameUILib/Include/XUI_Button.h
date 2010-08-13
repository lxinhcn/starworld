#ifndef BUTTON_H
#define BUTTON_H
#include "XUI_Wnd.h"
namespace XGC
{
	namespace ui
	{
		class XUI_Button : public XUI_Wnd
		{
			friend class XUI_Lua;
		public:
			enum ButtonState
			{
				Normal,
				MouseOver,
				ButtonDown,
				Disable,
				State_count,
			};
			XUI_Button(void);
			~XUI_Button(void);

		protected:
			_string			m_strCaption;
			XUI_ISprite*	m_pButtonSkin;
			ButtonState		m_enState;
			_uint32			m_dwColor;

			void RenderSelf( const iPoint& adjust );

			bool OnMouseEnter();
			bool OnMouseLeave();
			bool OnButtonDown(int button, const iPoint& pt, UINT sysKeys);
			bool OnButtonUp(int button, const iPoint& pt, UINT sysKeys);

			bool OnKeyDown(_uint32 keycode, UINT sysKeys);

			//获得焦点
			virtual void OnSetFocus();
			//失去焦点
			virtual void OnLostFocus();

		protected:
			void OnEnable( BOOL bEnable );
			virtual void OnLoadPropertys( const char* name, TiXmlElement* pNode );

		public:
			void SetText( _lpctstr sText);
			void SetTextColor( _uint32 dwColor ){ m_dwColor = dwColor; }
			void SetState( ButtonState enState );
		};
	}
}
#endif
