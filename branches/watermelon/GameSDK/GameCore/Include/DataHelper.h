#pragma once
#ifndef _DATEHELPER_
#define _DATEHELPER_
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file
//#ifdef CORE_EXPORTS
//#define CORE_API __declspec(dllexport)
//#else
//#define CORE_API __declspec(dllimport)
//#endif	//CORE_EXPORTS

namespace XGC
{
	template< class type > class xgcSize;
	template< class type > class xgcPoint;
	template< class type > class xgcRect;

	/////////////////////////////////////////////////////////////////////////////
	// xgcSize<type> - An extent, similar to Windows xgcSize<type> structure.
	template< class type >
	class xgcSize
	{
	public:
		type cx;
		type cy;

		// Constructors
		// construct an uninitialized size
		xgcSize<type>() throw();
		// create from two integers
		xgcSize<type>(type initCX, type initCY) throw();
		// create from another size
		xgcSize<type>(const xgcSize<type> &initSize) throw();
		// create from a point
		xgcSize<type>(const xgcPoint<type> &initPt) throw();

		// Operations
		bool operator==(xgcSize<type> size) const throw();
		bool operator!=(xgcSize<type> size) const throw();
		void operator+=(xgcSize<type> size) throw();
		void operator-=(xgcSize<type> size) throw();
		void SetSize(type cx, type cy) throw();

		// Operators returning xgcSize<type> values
		xgcSize<type> operator+(xgcSize<type> size) const throw();
		xgcSize<type> operator-(xgcSize<type> size) const throw();
		xgcSize<type> operator-() const throw();

		// Operators returning xgcPoint<type> values
		xgcPoint<type> operator+(xgcPoint<type> point) const throw();
		xgcPoint<type> operator-(xgcPoint<type> point) const throw();

		// Operators returning xgcRect<type> values
		xgcRect<type> operator+(const xgcRect<type>* lpRect) const throw();
		xgcRect<type> operator-(const xgcRect<type>* lpRect) const throw();
	};

	/////////////////////////////////////////////////////////////////////////////
	// xgcPoint<type> - A 2-D point, similar to Windows xgcPoint<type> structure.
	template< class type >
	class xgcPoint
	{
	public:
		// Constructors
		type x, y;
		// create an uninitialized point
		xgcPoint<type>() throw();
		// create from two integers
		xgcPoint<type>(type initX, type initY) throw();
		// create from another point
		xgcPoint<type>(const xgcPoint<type> &initPt) throw();
		// create from a size
		xgcPoint<type>(const xgcSize<type> &initSize) throw();

		// Operations

		// translate the point
		void Offset(type xOffset, type yOffset) throw();
		void Offset(xgcPoint<type> point) throw();
		void Offset(xgcSize<type> size) throw();
		void SetPoint(type X, type Y) throw();

		bool operator==(xgcPoint<type> point) const throw();
		bool operator!=(xgcPoint<type> point) const throw();
		void operator+=(xgcSize<type> size) throw();
		void operator-=(xgcSize<type> size) throw();
		void operator+=(xgcPoint<type> point) throw();
		void operator-=(xgcPoint<type> point) throw();

		// Operators returning xgcPoint<type> values
		xgcPoint<type> operator+(xgcSize<type> size) const throw();
		xgcPoint<type> operator-(xgcSize<type> size) const throw();
		xgcPoint<type> operator-() const throw();
		xgcPoint<type> operator+(xgcPoint<type> point) const throw();

		// Operators returning xgcSize<type> values
		xgcSize<type> operator-(xgcPoint<type> point) const throw();

		// Operators returning xgcRect<type> values
		xgcRect<type> operator+(const xgcRect<type>* lpRect) const throw();
		xgcRect<type> operator-(const xgcRect<type>* lpRect) const throw();
	};

	/////////////////////////////////////////////////////////////////////////////
	// xgcRect<type> - A 2-D rectangle, similar to Windows xgcRect<type> structure.
	template< class type > 
	class xgcRect
	{
		// Constructors
	public:
		type    left;
		type    top;
		type    right;
		type    bottom;

		// uninitialized rectangle
		xgcRect<type>() throw();
		// from left, top, right, and bottom
		xgcRect<type>(type l, type t, type r, type b) throw();
		// copy constructor
		xgcRect<type>(const xgcRect<type>& srcRect) throw();
		// from a pointer to another rect
		xgcRect<type>(const xgcRect<type>* lpSrcRect) throw();
		// from a point and size
		xgcRect<type>(xgcPoint<type> point, xgcSize<type> size) throw();
		// from two points
		xgcRect<type>(xgcPoint<type> topLeft, xgcPoint<type> bottomRight) throw();

