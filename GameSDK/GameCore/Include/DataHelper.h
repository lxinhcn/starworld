#pragma once
#ifndef _DATEHELPER_
#define _DATEHELPER_

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

namespace XGC
{
	class x_size;
	class x_point;
	class x_rect;

	/////////////////////////////////////////////////////////////////////////////
	// x_size - An extent, similar to Windows SIZE structure.

	class CORE_API x_size : public tagSIZE
	{
	public:

		// Constructors
		// construct an uninitialized size
		x_size() throw();
		// create from two integers
		x_size(int initCX, int initCY) throw();
		// create from another size
		x_size(SIZE initSize) throw();
		// create from a point
		x_size(POINT initPt) throw();
		// create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
		x_size(DWORD dwSize) throw();

		// Operations
		BOOL operator==(SIZE size) const throw();
		BOOL operator!=(SIZE size) const throw();
		void operator+=(SIZE size) throw();
		void operator-=(SIZE size) throw();
		void SetSize(int CX, int CY) throw();

		// Operators returning x_size values
		x_size operator+(SIZE size) const throw();
		x_size operator-(SIZE size) const throw();
		x_size operator-() const throw();

		// Operators returning x_point values
		x_point operator+(POINT point) const throw();
		x_point operator-(POINT point) const throw();

		// Operators returning x_rect values
		x_rect operator+(const RECT* lpRect) const throw();
		x_rect operator-(const RECT* lpRect) const throw();
	};

	/////////////////////////////////////////////////////////////////////////////
	// x_point - A 2-D point, similar to Windows POINT structure.

	class CORE_API x_point : public tagPOINT
	{
	public:
		// Constructors

		// create an uninitialized point
		x_point() throw();
		// create from two integers
		x_point(int initX, int initY) throw();
		// create from another point
		x_point(POINT initPt) throw();
		// create from a size
		x_point(SIZE initSize) throw();
		// create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
		x_point(LPARAM dwPoint) throw();


		// Operations

		// translate the point
		void Offset(int xOffset, int yOffset) throw();
		void Offset(POINT point) throw();
		void Offset(SIZE size) throw();
		void SetPoint(int X, int Y) throw();

		BOOL operator==(POINT point) const throw();
		BOOL operator!=(POINT point) const throw();
		void operator+=(SIZE size) throw();
		void operator-=(SIZE size) throw();
		void operator+=(POINT point) throw();
		void operator-=(POINT point) throw();

		// Operators returning x_point values
		x_point operator+(SIZE size) const throw();
		x_point operator-(SIZE size) const throw();
		x_point operator-() const throw();
		x_point operator+(POINT point) const throw();

		// Operators returning x_size values
		x_size operator-(POINT point) const throw();

		// Operators returning x_rect values
		x_rect operator+(const RECT* lpRect) const throw();
		x_rect operator-(const RECT* lpRect) const throw();
	};

	/////////////////////////////////////////////////////////////////////////////
	// x_rect - A 2-D rectangle, similar to Windows RECT structure.

	class CORE_API x_rect : public tagRECT
	{
		// Constructors
	public:
		// uninitialized rectangle
		x_rect() throw();
		// from left, top, right, and bottom
		x_rect(int l, int t, int r, int b) throw();
		// copy constructor
		x_rect(const RECT& srcRect) throw();
		// from a pointer to another rect
		x_rect(LPCRECT lpSrcRect) throw();
		// from a point and size
		x_rect(POINT point, SIZE size) throw();
		// from two points
		x_rect(POINT topLeft, POINT bottomRight) throw();

		// Attributes (in addition to RECT members)

		// retrieves the width
		int Width() const throw();
		// returns the height
		int Height() const throw();
		// returns the size
		x_size Size() const throw();
		// reference to the top-left point
		x_point& TopLeft() throw();
		// reference to the bottom-right point
		x_point& BottomRight() throw();
		// const reference to the top-left point
		const x_point& TopLeft() const throw();
		// const reference to the bottom-right point
		const x_point& BottomRight() const throw();
		// the geometric center point of the rectangle
		x_point CenterPoint() const throw();
		// swap the left and right
		void SwapLeftRight() throw();
		static void WINAPI SwapLeftRight(LPRECT lpRect) throw();

