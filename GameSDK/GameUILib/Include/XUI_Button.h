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
		SpriteAttribute	m_SpriteAttribute;

		XUI_ISprite*	m_pButtonSkin[State_count];
		ButtonState		m_enState;
		DWORD			m_dwColor;

		void RenderSelf(const CRect& clipper);
		bool onMouseEnter();
		bool onMouseLeave();
		bool onButtonDown(int button, const CPoint& pt, UINT sysKeys);
		bool onButtonUp(int button, const CPoint& pt, UINT sysKeys);
		
		bool onKeyDown(DWORD keycode, UINT sysKeys);

		//获得焦点
		virtual void onGetFocus();
		//失去焦点
		virtual void onLostFocus();

		// 皮肤改变时调用
		void onSkinChange();
	protected:
		uimsg void OnEnable( BOOL bEnable );

		LRESULT OnSkinChanged( SpriteAttribute& attrib );
	public:

		// 设置控件属性
		void			SetSkin( const _string& path, float x, float y, float w, float h );
		XUI_ISprite*	GetSkin( int idx )const{ return( idx >= 0 && idx < _countof(m_pButtonSkin) )?m_pButtonSkin[idx]:NULL; }

		void SetText( LPCTSTR sText);
		void SetTextColor( DWORD dwColor ){ m_dwColor = dwColor; }
		void SetState( ButtonState enState );
	};
}
#endif