		// Attributes (in addition to xgcRect<type> members)

		// retrieves the width
		type Width() const throw();
		// returns the height
		type Height() const throw();
		// returns the size
		xgcSize<type> Size() const throw();
		// reference to the top-left point
		xgcPoint<type>& TopLeft() throw();
		// reference to the bottom-right point
		xgcPoint<type>& BottomRight() throw();
		// const reference to the top-left point
		const xgcPoint<type>& TopLeft() const throw();
		// const reference to the bottom-right point
		const xgcPoint<type>& BottomRight() const throw();
		// the geometric center point of the rectangle
		xgcPoint<type> CenterPoint() const throw();
		// swap the left and right
		void SwapLeftRight() throw();

		// returns true if rectangle has no area
		bool IsRectEmpty() const throw();
		// returns true if rectangle is at (0,0) and has no area
		bool IsRectNull() const throw();
		// returns true if point is within rectangle
		bool PtInRect(xgcPoint<type> point) const throw();

		// Operations

		// set rectangle from left, top, right, and bottom
		void SetRect(type x1, type y1, type x2, type y2) throw();
		void SetRect(xgcPoint<type> topLeft, xgcPoint<type> bottomRight) throw();
		// empty the rectangle
		void SetRectEmpty() throw();
		// copy from another rectangle
		void CopyRect(const xgcRect<type>* lpSrcRect) throw();
		// true if exactly the same as another rectangle
		bool EqualRect(const xgcRect<type>* lpRect) const throw();

		// Inflate rectangle's width and height by
		// x units to the left and right ends of the rectangle
		// and y units to the top and bottom.
		void InflateRect(type x, type y) throw();
		// Inflate rectangle's width and height by
		// size.cx units to the left and right ends of the rectangle
		// and size.cy units to the top and bottom.
		void InflateRect(xgcSize<type> size) throw();
		// Inflate rectangle's width and height by moving individual sides.
		// Left side is moved to the left, right side is moved to the right,
		// top is moved up and bottom is moved down.
		void InflateRect(const xgcRect<type>* lpRect) throw();
		void InflateRect(type l, type t, type r, type b) throw();

		// deflate the rectangle's width and height without
		// moving its top or left
		void DeflateRect(type x, type y) throw();
		void DeflateRect(xgcSize<type> size) throw();
		void DeflateRect(const xgcRect<type> *lpRect) throw();
		void DeflateRect(type l, type t, type r, type b) throw();

		// translate the rectangle by moving its top and left
		void OffsetRect(type x, type y) throw();
		void OffsetRect(xgcSize<type> size) throw();
		void OffsetRect(xgcPoint<type> point) throw();
		void NormalizeRect() throw();

		// absolute position of rectangle
		void MoveToY(type y) throw();
		void MoveToX(type x) throw();
		void MoveToXY(type x, type y) throw();
		void MoveToXY(xgcPoint<type> point) throw();

		// set this rectangle to intersection of two others
		bool IntersectRect(const xgcRect<type>* lpRect1, const xgcRect<type>* lpRect2) throw();

		// set this rectangle to bounding union of two others
		bool UnionRect(const xgcRect<type>* lpRect1, const xgcRect<type>* lpRect2) throw();

		//// set this rectangle to minimum of two others
		//bool SubtractRect(xgcRect<type>* lpRectSrc1, xgcRect<type>* lpRectSrc2) throw();

		// Additional Operations
		void operator=(const xgcRect<type>& srcRect) throw();
		bool operator==(const xgcRect<type>& rect) const throw();
		bool operator!=(const xgcRect<type>& rect) const throw();
		void operator+=(xgcPoint<type> point) throw();
		void operator+=(xgcSize<type> size) throw();
		void operator+=(xgcRect<type>* lpRect) throw();
		void operator-=(xgcPoint<type> point) throw();
		void operator-=(xgcSize<type> size) throw();
		void operator-=(xgcRect<type>* lpRect) throw();
		void operator&=(const xgcRect<type>& rect) throw();
		void operator|=(const xgcRect<type>& rect) throw();

