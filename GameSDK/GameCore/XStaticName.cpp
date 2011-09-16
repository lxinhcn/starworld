#include "stdafx.h"
#include "XStaticName.h"
namespace XGC
{
	/* magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */ 
	static const unsigned int InitialFNV = 2166136261U; 
	static const unsigned int FNVMultiple = 16777619;

	unsigned short XStaticName_HashTable::gTableCount = 0;
	struct NameInfo
	{
		unsigned int mHashPos;
		unsigned int mIndex;
		_string* mNameStr;
		struct NameInfo* mNext;
	};

	XStaticName_HashTable StaticTable;

	/* Fowler / Noll / Vo (FNV) Hash */ 
	unsigned int Vge_Hash(const _tchar* s) 
	{ 
		unsigned int hash = InitialFNV; 
		for(unsigned int i = 0; i < _tcslen(s); i++) 
		{ 
			hash = hash ^ (s[i]);       /* xor  the low 8 bits */ 
			hash = hash * FNVMultiple;  /* multiply by the magic number */ 
		} 
		return hash; 
	} 

	XStaticName_HashTable::XStaticName_HashTable( unsigned short nHashSize /* = 65536 */ )
		: cHashSize( nHashSize )
		, cTableIdx( gTableCount++ )
		, gNameInfos( (NameInfo**)malloc( sizeof(NameInfo*)*nHashSize ) )
	{
		StaticInit();
	}

	XStaticName_HashTable::~XStaticName_HashTable()
	{
		StaticUninit();
	}

	void XStaticName_HashTable::StaticInit()
	{
		for ( int i = 0; i < cHashSize; ++i )
			gNameInfos[i] = xgc_nullptr;
	}

	void XStaticName_HashTable::StaticUninit()
	{
		for ( int i = 0; i < cHashSize; ++i )
		{
			NameInfo* naminfo = gNameInfos[i];
			while ( xgc_nullptr != naminfo )
			{
				NameInfo* curinfo = naminfo;
				naminfo = naminfo->mNext;

				delete curinfo->mNameStr;
				delete curinfo;
			}
		}

		gNames.clear();
	}

	XStaticName XStaticName_HashTable::Name( const _tchar* str )
	{
		XStaticName Name;
		// Make hash position
		unsigned int hashpos = Vge_Hash(str) & ( cHashSize - 1 );

		// Process conflict
		NameInfo* nameinfo = gNameInfos[hashpos];

		while ( xgc_nullptr != nameinfo )
		{
			if ( _tcsicmp((*nameinfo->mNameStr).c_str(), str ) == 0 )
			{
				Name.mIndex = nameinfo->mIndex;
				return Name;
			}

			nameinfo = nameinfo->mNext;
		}

		Name.mIndex = gNames.size();

		// Not found, make a new name info
		NameInfo* oldinfo = nameinfo;

		nameinfo = new NameInfo;
		nameinfo->mHashPos = hashpos;
		nameinfo->mNameStr = new _string(str);
		nameinfo->mIndex = Name.mIndex;
		nameinfo->mNext = xgc_nullptr;

		if (oldinfo)
			oldinfo->mNext = nameinfo;
		else
			gNameInfos[hashpos] = nameinfo;

		gNames.push_back(nameinfo);

		return Name;
	}

	_string& XStaticName_HashTable::GetString( const XStaticName& Name ) const
	{
		return *(gNames[Name.mIndex]->mNameStr);
	}

}