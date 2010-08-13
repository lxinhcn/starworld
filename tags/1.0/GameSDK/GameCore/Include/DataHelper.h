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
	class xgcSize;
	class xgcPoint;
	class xgcRect;

	/////////////////////////////////////////////////////////////////////////////
	// xgcSize - An extent, similar to Windows SIZE structure.

	class CORE_API xgcSize : public tagSIZE
	{
	public:

		// Constructors
		// construct an uninitialized size
		xgcSize() throw();
		// create from two integers
		xgcSize(int initCX, int initCY) throw();
		// create from another size
		xgcSize(SIZE initSize) throw();
		// create from a point
		xgcSize(POINT initPt) throw();
		// create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
		xgcSize(DWORD dwSize) throw();

		// Operations
		BOOL operator==(SIZE size) const throw();
		BOOL operator!=(SIZE size) const throw();
		void operator+=(SIZE size) throw();
		void operator-=(SIZE size) throw();
		void SetSize(int CX, int CY) throw();

		// Operators returning xgcSize values
		xgcSize operator+(SIZE size) const throw();
		xgcSize operator-(SIZE size) const throw();
		xgcSize operator-() const throw();

		// Operators returning xgcPoint values
		xgcPoint operator+(POINT point) const throw();
		xgcPoint operator-(POINT point) const throw();

		// Operators returning xgcRect values
		xgcRect operator+(const RECT* lpRect) const throw();
		xgcRect operator-(const RECT* lpRect) const throw();
	};

	/////////////////////////////////////////////////////////////////////////////
	// xgcPoint - A 2-D point, similar to Windows POINT structure.

	class CORE_API xgcPoint : public tagPOINT
	{
	public:
		// Constructors

		// create an uninitialized point
		xgcPoint() throw();
		// create from two integers
		xgcPoint(int initX, int initY) throw();
		// create from another point
		xgcPoint(POINT initPt) throw();
		// create from a size
		xgcPoint(SIZE initSize) throw();
		// create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
		xgcPoint(LPARAM dwPoint) throw();


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

		// Operators returning xgcPoint values
		xgcPoint operator+(SIZE size) const throw();
		xgcPoint operator-(SIZE size) const throw();
		xgcPoint operator-() const throw();
		xgcPoint operator+(POINT point) const throw();

		// Operators returning xgcSize values
		xgcSize operator-(POINT point) const throw();

		// Operators returning xgcRect values
		xgcRect operator+(const RECT* lpRect) const throw();
		xgcRect operator-(const RECT* lpRect) const throw();
	};

	/////////////////////////////////////////////////////////////////////////////
	// xgcRect - A 2-D rectangle, similar to Windows RECT structure.

	class CORE_API xgcRect : public tagRECT
	{
		// Constructors
	public:
		// uninitialized rectangle
		xgcRect() throw();
		// from left, top, right, and bottom
		xgcRect(int l, int t, int r, int b) throw();
		// copy constructor
		xgcRect(const RECT& srcRect) throw();
		// from a pointer to another rect
		xgcRect(LPCRECT lpSrcRect) throw();
		// from a point and size
		xgcRect(POINT point, SIZE size) throw();
		// from two points
		xgcRect(POINT topLeft, POINT bottomRight) throw();

		// Attributes (in addition to RECT members)

		// retrieves the width
		int Width() const throw();
		// returns the height
		int Height() const throw();
		// returns the size
		xgcSize Size() const throw();
		// reference to the top-left point
		xgcPoint& TopLeft() throw();
		// reference to the bottom-right point
		xgcPoint& BottomRight() throw();
		// const reference to the top-left point
		const xgcPoint& TopLeft() const throw();
		// const reference to the bottom-right point
		const xgcPoint& BottomRight() const throw();
		// the geometric center point of the rectangle
		xgcPoint CenterPoint() const throw();
		// swap the left and right
		void SwapLeftRight() throw();
		static void WINAPI SwapLeftRight(LPRECT lpRect) throw();

		// convert between xgcRect and LPRECT/LPCRECT (no need for &)
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

		// Operators returning xgcRect values
		xgcRect operator+(POINT point) const throw();
		xgcRect operator-(POINT point) const throw();
		xgcRect operator+(LPCRECT lpRect) const throw();
		xgcRect operator+(SIZE size) const throw();
		xgcRect operator-(SIZE size) const throw();
		xgcRect operator-(LPCRECT lpRect) const throw();
		xgcRect operator&(const RECT& rect2) const throw();
		xgcRect operator|(const RECT& rect2) const throw();
		xgcRect MulDiv(int nMultiplier, int nDivisor) const throw();
	};
	inline xgcSize::xgcSize() throw()
	{ /* random filled */ }
	inline xgcSize::xgcSize(int initCX, int initCY) throw()
	{ cx = initCX; cy = initCY; }
	inline xgcSize::xgcSize(SIZE initSize) throw()
	{ *(SIZE*)this = initSize; }
	inline xgcSize::xgcSize(POINT initPt) throw()
	{ *(POINT*)this = initPt; }
	inline xgcSize::xgcSize(DWORD dwSize) throw()
	{
		cx = (short)LOWORD(dwSize);
		cy = (short)HIWORD(dwSize);
	}
	inline BOOL xgcSize::operator==(SIZE size) const throw()
	{ return (cx == size.cx && cy == size.cy); }
	inline BOOL xgcSize::operator!=(SIZE size) const throw()
	{ return (cx != size.cx || cy != size.cy); }
	inline void xgcSize::operator+=(SIZE size) throw()
	{ cx += size.cx; cy += size.cy; }
	inline void xgcSize::operator-=(SIZE size) throw()
	{ cx -= size.cx; cy -= size.cy; }
	inline void xgcSize::SetSize(int CX, int CY) throw()
	{ cx = CX; cy = CY; }	
	inline xgcSize xgcSize::operator+(SIZE size) const throw()
	{ return xgcSize(cx + size.cx, cy + size.cy); }
	inline xgcSize xgcSize::operator-(SIZE size) const throw()
	{ return xgcSize(cx - size.cx, cy - size.cy); }
	inline xgcSize xgcSize::operator-() const throw()
	{ return xgcSize(-cx, -cy); }
	inline xgcPoint xgcSize::operator+(POINT point) const throw()
	{ return xgcPoint(cx + point.x, cy + point.y); }
	inline xgcPoint xgcSize::operator-(POINT point) const throw()
	{ return xgcPoint(cx - point.x, cy - point.y); }
	inline xgcRect xgcSize::operator+(const RECT* lpRect) const throw()
	{ return xgcRect(lpRect) + *this; }
	inline xgcRect xgcSize::operator-(const RECT* lpRect) const throw()
	{ return xgcRect(lpRect) - *this; }

	// xgcPoint
	inline xgcPoint::xgcPoint() throw()
	{ /* random filled */ }
	inline xgcPoint::xgcPoint(int initX, int initY) throw()
	{ x = initX; y = initY; }
	inline xgcPoint::xgcPoint(POINT initPt) throw()
	{ *(POINT*)this = initPt; }
	inline xgcPoint::xgcPoint(SIZE initSize) throw()
	{ *(SIZE*)this = initSize; }
	inline xgcPoint::xgcPoint(LPARAM dwPoint) throw()
	{
		x = (short)LOWORD(dwPoint);
		y = (short)HIWORD(dwPoint);
	}
	inline void xgcPoint::Offset(int xOffset, int yOffset) throw()
	{ x += xOffset; y += yOffset; }
	inline void xgcPoint::Offset(POINT point) throw()
	{ x += point.x; y += point.y; }
	inline void xgcPoint::Offset(SIZE size) throw()
	{ x += size.cx; y += size.cy; }
	inline void xgcPoint::SetPoint(int X, int Y) throw()
	{ x = X; y = Y; }
	inline BOOL xgcPoint::operator==(POINT point) const throw()
	{ return (x == point.x && y == point.y); }
	inline BOOL xgcPoint::operator!=(POINT point) const throw()
	{ return (x != point.x || y != point.y); }
	inline void xgcPoint::operator+=(SIZE size) throw()
	{ x += size.cx; y += size.cy; }
	inline void xgcPoint::operator-=(SIZE size) throw()
	{ x -= size.cx; y -= size.cy; }
	inline void xgcPoint::operator+=(POINT point) throw()
	{ x += point.x; y += point.y; }
	inline void xgcPoint::operator-=(POINT point) throw()
	{ x -= point.x; y -= point.y; }
	inline xgcPoint xgcPoint::operator+(SIZE size) const throw()
	{ return xgcPoint(x + size.cx, y + size.cy); }
	inline xgcPoint xgcPoint::operator-(SIZE size) const throw()
	{ return xgcPoint(x - size.cx, y - size.cy); }
	inline xgcPoint xgcPoint::operator-() const throw()
	{ return xgcPoint(-x, -y); }
	inline xgcPoint xgcPoint::operator+(POINT point) const throw()
	{ return xgcPoint(x + point.x, y + point.y); }
	inline xgcSize xgcPoint::operator-(POINT point) const throw()
	{ return xgcSize(x - point.x, y - point.y); }
	inline xgcRect xgcPoint::operator+(const RECT* lpRect) const throw()
	{ return xgcRect(lpRect) + *this; }
	inline xgcRect xgcPoint::operator-(const RECT* lpRect) const throw()
	{ return xgcRect(lpRect) - *this; }

	// xgcRect
	inline xgcRect::xgcRect() throw()
	{ /* random filled */ }
	inline xgcRect::xgcRect(int l, int t, int r, int b) throw()
	{ left = l; top = t; right = r; bottom = b; }
	inline xgcRect::xgcRect(const RECT& srcRect) throw()
	{ ::CopyRect(this, &srcRect); }
	inline xgcRect::xgcRect(LPCRECT lpSrcRect) throw()
	{ ::CopyRect(this, lpSrcRect); }
	inline xgcRect::xgcRect(POINT point, SIZE size) throw()
	{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
	inline xgcRect::xgcRect(POINT topLeft, POINT bottomRight) throw()
	{ left = topLeft.x; top = topLeft.y;
	right = bottomRight.x; bottom = bottomRight.y; }
	inline int xgcRect::Width() const throw()
	{ return right - left; }
	inline int xgcRect::Height() const throw()
	{ return bottom - top; }
	inline xgcSize xgcRect::Size() const throw()
	{ return xgcSize(right - left, bottom - top); }
	inline xgcPoint& xgcRect::TopLeft() throw()
	{ return *((xgcPoint*)this); }
	inline xgcPoint& xgcRect::BottomRight() throw()
	{ return *((xgcPoint*)this+1); }
	inline const xgcPoint& xgcRect::TopLeft() const throw()
	{ return *((xgcPoint*)this); }
	inline const xgcPoint& xgcRect::BottomRight() const throw()
	{ return *((xgcPoint*)this+1); }
	inline xgcPoint xgcRect::CenterPoint() const throw()
	{ return xgcPoint((left+right)/2, (top+bottom)/2); }
	inline void xgcRect::SwapLeftRight() throw()
	{ SwapLeftRight(LPRECT(this)); }
	inline void WINAPI xgcRect::SwapLeftRight(LPRECT lpRect) throw()
	{ LONG temp = lpRect->left; lpRect->left = lpRect->right; lpRect->right = temp; }
	inline xgcRect::operator LPRECT() throw()
	{ return this; }
	inline xgcRect::operator LPCRECT() const throw()
	{ return this; }
	inline BOOL xgcRect::IsRectEmpty() const throw()
	{ return ::IsRectEmpty(this); }
	inline BOOL xgcRect::IsRectNull() const throw()
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
	inline BOOL xgcRect::PtInRect(POINT point) const throw()
	{ return ::PtInRect(this, point); }
	inline void xgcRect::SetRect(int x1, int y1, int x2, int y2) throw()
	{ ::SetRect(this, x1, y1, x2, y2); }
	inline void xgcRect::SetRect(POINT topLeft, POINT bottomRight) throw()
	{ ::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
	inline void xgcRect::SetRectEmpty() throw()
	{ ::SetRectEmpty(this); }
	inline void xgcRect::CopyRect(LPCRECT lpSrcRect) throw()
	{ ::CopyRect(this, lpSrcRect); }
	inline BOOL xgcRect::EqualRect(LPCRECT lpRect) const throw()
	{ return ::EqualRect(this, lpRect); }
	inline void xgcRect::InflateRect(int x, int y) throw()
	{ ::InflateRect(this, x, y); }
	inline void xgcRect::InflateRect(SIZE size) throw()
	{ ::InflateRect(this, size.cx, size.cy); }
	inline void xgcRect::DeflateRect(int x, int y) throw()
	{ ::InflateRect(this, -x, -y); }
	inline void xgcRect::DeflateRect(SIZE size) throw()
	{ ::InflateRect(this, -size.cx, -size.cy); }
	inline void xgcRect::OffsetRect(int x, int y) throw()
	{ ::OffsetRect(this, x, y); }
	inline void xgcRect::OffsetRect(POINT point) throw()
	{ ::OffsetRect(this, point.x, point.y); }
	inline void xgcRect::OffsetRect(SIZE size) throw()
	{ ::OffsetRect(this, size.cx, size.cy); }
	inline void xgcRect::MoveToY(int y) throw()
	{ bottom = Height() + y; top = y; }
	inline void xgcRect::MoveToX(int x) throw()
	{ right = Width() + x; left = x; }
	inline void xgcRect::MoveToXY(int x, int y) throw()
	{ MoveToX(x); MoveToY(y); }
	inline void xgcRect::MoveToXY(POINT pt) throw()
	{ MoveToX(pt.x); MoveToY(pt.y); }
	inline BOOL xgcRect::IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{ return ::IntersectRect(this, lpRect1, lpRect2);}
	inline BOOL xgcRect::UnionRect(LPCRECT lpRect1, LPCRECT lpRect2) throw()
	{ return ::UnionRect(this, lpRect1, lpRect2); }
	inline void xgcRect::operator=(const RECT& srcRect) throw()
	{ ::CopyRect(this, &srcRect); }
	inline BOOL xgcRect::operator==(const RECT& rect) const throw()
	{ return ::EqualRect(this, &rect); }
	inline BOOL xgcRect::operator!=(const RECT& rect) const throw()
	{ return !::EqualRect(this, &rect); }
	inline void xgcRect::operator+=(POINT point) throw()
	{ ::OffsetRect(this, point.x, point.y); }
	inline void xgcRect::operator+=(SIZE size) throw()
	{ ::OffsetRect(this, size.cx, size.cy); }
	inline void xgcRect::operator+=(LPCRECT lpRect) throw()
	{ InflateRect(lpRect); }
	inline void xgcRect::operator-=(POINT point) throw()
	{ ::OffsetRect(this, -point.x, -point.y); }
	inline void xgcRect::operator-=(SIZE size) throw()
	{ ::OffsetRect(this, -size.cx, -size.cy); }
	inline void xgcRect::operator-=(LPCRECT lpRect) throw()
	{ DeflateRect(lpRect); }
	inline void xgcRect::operator&=(const RECT& rect) throw()
	{ ::IntersectRect(this, this, &rect); }
	inline void xgcRect::operator|=(const RECT& rect) throw()
	{ ::UnionRect(this, this, &rect); }
	inline xgcRect xgcRect::operator+(POINT pt) const throw()
	{ xgcRect rect(*this); ::OffsetRect(&rect, pt.x, pt.y); return rect; }
	inline xgcRect xgcRect::operator-(POINT pt) const throw()
	{ xgcRect rect(*this); ::OffsetRect(&rect, -pt.x, -pt.y); return rect; }
	inline xgcRect xgcRect::operator+(SIZE size) const throw()
	{ xgcRect rect(*this); ::OffsetRect(&rect, size.cx, size.cy); return rect; }
	inline xgcRect xgcRect::operator-(SIZE size) const throw()
	{ xgcRect rect(*this); ::OffsetRect(&rect, -size.cx, -size.cy); return rect; }
	inline xgcRect xgcRect::operator+(LPCRECT lpRect) const throw()
	{ xgcRect rect(this); rect.InflateRect(lpRect); return rect; }
	inline xgcRect xgcRect::operator-(LPCRECT lpRect) const throw()
	{ xgcRect rect(this); rect.DeflateRect(lpRect); return rect; }
	inline xgcRect xgcRect::operator&(const RECT& rect2) const throw()
	{ xgcRect rect; ::IntersectRect(&rect, this, &rect2);
	return rect; }
	inline xgcRect xgcRect::operator|(const RECT& rect2) const throw()
	{ xgcRect rect; ::UnionRect(&rect, this, &rect2);
	return rect; }
	inline BOOL xgcRect::SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2) throw()
	{ return ::SubtractRect(this, lpRectSrc1, lpRectSrc2); }

	inline void xgcRect::NormalizeRect() throw()
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

	inline void xgcRect::InflateRect(LPCRECT lpRect) throw()
	{
		left -= lpRect->left;		top -= lpRect->top;
		right += lpRect->right;		bottom += lpRect->bottom;
	}

	inline void xgcRect::InflateRect(int l, int t, int r, int b) throw()
	{
		left -= l;			top -= t;
		right += r;			bottom += b;
	}

	inline void xgcRect::DeflateRect(LPCRECT lpRect) throw()
	{
		left += lpRect->left;	top += lpRect->top;
		right -= lpRect->right;	bottom -= lpRect->bottom;
	}

	inline void xgcRect::DeflateRect(int l, int t, int r, int b) throw()
	{
		left += l;		top += t;
		right -= r;		bottom -= b;
	}

	inline xgcRect xgcRect::MulDiv(int nMultiplier, int nDivisor) const throw()
	{
		return xgcRect(
			::MulDiv(left, nMultiplier, nDivisor),
			::MulDiv(top, nMultiplier, nDivisor),
			::MulDiv(right, nMultiplier, nDivisor),
			::MulDiv(bottom, nMultiplier, nDivisor));
	}
}
#endif // _DATEHELPER_