		// Operators returning xgcRect<type> values
		xgcRect<type> operator+(xgcPoint<type> point) const throw();
		xgcRect<type> operator-(xgcPoint<type> point) const throw();
		xgcRect<type> operator+(xgcRect<type>* lpRect) const throw();
		xgcRect<type> operator+(xgcSize<type> size) const throw();
		xgcRect<type> operator-(xgcSize<type> size) const throw();
		xgcRect<type> operator-(xgcRect<type>* lpRect) const throw();
		xgcRect<type> operator&(const xgcRect<type>& rect2) const throw();
		xgcRect<type> operator|(const xgcRect<type>& rect2) const throw();
		xgcRect<type> MulDiv(type nMultiplier, type nDivisor) const throw();
	};
	template< class type > 
	inline xgcSize<type>::xgcSize() throw()
	{ /* random filled */ }

	template< class type > 
	inline xgcSize<type>::xgcSize(type initCX, type initCY) throw()
	{ cx = initCX; cy = initCY; }

	template< class type > 
	inline xgcSize<type>::xgcSize(const xgcSize<type> &initSize) throw()
	{ *(xgcSize<type>*)this = initSize; }

	template< class type > 
	inline xgcSize<type>::xgcSize(const xgcPoint<type> &initPt) throw()
	{ *(xgcPoint<type>*)this = initPt; }

	template< class type > 
	inline bool xgcSize<type>::operator==(xgcSize<type> size) const throw()
	{ return (cx == size.cx && cy == size.cy); }

	template< class type > 
	inline bool xgcSize<type>::operator!=(xgcSize<type> size) const throw()
	{ return (cx != size.cx || cy != size.cy); }

	template< class type > 
	inline void xgcSize<type>::operator+=(xgcSize<type> size) throw()
	{ cx += size.cx; cy += size.cy; }

	template< class type > 
	inline void xgcSize<type>::operator-=(xgcSize<type> size) throw()
	{ cx -= size.cx; cy -= size.cy; }

	template< class type > 
	inline void xgcSize<type>::SetSize(type CX, type CY) throw()
	{ cx = CX; cy = CY; }	

	template< class type > 
	inline xgcSize<type> xgcSize<type>::operator+(xgcSize<type> size) const throw()
	{ return xgcSize<type>(cx + size.cx, cy + size.cy); }

	template< class type > 
	inline xgcSize<type> xgcSize<type>::operator-(xgcSize<type> size) const throw()
	{ return xgcSize<type>(cx - size.cx, cy - size.cy); }

	template< class type > 
	inline xgcSize<type> xgcSize<type>::operator-() const throw()
	{ return xgcSize<type>(-cx, -cy); }

	template< class type > 
	inline xgcPoint<type> xgcSize<type>::operator+(xgcPoint<type> point) const throw()
	{ return xgcPoint<type>(cx + point.x, cy + point.y); }

	template< class type > 
	inline xgcPoint<type> xgcSize<type>::operator-(xgcPoint<type> point) const throw()
	{ return xgcPoint<type>(cx - point.x, cy - point.y); }

	template< class type > 
	inline xgcRect<type> xgcSize<type>::operator+(const xgcRect<type>* lpRect) const throw()
	{ return xgcRect<type>(lpRect) + *this; }

	template< class type > 
	inline xgcRect<type> xgcSize<type>::operator-(const xgcRect<type>* lpRect) const throw()
	{ return xgcRect<type>(lpRect) - *this; }


	// xgcPoint<type>
	template< class type > 
	inline xgcPoint<type>::xgcPoint() throw()
	{ /* random filled */ }

	template< class type > 
	inline xgcPoint<type>::xgcPoint(type initX, type initY) throw()
	{ x = initX; y = initY; }

	template< class type > 
	inline xgcPoint<type>::xgcPoint(const xgcPoint<type> &initPt) throw()
	{ *(xgcPoint<type>*)this = initPt; }

	template< class type > 
	inline xgcPoint<type>::xgcPoint(const xgcSize<type> &initSize) throw()
	{ *(xgcSize<type>*)this = initSize; }

	template< class type > 
	inline void xgcPoint<type>::Offset(type xOffset, type yOffset) throw()
	{ x += xOffset; y += yOffset; }

	template< class type > 
	inline void xgcPoint<type>::Offset(xgcPoint<type> point) throw()
	{ x += point.x; y += point.y; }

	template< class type > 
	inline void xgcPoint<type>::Offset(xgcSize<type> size) throw()
	{ x += size.cx; y += size.cy; }

	template< class type > 
	inline void xgcPoint<type>::SetPoint(type X, type Y) throw()
	{ x = X; y = Y; }

	template< class type > 
	inline bool xgcPoint<type>::operator==(xgcPoint<type> point) const throw()
	{ return (x == point.x && y == point.y); }