		// convert between x_rect and LPRECT/LPCRECT (no need for &)
		operator LPRECT() throw();
		operator LPCRECT() const throw();

		// returns true if rectangle has no area
		BOOL IsRectEmpty() const throw();
		// returns true if rectangle is at (0,0) and has no area
		BOOL IsRectNull() const throw();
		// returns true if point is within rectangle
		BOOL PtInRect(POINT point) const throw();

		// Operations

		// set rectangle from left, top, right, and bottom
		void SetRect(int x1, int y1, int x2, int y2) throw();
		void SetRect(POINT topLeft, POINT bottomRight) throw();
		// empty the rectangle
		void SetRectEmpty() throw();
		// copy from another rectangle
		void CopyRect(LPCRECT lpSrcRect) throw();
		// true if exactly the same as another rectangle
		BOOL EqualRect(LPCRECT lpRect) const throw();

		// Inflate rectangle's width and height by
		// x units to the left and right ends of the rectangle
		// and y units to the top and bottom.
		void InflateRect(int x, int y) throw();
		// Inflate rectangle's width and height by
		// size.cx units to the left and right ends of the rectangle
		// and size.cy units to the top and bottom.
		void InflateRect(SIZE size) throw();
		// Inflate rectangle's width and height by moving individual sides.
		// Left side is moved to the left, right side is moved to the right,
		// top is moved up and bottom is moved down.
		void InflateRect(LPCRECT lpRect) throw();
		void InflateRect(int l, int t, int r, int b) throw();

		// deflate the rectangle's width and height without
		// moving its top or left
		void DeflateRect(int x, int y) throw();
		void DeflateRect(SIZE size) throw();
		void DeflateRect(LPCRECT lpRect) throw();
		void DeflateRect(int l, int t, int r, int b) throw();

		// translate the rectangle by moving its top and left
		void OffsetRect(int x, int y) throw();
		void OffsetRect(SIZE size) throw();
		void OffsetRect(POINT point) throw();
		void NormalizeRect() throw();

		// absolute position of rectangle
		void MoveToY(int y) throw();
		void MoveToX(int x) throw();
		void MoveToXY(int x, int y) throw();
		void MoveToXY(POINT point) throw();

		// set this rectangle to intersection of two others
		BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) throw();

		// set this rectangle to bounding union of two others
		BOOL UnionRect(LPCRECT lpRect1, LPCRECT lpRect2) throw();

		// set this rectangle to minimum of two others
		BOOL SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw();

		// Additional Operations
		void operator=(const RECT& srcRect) throw();
		BOOL operator==(const RECT& rect) const throw();
		BOOL operator!=(const RECT& rect) const throw();
		void operator+=(POINT point) throw();
		void operator+=(SIZE size) throw();
		void operator+=(LPCRECT lpRect) throw();
		void operator-=(POINT point) throw();
		void operator-=(SIZE size) throw();
		void operator-=(LPCRECT lpRect) throw();
		void operator&=(const RECT& rect) throw();
		void operator|=(const RECT& rect) throw();

