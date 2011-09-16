#ifndef _XATTRIBUTE_H
#define _XATTRIBUTE_H

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#ifndef _USE_BOOST
	#include <functional>
	using namespace std;
	using namespace std::tr1;
	using namespace std::tr1::placeholders;
#else
	#include "boost\function.hpp"
	#include "boost\bind.hpp"
	using namespace boost;
#endif

namespace XGC
{
	//////////////////////////////////////////////////////////////////////////
	// 简单变体类型类,仅包含了整型和浮点类型
	//////////////////////////////////////////////////////////////////////////
	class CAttrib;
	class CORE_API CXVariant
	{
		friend class CAttrib;
		DEBUG_CODE( enum { type_unknowe, type_integer, type_float }; )
	private:
		DEBUG_CODE( int t; )

	public:
		union
		{
			float	fValue;
			int		nValue;
		};

		CXVariant()							{ DEBUG_CODE( t = type_unknowe; )	nValue = 0; }
		CXVariant( const CXVariant& rsh )	{ DEBUG_CODE( t = rsh.t; )			fValue = rsh.fValue; }
		CXVariant( float rsh )				{ DEBUG_CODE( t = type_float; )		fValue = rsh; }
		CXVariant( int rsh )				{ DEBUG_CODE( t = type_integer; )	nValue = rsh; }
		CXVariant( short rsh )				{ DEBUG_CODE( t = type_integer; )	nValue = rsh; }
		CXVariant( unsigned int rsh )		{ DEBUG_CODE( t = type_integer; )	nValue = rsh; }
		CXVariant( unsigned short rsh )		{ DEBUG_CODE( t = type_integer; )	nValue = rsh; }

		inline bool IsValid()const { return this != xgc_nullptr; }
		inline bool operator==( const CXVariant& val )const	{ return DEBUG_CODE( t == val.t && ) val.nValue == nValue; }
		inline bool operator==( float val )const			{ return DEBUG_CODE( t == type_float && ) val == fValue; }
		inline bool operator==( int val )const				{ return DEBUG_CODE( t == type_integer && ) val == nValue; }
		inline bool operator==( short val )const			{ return DEBUG_CODE( t == type_integer && ) val == nValue; }
		inline bool operator==( unsigned int val )const		{ return DEBUG_CODE( t == type_integer && ) (int)val == nValue; }
		inline bool operator==( unsigned short val )const	{ return DEBUG_CODE( t == type_integer && ) (int)val == nValue; }

		inline CXVariant& operator=( const CXVariant& val )	{ DEBUG_CODE( t = val.t );			nValue = val.nValue; return *this; }
		inline CXVariant& operator=( float val )			{ DEBUG_CODE( t = type_float );		fValue = val; return *this; }
		inline CXVariant& operator=( int val )				{ DEBUG_CODE( t = type_integer );	nValue = val; return *this; }
		inline CXVariant& operator=( short val )			{ DEBUG_CODE( t = type_integer );	nValue = val; return *this; }
		inline CXVariant& operator=( unsigned int val )		{ DEBUG_CODE( t = type_integer );	nValue = (int)val; return *this; }
		inline CXVariant& operator=( unsigned short val )	{ DEBUG_CODE( t = type_integer );	nValue = (int)val; return *this; }

		inline operator int()const				{ DEBUG_CODE( VERIFY( t == type_integer ) ); return nValue; }
		inline operator short()const			{ DEBUG_CODE( VERIFY( t == type_integer ) ); return nValue; }
		inline operator unsigned int()const		{ DEBUG_CODE( VERIFY( t == type_integer ) ); return nValue; }
		inline operator unsigned short()const	{ DEBUG_CODE( VERIFY( t == type_integer ) ); return nValue; }
		inline operator float()const			{ DEBUG_CODE( VERIFY( t == type_float	) ); return fValue; }
		inline operator double()const			{ DEBUG_CODE( VERIFY( t == type_float	) ); return fValue; }
		inline operator bool()const				{ DEBUG_CODE( VERIFY( t == type_integer ) ); return nValue != 0; }
	};

	//__inline bool operator <( float fValue, const CXVariant& xValue ){ return fValue < xValue.fValue; }
	//__inline bool operator <( int nValue, const CXVariant& xValue ){ return nValue < xValue.nValue; }

	//__inline bool operator <=( float fValue, const CXVariant& xValue ){ return fValue <= xValue.fValue; }
	//__inline bool operator <=( int nValue, const CXVariant& xValue ){ return nValue <= xValue.nValue; }

	//__inline bool operator >( float fValue, const CXVariant& xValue ){ return fValue > xValue.fValue; }
	//__inline bool operator >( int nValue, const CXVariant& xValue ){ return nValue > xValue.nValue; }

	//__inline bool operator >=( float fValue, const CXVariant& xValue ){ return fValue >= xValue.fValue; }
	//__inline bool operator >=( int nValue, const CXVariant& xValue ){ return nValue >= xValue.nValue; }

	////////////////////////////////////////////////////////////////////////////
	//__inline bool operator <( const CXVariant& xValue, float fValue ){ return xValue.fValue < fValue; }
	//__inline bool operator <( const CXVariant& xValue, int nValue ){ return xValue.nValue < nValue; }

