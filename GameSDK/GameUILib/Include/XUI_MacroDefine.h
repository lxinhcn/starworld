/*********************************************************************/
/*
    Create:     2006/06/16 6:2006   15:42
    FileName:   GuiMacroDefine.h
    Author:     Albert xu

    Levle:      0
	
	这里定义了消息影射表定义宏和消息映射表项的转换宏，
	UI_COMMAND	为菜单消息，事件编号为0
	UI_CONTROL	为各个控件的事件定义，比如按钮的Click事件DbClick事件
	UI_NOTIFY	为控件的通知事件，比如 NC_CREATE。

	消息反射机制暂时没有定义，需要在以后加入。
*/
/*********************************************************************/
#define OFFSET( structure, member )	((DWORD_PTR)&((structure*)0)->member)
#define XUI_LBUTTON 0
#define XUI_RBUTTON 1
#define XUI_MBUTTON 2
#define XUILOG	_T("XUI")

//////////////////////////////////////////////////////////////////////////
// 消息映射定义的宏
//////////////////////////////////////////////////////////////////////////
#define DECLARE_UIMSG_MAP() \
private: \
	static const UI_MSGMAP_ENTRY _messageEntries[]; \
protected: \
	static const UI_MSGMAP messageMap; \
	virtual const UI_MSGMAP* GetMessageMap() const; \

#define BEGIN_UIMSG_MAP(theClass, baseClass) \
const UI_MSGMAP* theClass::GetMessageMap() const \
{ return &theClass::messageMap; }; \
	__declspec( selectany ) const UI_MSGMAP theClass::messageMap = \
{ &baseClass::messageMap, &theClass::_messageEntries[0] }; \
	typedef theClass ThisClass;\
	typedef baseClass TheBassClass;\
	__declspec( selectany ) const UI_MSGMAP_ENTRY theClass::_messageEntries[] = \
{ \

#define uimsg

#define END_UIMSG_MAP() \
{ 0, 0, 0, 0, uiSig_end, (UI_PMSG)0 }\
};\

//////////////////////////////////////////////////////////////////////////
/*
** HGE Virtual-key codes
*/
#define XUIK_LBUTTON	0x01
#define XUIK_RBUTTON	0x02
#define XUIK_MBUTTON	0x04

#define XUIK_ESCAPE		0x1B
#define XUIK_BACKSPACE	0x08
#define XUIK_TAB		0x09
#define XUIK_ENTER		0x0D
#define XUIK_SPACE		0x20

#define XUIK_SHIFT		0x10
#define XUIK_CTRL		0x11
#define XUIK_ALT		0x12

#define XUIK_LWIN		0x5B
#define XUIK_RWIN		0x5C
#define XUIK_APPS		0x5D

#define XUIK_PAUSE		0x13
#define XUIK_CAPSLOCK	0x14
#define XUIK_NUMLOCK	0x90
#define XUIK_SCROLLLOCK	0x91

#define XUIK_PGUP		0x21
#define XUIK_PGDN		0x22
#define XUIK_HOME		0x24
#define XUIK_END		0x23
#define XUIK_INSERT		0x2D
#define XUIK_DELETE		0x2E

#define XUIK_LEFT		0x25
#define XUIK_UP			0x26
#define XUIK_RIGHT		0x27
#define XUIK_DOWN		0x28

#define XUIK_0			0x30
#define XUIK_1			0x31
#define XUIK_2			0x32
#define XUIK_3			0x33
#define XUIK_4			0x34
#define XUIK_5			0x35
#define XUIK_6			0x36
#define XUIK_7			0x37
#define XUIK_8			0x38
#define XUIK_9			0x39

#define XUIK_A			0x41
#define XUIK_B			0x42
#define XUIK_C			0x43
#define XUIK_D			0x44
#define XUIK_E			0x45
#define XUIK_F			0x46
#define XUIK_G			0x47
#define XUIK_H			0x48
#define XUIK_I			0x49
#define XUIK_J			0x4A
#define XUIK_K			0x4B
#define XUIK_L			0x4C
#define XUIK_M			0x4D
#define XUIK_N			0x4E
#define XUIK_O			0x4F
#define XUIK_P			0x50
#define XUIK_Q			0x51
#define XUIK_R			0x52
#define XUIK_S			0x53
#define XUIK_T			0x54
#define XUIK_U			0x55
#define XUIK_V			0x56
#define XUIK_W			0x57
#define XUIK_X			0x58
#define XUIK_Y			0x59
#define XUIK_Z			0x5A

#define XUIK_GRAVE		0xC0
#define XUIK_MINUS		0xBD
#define XUIK_EQUALS		0xBB
#define XUIK_BACKSLASH	0xDC
#define XUIK_LBRACKET	0xDB
#define XUIK_RBRACKET	0xDD
#define XUIK_SEMICOLON	0xBA
#define XUIK_APOSTROPHE	0xDE
#define XUIK_COMMA		0xBC
#define XUIK_PERIOD		0xBE
#define XUIK_SLASH		0xBF

#define XUIK_NUMPAD0	0x60
#define XUIK_NUMPAD1	0x61
#define XUIK_NUMPAD2	0x62
#define XUIK_NUMPAD3	0x63
#define XUIK_NUMPAD4	0x64
#define XUIK_NUMPAD5	0x65
#define XUIK_NUMPAD6	0x66
#define XUIK_NUMPAD7	0x67
#define XUIK_NUMPAD8	0x68
#define XUIK_NUMPAD9	0x69

#define XUIK_MULTIPLY	0x6A
#define XUIK_DIVIDE		0x6F
#define XUIK_ADD		0x6B
#define XUIK_SUBTRACT	0x6D
#define XUIK_DECIMAL	0x6E

#define XUIK_F1			0x70
#define XUIK_F2			0x71
#define XUIK_F3			0x72
#define XUIK_F4			0x73
#define XUIK_F5			0x74
#define XUIK_F6			0x75
#define XUIK_F7			0x76
#define XUIK_F8			0x77
#define XUIK_F9			0x78
#define XUIK_F10		0x79
#define XUIK_F11		0x7A
#define XUIK_F12		0x7B