	template< class type > 
	inline bool xgcPoint<type>::operator!=(xgcPoint<type> point) const throw()
	{ return (x != point.x || y != point.y); }

	template< class type > 
	inline void xgcPoint<type>::operator+=(xgcSize<type> size) throw()
	{ x += size.cx; y += size.cy; }

	template< class type > 
	inline void xgcPoint<type>::operator-=(xgcSize<type> size) throw()
	{ x -= size.cx; y -= size.cy; }

	template< class type > 
	inline void xgcPoint<type>::operator+=(xgcPoint<type> point) throw()
	{ x += point.x; y += point.y; }

	template< class type > 
	inline void xgcPoint<type>::operator-=(xgcPoint<type> point) throw()
	{ x -= point.x; y -= point.y; }

	template< class type > 
	inline xgcPoint<type> xgcPoint<type>::operator+(xgcSize<type> size) const throw()
	{ return xgcPoint<type>(x + size.cx, y + size.cy); }

	template< class type > 
	inline xgcPoint<type> xgcPoint<type>::operator-(xgcSize<type> size) const throw()
	{ return xgcPoint<type>(x - size.cx, y - size.cy); }

	template< class type > 
	inline xgcPoint<type> xgcPoint<type>::operator-() const throw()
	{ return xgcPoint<type>(-x, -y); }

	template< class type > 
	inline xgcPoint<type> xgcPoint<type>::operator+(xgcPoint<type> point) const throw()
	{ return xgcPoint<type>(x + point.x, y + point.y); }

	template< class type > 
	inline xgcSize<type> xgcPoint<type>::operator-(xgcPoint<type> point) const throw()
	{ return xgcSize<type>(x - point.x, y - point.y); }

	template< class type > 
	inline xgcRect<type> xgcPoint<type>::operator+(const xgcRect<type>* lpRect) const throw()
	{ return xgcRect<type>(lpRect) + *this; }

	template< class type > 
	inline xgcRect<type> xgcPoint<type>::operator-(const xgcRect<type>* lpRect) const throw()
	{ return xgcRect<type>(lpRect) - *this; }

	// xgcRect<type>
	template< class type > 
	inline xgcRect<type>::xgcRect() throw()
	{ /* random filled */ }

	template< class type > 
	inline xgcRect<type>::xgcRect(type l, type t, type r, type b) throw()
	{ left = l; top = t; right = r; bottom = b; }

	template< class type > 
	inline xgcRect<type>::xgcRect(const xgcRect<type>& srcRect) throw()
	{ CopyRect( &srcRect ); }

	template< class type > 
	inline xgcRect<type>::xgcRect(const xgcRect<type>* lpSrcRect) throw()
	{ CopyRect( lpSrcRect ); }

	template< class type > 
	inline xgcRect<type>::xgcRect(xgcPoint<type> point, xgcSize<type> size) throw()
	{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }

	template< class type > 
	inline xgcRect<type>::xgcRect(xgcPoint<type> topLeft, xgcPoint<type> bottomRight) throw()
	{ left = topLeft.x; top = topLeft.y;
	right = bottomRight.x; bottom = bottomRight.y; }

	template< class type > 
	inline type xgcRect<type>::Width() const throw()
	{ return right - left; }

	template< class type > 
	inline type xgcRect<type>::Height() const throw()
	{ return bottom - top; }

	template< class type > 
	inline xgcSize<type> xgcRect<type>::Size() const throw()
	{ return xgcSize<type>(right - left, bottom - top); }

	template< class type > 
	inline xgcPoint<type>& xgcRect<type>::TopLeft() throw()
	{ return *((xgcPoint<type>*)this); }

	template< class type > 
	inline xgcPoint<type>& xgcRect<type>::BottomRight() throw()
	{ return *((xgcPoint<type>*)this+1); }

	template< class type > 
	inline const xgcPoint<type>& xgcRect<type>::TopLeft() const throw()
	{ return *((xgcPoint<type>*)this); }

	template< class type > 
	inline const xgcPoint<type>& xgcRect<type>::BottomRight() const throw()
	{ return *((xgcPoint<type>*)this+1); }

	template< class type > 
	inline xgcPoint<type> xgcRect<type>::CenterPoint() const throw()
	{ return xgcPoint<type>((left+right)/2, (top+bottom)/2); }

	template< class type > 
	inline void xgcRect<type>::SwapLeftRight() throw()
	{ type temp = left; left = right; right = temp; }

