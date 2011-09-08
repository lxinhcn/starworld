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
			xuiSprite		m_hButtonSkin[4];
			xuiSpriteInfo	m_ButtonInfo[4];
			ButtonState		m_eState;
			_uint32			m_dwColor;

			void RenderSelf( const iPoint& adjust );

			bool OnMouseEnter();
			bool OnMouseLeave();
			bool OnButtonDown( int nButton, const iPoint& ptMouse, _uint32 nFlags );
			bool OnButtonUp( int nButton, const iPoint& ptMouse, _uint32 nFlags );

			bool OnKeyDown( _uint32 nKey, _uint32 nFlags );

			//获得焦点
			virtual void OnSetFocus();
			//失去焦点
			virtual void OnLostFocus();

		protected:
			void OnEnable( bool bEnable );
			virtual void OnLoadPropertys( const char* pName, TiXmlElement* pNode );

		public:
			void SetText( _lpctstr sText);
			void SetTextColor( _uint32 dwColor ){ m_dwColor = dwColor; }
			void SetState( ButtonState enState );
		};
	}
}
#endif