	//__inline bool operator <=( const CXVariant& xValue, float fValue ){ return xValue.fValue <= fValue; }
	//__inline bool operator <=( const CXVariant& xValue, int nValue ){ return xValue.nValue <= nValue; }

	//__inline bool operator >( const CXVariant& xValue, float fValue ){ return xValue.fValue > fValue; }
	//__inline bool operator >( const CXVariant& xValue, int nValue ){ return xValue.nValue > nValue; }

	//__inline bool operator >=( const CXVariant& xValue, float fValue ){ return xValue.fValue >= fValue; }
	//__inline bool operator >=( const CXVariant& xValue, int nValue ){ return xValue.nValue >= nValue; }

	////////////////////////////////////////////////////////////////////////////
	//__inline CXVariant operator+( const CXVariant& xValue, int nValue ){ return xValue.nValue + nValue; }
	//__inline CXVariant operator-( const CXVariant& xValue, int nValue ){ return xValue.nValue - nValue; }
	//__inline CXVariant operator*( const CXVariant& xValue, int nValue ){ return xValue.nValue * nValue; }
	//__inline CXVariant operator/( const CXVariant& xValue, int nValue ){ return xValue.nValue / nValue; }

	////////////////////////////////////////////////////////////////////////////
	//__inline CXVariant operator+( const CXVariant& xValue, float fValue ){ return xValue.fValue + fValue; }
	//__inline CXVariant operator-( const CXVariant& xValue, float fValue ){ return xValue.fValue - fValue; }
	//__inline CXVariant operator*( const CXVariant& xValue, float fValue ){ return xValue.fValue * fValue; }
	//__inline CXVariant operator/( const CXVariant& xValue, float fValue ){ return xValue.fValue / fValue; }

	class CORE_API CAttrib
	{
	public:
		CAttrib()
			: Value( 0 )
		{
		}

		CAttrib( const CXVariant& InitValue )
			: Value( InitValue )
		{
		}

		~CAttrib()
		{
		}

		operator const CXVariant&()const{ return Value; }
		operator CXVariant()const{ return Value; }

		__inline int ChangeIntegerPercent( float fPercent, int nBaseValue )
		{
			return Value.nValue += int( nBaseValue * fPercent * 0.01f );
		}

		__inline float ChangeFloatPercent( float fPercent, float fBaseValue )
		{
			return Value.fValue += fBaseValue * fPercent * 0.01f;
		}

		__inline float ChangeFloatValue( float fValue )
		{
			return Value.fValue += fValue;
		}

		__inline int ChangeIntegerValue( int nValue )
		{
			return Value.nValue += nValue;
		}

		//////////////////////////////////////////////////////////////////////////
		// Limit
		//////////////////////////////////////////////////////////////////////////
		__inline int ChangeIntegerPercent_Range( float fPercent, int nBaseValue, int nMin, int nMax )
		{
			int nValue = Value.nValue + int( nBaseValue * fPercent * 0.01f );

			if( nValue > nMax )
				Value.nValue = nMax;
			else if( nValue < nMin )
				Value.nValue = nMin;
			else
				Value.nValue = nValue;

			return nValue - Value.nValue;
		}

		__inline float ChangeFloatPercent_Range( float fPercent, float fBaseValue, float fMin, float fMax )
		{
			float fValue = Value.fValue + int( fBaseValue * fPercent * 0.01f );

			if( fValue > fMax )
				Value.fValue = fMax;
			else if( fValue < fMin )
				Value.fValue = fMin;
			else
				Value.fValue = fValue;

			return fValue - Value.fValue;
		}

		__inline float ChangeFloatValue_Range( float fValue, float fMin, float fMax )
		{
			fValue += Value.fValue;
		
			if( Value.fValue > fMax )
				Value.fValue = fMax;
			else if( Value.fValue < fMin )
				Value.fValue = fMin;
			else
				Value.fValue = fValue;

			return fValue - Value.fValue;
		}

		__inline int ChangeIntegerValue_Range( int nValue, int nMin, int nMax )
		{
			nValue += Value.nValue;

			if( Value.nValue > nMax )
				Value.nValue = nMax;
			else if(Value.nValue < nMin )
				Value.nValue = nMin;
			else
				Value.nValue = nValue;

			return nValue - Value.nValue;
		}

		//////////////////////////////////////////////////////////////////////////
		// value operator
		//////////////////////////////////////////////////////////////////////////
		__inline void SetBit( int idx, bool bit )
		{
			Value.nValue |= (1<<idx);
		}

		__inline void GetBit( int idx )
		{
			Value.nValue &= (1<<idx);
		}

		__inline void SetValue( const CXVariant Val )
		{ 
			Value = Val; 
		}

		__inline const CXVariant& GetValue()const 
		{ 
			return Value; 
		}

		__inline float GetFloat()const
		{
			return Value.fValue;
		}

		__inline int GetInt()const
		{
			return Value.nValue;
		}
	private:
		CXVariant Value;	// 基准值
	};

