#include "stdafx.h"
#include "Debuger.h"
#include "Broadcast.h"
#include "App.h"
extern StudioApp theApp;
bool ProcessRetCommand( char* data, size_t size, size_t maxsize )
{
	if( data && size )
	{
		switch( data[0] )
		{
		case '$':
			return false;
		case '@':
			{
				if( _strnicmp( "break", data+1, 5 ) == 0 )
				{
					CWnd* pWnd = theApp.GetMainWnd();
					ASSERT( pWnd );
					if( pWnd )
						pWnd->PostMessage( CBroadcast::WM_USER_EVENT, CBroadcast::Event_Break, 0 );
					return false;
				}
			}
		}
	}
	return true;
}