		// Operators returning x_rect values
		x_rect operator+(POINT point) const throw();
		x_rect operator-(POINT point) const throw();
		x_rect operator+(LPCRECT lpRect) const throw();
		x_rect operator+(SIZE size) const throw();
		x_rect operator-(SIZE size) const throw();
		x_rect operator-(LPCRECT lpRect) const throw();
		x_rect operator&(const RECT& rect2) const throw();
		x_rect operator|(const RECT& rect2) const throw();
		x_rect MulDiv(int nMultiplier, int nDivisor) const throw();
	};
	inline x_size::x_size() throw()
	{ /* random filled */ }
	inline x_size::x_size(int initCX, int initCY) throw()
	{ cx = initCX; cy = initCY; }
	inline x_size::x_size(SIZE initSize) throw()
	{ *(SIZE*)this = initSize; }
	inline x_size::x_size(POINT initPt) throw()
	{ *(POINT*)this = initPt; }
	inline x_size::x_size(DWORD dwSize) throw()
	{
		cx = (short)LOWORD(dwSize);
		cy = (short)HIWORD(dwSize);
	}
	inline BOOL x_size::operator==(SIZE size) const throw()
	{ return (cx == size.cx && cy == size.cy); }
	inline BOOL x_size::operator!=(SIZE size) const throw()
	{ return (cx != size.cx || cy != size.cy); }
	inline void x_size::operator+=(SIZE size) throw()
	{ cx += size.cx; cy += size.cy; }
	inline void x_size::operator-=(SIZE size) throw()
	{ cx -= size.cx; cy -= size.cy; }
	inline void x_size::SetSize(int CX, int CY) throw()
	{ cx = CX; cy = CY; }	
	inline x_size x_size::operator+(SIZE size) const throw()
	{ return x_size(cx + size.cx, cy + size.cy); }
	inline x_size x_size::operator-(SIZE size) const throw()
	{ return x_size(cx - size.cx, cy - size.cy); }
	inline x_size x_size::operator-() const throw()
	{ return x_size(-cx, -cy); }
	inline x_point x_size::operator+(POINT point) const throw()
	{ return x_point(cx + point.x, cy + point.y); }
	inline x_point x_size::operator-(POINT point) const throw()
	{ return x_point(cx - point.x, cy - point.y); }
	inline x_rect x_size::operator+(const RECT* lpRect) const throw()
	{ return x_rect(lpRect) + *this; }
	inline x_rect x_size::operator-(const RECT* lpRect) const throw()
	{ return x_rect(lpRect) - *this; }

	// x_point
	inline x_point::x_point() throw()
	{ /* random filled */ }
	inline x_point::x_point(int initX, int initY) throw()
	{ x = initX; y = initY; }
	inline x_point::x_point(POINT initPt) throw()
	{ *(POINT*)this = initPt; }
	inline x_point::x_point(SIZE initSize) throw()
	{ *(SIZE*)this = initSize; }
	inline x_point::x_point(LPARAM dwPoint) throw()
	{
		x = (short)LOWORD(dwPoint);
		y = (short)HIWORD(dwPoint);
	}
	inline void x_point::Offset(int xOffset, int yOffset) throw()
	{ x += xOffset; y += yOffset; }
	inline void x_point::Offset(POINT point) throw()
	{ x += point.x; y += point.y; }
	inline void x_point::Offset(SIZE size) throw()
	{ x += size.cx; y += size.cy; }
	inline void x_point::SetPoint(int X, int Y) throw()
	{ x = X; y = Y; }
	inline BOOL x_point::operator==(POINT point) const throw()
	{ return (x == point.x && y == point.y); }
	inline BOOL x_point::operator!=(POINT point) const throw()
	{ return (x != point.x || y != point.y); }
	inline void x_point::operator+=(SIZE size) throw()
	{ x += size.cx; y += size.cy; }
	inline void x_point::operator-=(SIZE size) throw()
	{ x -= size.cx; y -= size.cy; }
	inline void x_point::operator+=(POINT point) throw()
	{ x += point.x; y += point.y; }
	inline void x_point::operator-=(POINT point) throw()
	{ x -= point.x; y -= point.y; }
	inline x_point x_point::operator+(SIZE size) const throw()
	{ return x_point(x + size.cx, y + size.cy); }
	inline x_point x_point::operator-(SIZE size) const throw()
	{ return x_point(x - size.cx, y - size.cy); }
	inline x_point x_point::operator-() const throw()
	{ return x_point(-x, -y); }
	inline x_point x_point::operator+(POINT point) const throw()
	{ return x_point(x + point.x, y + point.y); }
	inline x_size x_point::operator-(POINT point) const throw()
	{ return x_size(x - point.x, y - point.y); }
	inline x_rect x_point::operator+(const RECT* lpRect) const throw()
	{ return x_rect(lpRect) + *this; }
	inline x_rect x_point::operator-(const RECT* lpRect) const throw()
	{ return x_rect(lpRect) - *this; }

	// x_rect
	inline x_rect::x_rect() throw()
	{ /* random filled */ }
	inline x_rect::x_rect(int l, int t, int r, int b) throw()
	{ left = l; top = t; right = r; bottom = b; }
	inline x_rect::x_rect(const RECT& srcRect) throw()
	{ ::CopyRect(this, &srcRect); }
	inline x_rect::x_rect(LPCRECT lpSrcRect) throw()
	{ ::CopyRect(this, lpSrcRect); }
	inline x_rect::x_rect(POINT point, SIZE size) throw()
	{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
	inline x_rect::x_rect(POINT topLeft, POINT bottomRight) throw()
	{ left = topLeft.x; top = topLeft.y;
	right = bottomRight.x; bottom = bottomRight.y; }
	inline int x_rect::Width() const throw()
	{ return right - left; }
	inline int x_rect::Height() const throw()
	{ return bottom - top; }
	inline x_size x_rect::Size() const throw()
	{ return x_size(right - left, bottom - top); }
	inline x_point& x_rect::TopLeft() throw()
	{ return *((x_point*)this); }
	inline x_point& x_rect::BottomRight() throw()
	{ return *((x_point*)this+1); }
	inline const x_point& x_rect::TopLeft() const throw()
	{ return *((x_point*)this); }
	inline const x_point& x_rect::BottomRight() const throw()
	{ return *((x_point*)this+1); }
	inline x_point x_rect::CenterPoint() const throw()
	{ return x_point((left+right)/2, (top+bottom)/2); }
	inline void x_rect::SwapLeftRight() throw()
	{ SwapLeftRight(LPRECT(this)); }
	inline void WINAPI x_rect::SwapLeftRight(LPRECT lpRect) throw()
	{ LONG temp = lpRect->left; lpRect->left = lpRect->right; lpRect->right = temp; }
	inline x_rect::operator LPRECT() throw()
	{ return this; }
	inline x_rect::operator LPCRECT() const throw()
	{ return this; }
	inline BOOL x_rect::IsRectEmpty() const throw()
	{ return ::IsRectEmpty(this); }
	inline BOOL x_rect::IsRectNull() const throw()
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
	inline BOOL x_rect::PtInRect(POINT point) const throw()
	{ return ::PtInRect(this, point); }
	inline void x_rect::SetRect(int x1, int y1, int x2, int y2) throw()
	{ ::SetRect(this, x1, y1, x2, y2); }
	inline void x_rect::SetRect(POINT topLeft, POINT bottomRight) throw()
	{ ::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
	inline void x_rect::SetRectEmpty() throw()
	{ ::SetRectEmpty(this); }
	inline void x_rect::CopyRect(LPCRECT lpSrcRect) throw()
	{ ::CopyRect(this, lpSrcRect); }
	inline BOOL x_rect::EqualRect(LPCRECT lpRect) const throw()
	{ return ::EqualRect(this, lpRect); }
	inline void x_rect::InflateRect(int x, int y) throw()
	{ ::InflateRect(this, x, y); }
	inline void x_rect::InflateRect(SIZE size) throw()
	{ ::InflateRect(this, size.cx, size.cy); }
	inline void x_rect::DeflateRect(int x, int y) throw()
	{ ::InflateRect(this, -x, -y); }
	inline void x_rect::DeflateRect(SIZE size) throw()
	{ ::InflateRect(this, -size.cx, -size.cy); }
	inline void x_rect::OffsetRect(int x, int y) throw()
	{ ::OffsetRect(this, x, y); }
	inline void x_rect::OffsetRect(POINT point) throw()
	{ ::OffsetRect(this, point.x, point.y); }
	inline void x_rect::OffsetRect(SIZE size) throw()
	{ ::OffsetRect(this, size.cx, size.cy); }
	inline void x_rect::MoveToY(int y) throw()
	{ bottom = Height() + y; top = y; }
	inline void x_rect::MoveToX(int x) throw()
	{ right = Width() + x; left = x; }
	inline void x_rect::MoveToXY(int x, int y) throw()
	{ MoveToX(x); MoveToY(y); }
	inline void x_rect::MoveToXY(POINT pt) throw()
	{ MoveToX(pt.x); MoveToY(pt.y); }
	inline BOOL x_rect::IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{ return ::IntersectRect(this, lpRect1, lpRect2);}
	inline BOOL x_rect::UnionRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{ return ::UnionRect(this, lpRect1, lpRect2); }
	inline void x_rect::operator=(const RECT& srcRect) throw()
	{ ::CopyRect(this, &srcRect); }
	inline BOOL x_rect::operator==(const RECT& rect) const throw()
	{ return ::EqualRect(this, &rect); }
	inline BOOL x_rect::operator!=(const RECT& rect) const throw()
	{ return !::EqualRect(this, &rect); }
	inline void x_rect::operator+=(POINT point) throw()
	{ ::OffsetRect(this, point.x, point.y); }
	inline void x_rect::operator+=(SIZE size) throw()
	{ ::OffsetRect(this, size.cx, size.cy); }
	inline void x_rect::operator+=(LPCRECT lpRect) throw()
	{ InflateRect(lpRect); }
	inline void x_rect::operator-=(POINT point) throw()
	{ ::OffsetRect(this, -point.x, -point.y); }
	inline void x_rect::operator-=(SIZE size) throw()
	{ ::OffsetRect(this, -size.cx, -size.cy); }
	inline void x_rect::operator-=(LPCRECT lpRect) throw()
	{ DeflateRect(lpRect); }
	inline void x_rect::operator&=(const RECT& rect) throw()
	{ ::IntersectRect(this, this, &rect); }
	inline void x_rect::operator|=(const RECT& rect) throw()
	{ ::UnionRect(this, this, &rect); }
	inline x_rect x_rect::operator+(POINT pt) const throw()
	{ x_rect rect(*this); ::OffsetRect(&rect, pt.x, pt.y); return rect; }
	inline x_rect x_rect::operator-(POINT pt) const throw()
	{ x_rect rect(*this); ::OffsetRect(&rect, -pt.x, -pt.y); return rect; }
	inline x_rect x_rect::operator+(SIZE size) const throw()
	{ x_rect rect(*this); ::OffsetRect(&rect, size.cx, size.cy); return rect; }
	inline x_rect x_rect::operator-(SIZE size) const throw()
	{ x_rect rect(*this); ::OffsetRect(&rect, -size.cx, -size.cy); return rect; }
	inline x_rect x_rect::operator+(LPCRECT lpRect) const throw()
	{ x_rect rect(this); rect.InflateRect(lpRect); return rect; }
	inline x_rect x_rect::operator-(LPCRECT lpRect) const throw()
	{ x_rect rect(this); rect.DeflateRect(lpRect); return rect; }
	inline x_rect x_rect::operator&(const RECT& rect2) const throw()
	{ x_rect rect; ::IntersectRect(&rect, this, &rect2);
	return rect; }
	inline x_rect x_rect::operator|(const RECT& rect2) const throw()
	{ x_rect rect; ::UnionRect(&rect, this, &rect2);
	return rect; }
	inline BOOL x_rect::SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw()
	{ return ::SubtractRect(this, lpRectSrc1, lpRectSrc2); }

	inline void x_rect::NormalizeRect() throw()
	{
		int nTemp;
		if (left > right)
		{
			nTemp = left;
			left = right;
			right = nTemp;
		}
		if (top > bottom)
		{
			nTemp = top;
			top = bottom;
			bottom = nTemp;
		}
	}

	inline void x_rect::InflateRect(LPCRECT lpRect) throw()
	{
		left -= lpRect->left;		top -= lpRect->top;
		right += lpRect->right;		bottom += lpRect->bottom;
	}

	inline void x_rect::InflateRect(int l, int t, int r, int b) throw()
	{
		left -= l;			top -= t;
		right += r;			bottom += b;
	}

	inline void x_rect::DeflateRect(LPCRECT lpRect) throw()
	{
		left += lpRect->left;	top += lpRect->top;
		right -= lpRect->right;	bottom -= lpRect->bottom;
	}

	inline void x_rect::DeflateRect(int l, int t, int r, int b) throw()
	{
		left += l;		top += t;
		right -= r;		bottom -= b;
	}

	inline x_rect x_rect::MulDiv(int nMultiplier, int nDivisor) const throw()
	{
		return x_rect(
			::MulDiv(left, nMultiplier, nDivisor),
			::MulDiv(top, nMultiplier, nDivisor),
			::MulDiv(right, nMultiplier, nDivisor),
			::MulDiv(bottom, nMultiplier, nDivisor));
	}
}
#endif // _DATEHELPER_