	template< class type > 
	inline bool xgcRect<type>::IsRectEmpty() const throw()
	{ return (left == right && top == bottom); }

	template< class type > 
	inline bool xgcRect<type>::IsRectNull() const throw()
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }

	template< class type > 
	inline bool xgcRect<type>::PtInRect(xgcPoint<type> point) const throw()
	{ return (point.x > left && point.x < right && point.y > top && point.y < bottom); }

	template< class type > 
	inline void xgcRect<type>::SetRect(type x1, type y1, type x2, type y2) throw()
	{ 
		left	= x1;
		top		= y1;
		right	= x2;
		bottom	= y2;
	}

	template< class type > 
	inline void xgcRect<type>::SetRect(xgcPoint<type> topLeft, xgcPoint<type> bottomRight) throw()
	{ 
		left	= topLeft.x;
		top		= topLeft.y;
		right	= bottomRight.x;
		bottom	= bottomRight.y;
 	}

	template< class type > 
	inline void xgcRect<type>::SetRectEmpty() throw()
	{ left = right = top = bottom = 0; }

	template< class type > 
	inline void xgcRect<type>::CopyRect(const xgcRect<type>* lpSrcRect) throw()
	{ 
		left	= lpSrcRect->left; 
		top		= lpSrcRect->top;
		right	= lpSrcRect->right;
		bottom	= lpSrcRect->bottom;
	}

	template< class type > 
	inline bool xgcRect<type>::EqualRect(const xgcRect<type>* lpRect) const throw()
	{ return left == lpRect->left && right == lpRect->right && top == lpRect->top && bottom == lpRect->bottom; }

	template< class type > 
	inline void xgcRect<type>::InflateRect(type x, type y) throw()
	{ left -= x;right += x; top -= y; bottom += y;}

	template< class type > 
	inline void xgcRect<type>::InflateRect(xgcSize<type> size) throw()
	{ InflateRect(size.cx, size.cy); }

	template< class type > 
	inline void xgcRect<type>::DeflateRect(type x, type y) throw()
	{ InflateRect(-x, -y); }

	template< class type > 
	inline void xgcRect<type>::DeflateRect(xgcSize<type> size) throw()
	{ InflateRect(-size.cx, -size.cy); }

	template< class type > 
	inline void xgcRect<type>::OffsetRect(type x, type y) throw()
	{ left += x;right += x;top += y;bottom += y; }

	template< class type > 
	inline void xgcRect<type>::OffsetRect(xgcPoint<type> point) throw()
	{ OffsetRect(point.x, point.y); }

	template< class type > 
	inline void xgcRect<type>::OffsetRect(xgcSize<type> size) throw()
	{ OffsetRect(size.cx, size.cy); }

	template< class type > 
	inline void xgcRect<type>::MoveToY(type y) throw()
	{ bottom = Height() + y; top = y; }

	template< class type > 
	inline void xgcRect<type>::MoveToX(type x) throw()
	{ right = Width() + x; left = x; }

	template< class type > 
	inline void xgcRect<type>::MoveToXY(type x, type y) throw()
	{ MoveToX(x); MoveToY(y); }

	template< class type > 
	inline void xgcRect<type>::MoveToXY(xgcPoint<type> pt) throw()
	{ MoveToX(pt.x); MoveToY(pt.y); }

	template< class type > 
	inline bool xgcRect<type>::IntersectRect(const xgcRect<type>* lpRect1, const xgcRect<type>* lpRect2) throw()
	{ 
		left	= __max( lpRect1->left, lpRect2->left );
		right	= __min( lpRect1->right, lpRect2->right );
		top		= __max( lpRect1->top, lpRect2->top );
		bottom	= __min( lpRect1->bottom, lpRect2->bottom );

		return ( left > right || top > bottom );
	}

	template< class type > 
	inline bool xgcRect<type>::UnionRect(const xgcRect<type>* lpRect1, const xgcRect<type>* lpRect2) throw()
	{ 
		left	= __min( lpRect1->left, lpRect2->left );
		right	= __max( lpRect1->right, lpRect2->right );
		top		= __min( lpRect1->top, lpRect2->top );
		bottom	= __max( lpRect1->bottom, lpRect2->bottom );

		return ( left < right && top < bottom );
	}

	template< class type > 
	inline void xgcRect<type>::operator=(const xgcRect<type>& srcRect) throw()
	{ CopyRect(&srcRect); }

	template< class type > 
	inline bool xgcRect<type>::operator==(const xgcRect<type>& rect) const throw()
	{ return EqualRect(&rect); }

	template< class type > 
	inline bool xgcRect<type>::operator!=(const xgcRect<type>& rect) const throw()
	{ return !EqualRect(&rect); }

	template< class type > 
	inline void xgcRect<type>::operator+=(xgcPoint<type> point) throw()
	{ OffsetRect(point.x, point.y); }

	template< class type > 
	inline void xgcRect<type>::operator+=(xgcSize<type> size) throw()
	{ OffsetRect(size.cx, size.cy); }

	template< class type > 
	inline void xgcRect<type>::operator+=(xgcRect<type>* lpRect) throw()
	{ InflateRect(lpRect); }

	template< class type > 
	inline void xgcRect<type>::operator-=(xgcPoint<type> point) throw()
	{ OffsetRect(-point.x, -point.y); }

	template< class type > 
	inline void xgcRect<type>::operator-=(xgcSize<type> size) throw()
	{ OffsetRect(-size.cx, -size.cy); }

	template< class type > 
	inline void xgcRect<type>::operator-=(xgcRect<type>* lpRect) throw()
	{ DeflateRect(lpRect); }

	template< class type > 
	inline void xgcRect<type>::operator&=(const xgcRect<type>& rect) throw()
	{ IntersectRect(this, &rect); }

	template< class type > 
	inline void xgcRect<type>::operator|=(const xgcRect<type>& rect) throw()
	{ UnionRect(this, &rect); }

	template< class type > 
	inline xgcRect<type> xgcRect<type>::operator+(xgcPoint<type> pt) const throw()
	{ xgcRect<type> rect(*this); rect.OffsetRect(pt.x, pt.y); return rect; }

	template< class type > 
	inline xgcRect<type> xgcRect<type>::operator-(xgcPoint<type> pt) const throw()
	{ xgcRect<type> rect(*this); rect.OffsetRect(-pt.x, -pt.y); return rect; }

	template< class type > 
	inline xgcRect<type> xgcRect<type>::operator+(xgcSize<type> size) const throw()
	{ xgcRect<type> rect(*this); rect.OffsetRect(size.cx, size.cy); return rect; }

	template< class type > 
	inline xgcRect<type> xgcRect<type>::operator-(xgcSize<type> size) const throw()
	{ xgcRect<type> rect(*this); rect.OffsetRect(-size.cx, -size.cy); return rect; }

	template< class type > 
	inline xgcRect<type> xgcRect<type>::operator+(xgcRect<type>* lpRect) const throw()
	{ xgcRect<type> rect(this); rect.InflateRect(lpRect); return rect; }

	template< class type > 
	inline xgcRect<type> xgcRect<type>::operator-(xgcRect<type>* lpRect) const throw()
	{ xgcRect<type> rect(this); rect.DeflateRect(lpRect); return rect; }

	template< class type > 
	inline xgcRect<type> xgcRect<type>::operator&(const xgcRect<type>& rect2) const throw()
	{ xgcRect<type> rect; rect.IntersectRect(this, &rect2); return rect; }

	template< class type > 
	inline xgcRect<type> xgcRect<type>::operator|(const xgcRect<type>& rect2) const throw()
	{ xgcRect<type> rect; rect.UnionRect(this, &rect2);return rect; }

	template< class type > 
	inline void xgcRect<type>::NormalizeRect() throw()
	{
		type nTemp;
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

	template< class type > 
	inline void xgcRect<type>::InflateRect(const xgcRect<type>* lpRect) throw()
	{
		left -= lpRect->left;		top -= lpRect->top;
		right += lpRect->right;		bottom += lpRect->bottom;
	}

	template< class type > 
	inline void xgcRect<type>::InflateRect(type l, type t, type r, type b) throw()
	{
		left -= l;			top -= t;
		right += r;			bottom += b;
	}

	template< class type > 
	inline void xgcRect<type>::DeflateRect(const xgcRect<type>* lpRect) throw()
	{
		left += lpRect->left;	top += lpRect->top;
		right -= lpRect->right;	bottom -= lpRect->bottom;
	}

	template< class type > 
	inline void xgcRect<type>::DeflateRect(type l, type t, type r, type b) throw()
	{
		left += l;		top += t;
		right -= r;		bottom -= b;
	}

	typedef xgcSize< int >	iSize;
	typedef xgcPoint< int > iPoint;
	typedef xgcRect< int >	iRect;

	typedef xgcSize< int >	fSize;
	typedef xgcPoint< int > fPoint;
	typedef xgcRect< int >	fRect;
}
#endif // _DATEHELPER_
