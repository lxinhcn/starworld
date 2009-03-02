#pragma once
#ifdef GAMEWORLD_EXPORTS
#define GAMEWORLD_API __declspec(dllexport)
#else
#define GAMEWORLD_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#define INVALID_RESID -1
class GAMEWORLD_API CResourceMng
{
public:
	enum RES_TYPE{	RES_TYPE_SURFACE	= 0,	RES_TYPE_COUNT, };

protected:
	typedef struct _resource_
	{
		DWORD	nID;	
		DWORD	nType;
		DWORD	nRef;
		void*	pResource;
		_resource_()
		{
			nType = 0;
			nRef = 0;
			pResource = NULL;
		}
	}RESOURCE, *PRESOURCE;

protected:
	CResourceMng();
	~CResourceMng();
public:
	static	CResourceMng& GetInstance();

	int		AddResource( RES_TYPE enType, void* pRes );

	BOOL	DeleteResource( DWORD dwResID );

	BOOL	IsResIDValid( DWORD dwResID );
	void*	GetResource( DWORD dwResID );
	void	ReleaseResource( DWORD dwResID );

private:
	struct	Impl;
	Impl*	m_Impl;
};
