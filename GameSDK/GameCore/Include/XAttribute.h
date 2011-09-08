#ifndef _XATTRIBUTE_H
#define _XATTRIBUTE_H

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

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
			Value.nValue += int( nBaseValue * fPercent * 0.01f );
			
			if( Value.nValue > nMax )
				Value.nValue = nMax;
			else if( Value.nValue < nMin )
				Value.nValue = nMin;

			return Value.nValue;
		}

		__inline float ChangeFloatPercent_Range( float fPercent, float fBaseValue, float fMin, float fMax )
		{
			Value.fValue += int( Value.fValue * fPercent * 0.01f );

			if( Value.fValue > fMax )
				Value.fValue = fMax;
			else if( Value.fValue < fMin )
				Value.fValue = fMin;

			return Value.fValue;
		}

		__inline float ChangeFloatValue_Range( float fValue, float fMin, float fMax )
		{
			Value.fValue += fValue;
			
			if( Value.fValue > fMax )
				Value.fValue = fMax;
			else if( Value.fValue < fMin )
				Value.fValue = fMin;

			return Value.fValue;
		}

		__inline int ChangeIntegerValue_Range( int nValue, int nMin, int nMax )
		{
			Value.nValue += nValue;

			if( Value.nValue > nMax )
				Value.nValue = nMax;
			else if(Value.nValue < nMin )
				Value.nValue = nMin;

			return Value.nValue;
		}

		//////////////////////////////////////////////////////////////////////////
		// value operator
		//////////////////////////////////////////////////////////////////////////
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

	typedef std::tr1::function< void ( _uint32 /*nType*/, _uint32 /*nIndex*/, const CXVariant& /*Value*/ ) > ListenFunction;

	static ListenFunction EmptyListener;
	class CORE_API CAttribute
	{
	public:
		CAttribute( CAttrib& attrib, ListenFunction &Fn = EmptyListener, int nType = -1, int nIndex = -1 )
			: m_Attrib( attrib )
			, m_ListenFunction( Fn )
			, m_nType( nType )
			, m_nIndex( nIndex )
		{

		}

		CAttribute( const CAttribute& src )
			: m_Attrib( src.m_Attrib )
			, m_ListenFunction( src.m_ListenFunction )
			, m_nType( src.m_nType )
			, m_nIndex( src.m_nIndex )
		{

		}

		__inline int	ChangeIntegerPercent( float fPercent, int nBaseValue )	
		{ 
			int ret = m_Attrib.ChangeIntegerPercent( fPercent, nBaseValue );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, m_nIndex, m_Attrib.GetValue() );
			}

			return ret;
		}

		__inline float	ChangeFloatPercent( float fPercent, float fBaseValue )	
		{ 
			float ret = m_Attrib.ChangeFloatPercent( fPercent, fBaseValue );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, m_nIndex, m_Attrib.GetValue() );
			}

			return ret;
		}

		__inline float	ChangeFloatValue( float fValue )		
		{ 
			float ret = m_Attrib.ChangeFloatValue( fValue ); 
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, m_nIndex, m_Attrib.GetValue() );
			}

			return ret;

		}

		__inline int	ChangeIntegerValue( int nValue )		
		{
			int ret = m_Attrib.ChangeIntegerValue( nValue );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, m_nIndex, m_Attrib.GetValue() );
			}

			return ret;
		}

		//////////////////////////////////////////////////////////////////////////
		// Limit
		//////////////////////////////////////////////////////////////////////////
		__inline int	ChangeIntegerPercent_Range( float fPercent, int nBaseValue, int nMin, int nMax )
		{ 
			int ret = m_Attrib.ChangeIntegerPercent_Range( fPercent, nBaseValue, nMin, nMax );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, m_nIndex, m_Attrib.GetValue() );
			}

			return ret;
		}

		__inline float	ChangeFloatPercent_Range( float fPercent, float fBaseValue, float fMin, float fMax )
		{
			float ret = m_Attrib.ChangeFloatPercent_Range( fPercent, fBaseValue, fMin, fMax );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, m_nIndex, m_Attrib.GetValue() );
			}

			return ret;
		}

		__inline float	ChangeFloatValue_Range( float fValue, float fMin, float fMax )
		{
			float ret = m_Attrib.ChangeFloatValue_Range( fValue, fMin, fMax );
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, m_nIndex, m_Attrib.GetValue() );
			}

			return ret;
		}

		__inline int	ChangeIntegerValue_Range( int nValue, int nMin, int nMax )
		{ 
			int ret = m_Attrib.ChangeIntegerValue_Range( nValue, nMin, nMax ); 
			if( m_ListenFunction )
			{
				m_ListenFunction( m_nType, m_nIndex, m_Attrib.GetValue() );
			}

			return ret;
		}

		//////////////////////////////////////////////////////////////////////////
		// value operator
		//////////////////////////////////////////////////////////////////////////
		__inline const CXVariant& GetValue()const{ return m_Attrib.GetValue(); }
		__inline float GetFloat()const{	return m_Attrib.GetFloat(); }
		__inline int GetInt()const{ return m_Attrib.GetInt(); }

	private:
		int				m_nType;
		int				m_nIndex;
		CAttrib&		m_Attrib;
		ListenFunction&	m_ListenFunction;
	};
}
#endif // _XATTRIBUTE_H