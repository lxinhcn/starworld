#pragma once

namespace XGC
{
	struct NameInfo;
	struct XStaticName_HashTable;
	struct CORE_API XStaticName
	{
		friend struct XStaticName_HashTable;

		//////////////////////////////////////////////////////////////////////////
		bool operator==( const XStaticName& rhs ) const
		{
			return mIndex == rhs.mIndex && mTable == rhs.mTable;
		}

		bool operator !=( const XStaticName& rhs ) const
		{
			return mIndex != rhs.mIndex || mTable != rhs.mTable;
		}

	private:
		unsigned short mTable;
		unsigned short mIndex;
	};

	struct CORE_API XStaticName_HashTable
	{
	public:
		XStaticName_HashTable( unsigned short nHashSize = 65535 );
		~XStaticName_HashTable();

		XStaticName Name( const _tchar* str );

		_string& GetString( const XStaticName& Name ) const;

	protected:
		void StaticInit();
		void StaticUninit();

	private:
		const unsigned short cTableIdx;
		const unsigned short cHashSize;

		NameInfo** gNameInfos;
		xgc_vector<NameInfo*> gNames;

		static unsigned short gTableCount;
	};

	extern XStaticName_HashTable StaticTable;

}