	typedef function< void ( _uint32 /*nType*/, _uint32 /*nIndex*/, const CXVariant& /*NewValue*/, const CXVariant& /*OldValue*/ ) > ListenFunction;

	static ListenFunction EmptyListener;
	struct CORE_API CAttribute
	{
	public:
		CAttribute()
			: m_Attrib( xgc_nullptr )
			, m_nCount( 0 )
			, m_nType( -1 )
			, m_ListenFunction( EmptyListener )
		{

		}

		CAttribute( CAttrib* Attrib, _uint32 nCount, unsigned int nType, ListenFunction &Fn = EmptyListener )
			: m_Attrib( Attrib )
			, m_nCount( nCount )
			, m_nType( nType )
			, m_ListenFunction( Fn )
		{

		}

		CAttribute( const CAttribute& src )
			: m_Attrib( src.m_Attrib )
			, m_nCount( src.m_nCount )
			, m_nType( src.m_nType )
			, m_ListenFunction( src.m_ListenFunction )
		{

		}

		CAttribute& operator=( const CAttribute& src )
		{
			m_Attrib = src.m_Attrib;
			m_nCount = src.m_nCount;
			m_nType = src.m_nType;
			m_ListenFunction = src.m_ListenFunction;

			return *this;
		}

		bool IsValid()
		{
			return m_Attrib != xgc_nullptr;
		}

		__inline int ChangeIntegerPercent( unsigned int nIndex, float fPercent, int nBaseValue )	
		{
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();
			int ret = m_Attrib[nIndex].ChangeIntegerPercent( fPercent, nBaseValue );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}

			return ret;
		}

		__inline float ChangeFloatPercent( unsigned int nIndex, float fPercent, float fBaseValue )	
		{
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();
			float ret = m_Attrib[nIndex].ChangeFloatPercent( fPercent, fBaseValue );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}

			return ret;
		}

		__inline float ChangeFloatValue( unsigned int nIndex, float fValue )		
		{ 
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();
			float ret = m_Attrib[nIndex].ChangeFloatValue( fValue ); 
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}

			return ret;

		}

		__inline int ChangeIntegerValue( unsigned int nIndex, int nValue )		
		{
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();
			int ret = m_Attrib[nIndex].ChangeIntegerValue( nValue );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}

			return ret;
		}

		//////////////////////////////////////////////////////////////////////////
		// Limit
		//////////////////////////////////////////////////////////////////////////
		__inline int ChangeIntegerPercent_Range( unsigned int nIndex, float fPercent, int nBaseValue, int nMin, int nMax )
		{ 
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();
			int ret = m_Attrib[nIndex].ChangeIntegerPercent_Range( fPercent, nBaseValue, nMin, nMax );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}

			return ret;
		}

		__inline float ChangeFloatPercent_Range( unsigned int nIndex, float fPercent, float fBaseValue, float fMin, float fMax )
		{
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();
			float ret = m_Attrib[nIndex].ChangeFloatPercent_Range( fPercent, fBaseValue, fMin, fMax );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}

			return ret;
		}

		__inline float ChangeFloatValue_Range( unsigned int nIndex, float fValue, float fMin, float fMax )
		{
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();

			float ret = m_Attrib[nIndex].ChangeFloatValue_Range( fValue, fMin, fMax );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}

			return ret;
		}

		__inline int ChangeIntegerValue_Range( unsigned int nIndex, int nValue, int nMin, int nMax )
		{ 
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();
			int ret = m_Attrib[nIndex].ChangeIntegerValue_Range( nValue, nMin, nMax ); 
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}

			return ret;
		}

		//////////////////////////////////////////////////////////////////////////
		// value operator
		//////////////////////////////////////////////////////////////////////////
		__inline void SetBit( unsigned int nIndex, int idx, bool bit )
		{
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") );
			CXVariant xOldValue = m_Attrib[nIndex].GetValue();
			m_Attrib[nIndex].SetBit( idx, bit );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, nIndex, m_Attrib[nIndex].GetValue(), xOldValue );
			}
		}

		__inline void SetIntegerValue( unsigned int nIndex, int nValue )
		{ 
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") ); 
			m_Attrib[nIndex].SetValue( nValue ); 
		}

		__inline void SetFloatValue( unsigned int nIndex, float fValue )
		{ 
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") ); 
			m_Attrib[nIndex].SetValue( fValue ); 
		}

		__inline const CXVariant& GetValue( unsigned int nIndex )const
		{ 
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") ); 
			return m_Attrib[nIndex].GetValue(); 
		}

		__inline float GetFloat( unsigned int nIndex )const
		{ 
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") ); 
			return m_Attrib[nIndex].GetFloat(); 
		}

		__inline int GetInt( unsigned int nIndex )const
		{ 
			ASSERT_MSG( nIndex < m_nCount, _T("属性下标超出范围。") ); 
			return m_Attrib[nIndex].GetInt(); 
		}

	private:
		unsigned int	m_nCount;
		unsigned int	m_nType;
		CAttrib*		m_Attrib;
		ListenFunction&	m_ListenFunction;
	};
}
#endif // _XATTRIBUTE_H