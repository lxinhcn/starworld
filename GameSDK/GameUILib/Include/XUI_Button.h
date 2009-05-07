#ifndef BUTTON_H
#define BUTTON_H
#include "XUI_Wnd.h"
namespace UILib
{
	class XUI_Button;
	struct XUI_ISprite;

	class XUI_Button : public UIObjTypeT< XUI_Wnd, TypeButton >
	{
	friend class LuaBindClass;
	DECLARE_UIMSG_MAP()
	DECLARE_LABLE( BUTTON )
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
		uint32			m_dwColor;

		void RenderSelf();

		bool onMouseEnter();
		bool onMouseLeave();
		bool onButtonDown(int button, const x_point& pt, UINT sysKeys);
		bool onButtonUp(int button, const x_point& pt, UINT sysKeys);
		
		bool onKeyDown(uint32 keycode, UINT sysKeys);

		//获得焦点
		virtual void onGetFocus();
		//失去焦点
		virtual void onLostFocus();

	protected:
		void OnEnable( BOOL bEnable );

	public:
		void SetText( _lpctstr sText);
		void SetTextColor( uint32 dwColor ){ m_dwColor = dwColor; }
		void SetState( ButtonState enState );
	};
}
#endif
