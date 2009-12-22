// MessageCreator.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "AnalyseFile.h"
#include <conio.h>
#include <direct.h>
#include <process.h>

using namespace std;
const char *usage = 
"MessageCreator v1.1 write by albert.xu\n"
"usage format: MessageCreator filename1[ filename2[ filename3...]] [[-d|-r] [-mt|-md] [-dll|-lib] [-h]]\n"
"\t-d\t\tdebug version.\n"
"\t-r\t\trelease version.\n"
"\t-mt\t\tmutithread stand library.\n"
"\t-md\t\tmutithread dll library.\n"
"\t-dll\t\tcreate dll library.\n"
"\t-lib\t\tcreate static library\n"
"\t-h\t\tdisplay usage.\n";

void compiler( root *proot );
void link( root *proot );
int main(int argc, char* argv[])
{
	root r;
	int ierror = 0;

	char szValue[1024];
	char szFullpath[1024];
	DWORD nRead = GetPrivateProfileStringA( "Config", "BinDir", ".\\lib", szValue, _countof(szValue), ".\\config.ini" );
	_fullpath( szFullpath, szValue, sizeof(szFullpath) );
	CreateDirectoryA( szFullpath, NULL );
	r.bindir = szFullpath;

	nRead = GetPrivateProfileStringA( "Config", "OutDir", ".\\include", szValue, _countof(szValue), ".\\config.ini" );
	_fullpath( szFullpath, szValue, sizeof(szFullpath) );
	CreateDirectoryA( szFullpath, NULL );
	r.outdir = szFullpath;

	nRead = GetPrivateProfileStringA( "Config", "LibDir", ".\\lib", szValue, _countof(szValue), ".\\config.ini" );
	_fullpath( szFullpath, szValue, sizeof(szFullpath) );
	CreateDirectoryA( szFullpath, NULL );
	r.libdir = szFullpath;

	r.opt.version = "/D\"NDEBUG\" /O2";
	r.opt._debug = false;
	r.opt.runtime = "/MD";
	r.opt._runtime = false;
	r.opt.linktar = "/D_LIB";
	r.opt._library = true;

	for( int i = 1; i < argc; ++i )
	{
		if( argv[i][0] != '-' )
		{
			ierror += analysefile( &r, argv[i] );
		}
		else
		{
			if( _stricmp( argv[i], "-help" ) == 0 )
			{
				puts( usage );
			}
			else if( _stricmp( argv[i], "-d" ) == 0 )
			{
				r.opt.version = "/D\"_DEBUG\" /Od /ZI";
				r.opt._debug = true;
			}
			else if( _stricmp( argv[i], "-r" ) == 0 )
			{
				r.opt.version = "/D\"NDEBUG\" /O2";
				r.opt._debug = false;
			}
			else if( _stricmp( argv[i], "-mt" ) == 0 )
			{
				r.opt.runtime = "/MT";
				r.opt._runtime = true;
			}
			else if( _stricmp( argv[i], "-md" ) == 0 )
			{
				r.opt.runtime = "/MD";
				r.opt._runtime = false;
			}
			else if( _stricmp( argv[i], "-dll" ) == 0 )
			{
				r.opt.linktar = "/D_USRDLL /DMESSAGE_API";
				r.opt._library = false;
			}
			else if( _stricmp( argv[i], "-lib" ) == 0 )
			{
				r.opt.linktar = "/D_LIB";
				r.opt._library = true;
			}
		}
	}

	if( ierror == 0 )
	{
		writefile( &r );
	}

	compiler( &r );
	link( &r );
	destroyall( &r );

	remove( "compiler.rsp" );
	remove( "link.rsp" );
	//remove( "structsdef.cpp");
	//remove( "structsdef.obj");
	//for( list< string >::iterator i = r.files.begin(); i != r.files.end(); ++i )
	//{
	//	string filename(*i);
	//	remove( (filename + ".cpp").c_str() );
	//	remove( (filename + ".obj").c_str() );
	//}

	//_getch();
	return 0;
}

void compiler( root *proot )
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	CreateDirectoryA( ".\\temp", NULL );
	fstream f;
	f.open( "compiler.rsp", ios_base::out|ios_base::trunc );
	if( f.is_open() )
	{
		string strfiles( proot->outdir );
		strfiles += "\\structsdef.cpp\n";
		for( list< string >::iterator i = proot->files.begin(); i != proot->files.end(); ++i )
		{
			strfiles += proot->outdir + "\\";
			strfiles += *i;
			strfiles += ".cpp\n";
		}

		//wchar_t szDirectory[1024];
		//GetCurrentDirectoryW( sizeof(szDirectory), szDirectory );

		f << "/I\"" << proot->outdir << "\" ";
		// /I\"..\\..\\xgcBase\\Common\" /I\"..\\..\\Depend\\Loki-0.1.6\\include\" /W3 "
		char szValue[1024];
		DWORD nRead = GetPrivateProfileStringA( "Config", "IncludePath", "..\\..\\xgcBase\\Common;..\\..\\Depend\\Loki-0.1.6\\include", szValue, _countof(szValue), ".\\config.ini" );
		char *token = NULL, *end = szValue+nRead, *next = NULL;
		token = strtok_s( szValue, ";", &next );
		while( token )
		{
			f << "/I\"" << token << "\" ";
			token = strtok_s( NULL, ";", &next );
		}

		f
			<< proot->opt.version << " "
			<< proot->opt.runtime
			<< (proot->opt._debug?"d ":" ")
			<< proot->opt.linktar << " "
			<< " /EHsc /GR "
			<< "/Fp\".\\temp\\message.pch\" "
			<< "/Fo\".\\temp\\\\\" "
			<< "/Fd\"" << ".\\temp\\vc90.pdb\" /c /ZI /TP "
			<< strfiles;

		f.close();

		//printf( "Current directory is %S\n", szDirectory );
		_tchar szCommandline[] = _T("cl.exe @compiler.rsp /nologo");
		BOOL ret = CreateProcess( 
			NULL,
			szCommandline, 
			NULL, 
			NULL, 
			TRUE, 
			0,
			NULL, 
			NULL, 
			&si, 
			&pi );

		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
}

void link( root *proot )
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	fstream f;
	f.open( "link.rsp", ios_base::out|ios_base::trunc );
	if( f.is_open() )
	{
		string strfiles( ".\\temp\\structsdef.obj \n" );
		for( list< string >::iterator i = proot->files.begin(); i != proot->files.end(); ++i )
		{
			strfiles += ".\\temp\\";
			strfiles += *i;
			strfiles += ".obj \n";
		}

		if( proot->opt._library )
		{
			char szValue[1024];
			GetPrivateProfileStringA( "Config", "OutName", "Message", szValue, _countof(szValue), ".\\config.ini" );
			f << "/OUT:" << proot->libdir << "\\" << szValue << ".lib ";
			GetPrivateProfileStringA( "Config", "LibraryPath", ".\\", szValue, _countof(szValue), ".\\config.ini" );
			f << "/LIBPATH:" << szValue << " ";
			f << strfiles;
			f.close();

			_tchar szCommandline[] = _T("lib.exe @link.rsp /nologo");

			BOOL ret = CreateProcess( 
				NULL,
				szCommandline,
				NULL, 
				NULL, 
				TRUE, 
				0,
				NULL, 
				NULL, 
				&si, 
				&pi );
		}
		else
		{
			char szValue[1024];
			char szFullpath[1024];
			GetPrivateProfileStringA( "Config", "OutName", "Message", szValue, _countof(szValue), ".\\config.ini" );
			f << "/OUT:" << proot->bindir << "\\" << szValue << ".dll /DLL ";
			f << "/IMPLIB:" << proot->libdir << "\\" << szValue << ".lib ";
			f << (proot->opt._debug?"/DEBUG ":" ") << "/PDB:" << proot->bindir << "\\" << szValue << ".pdb ";
			//f << "/MANIFEST /MANIFESTFILE:\"" << szValue << ".dll.intermediate.manifest\" /MANIFESTUAC:\"level='asInvoker' uiAccess='false'\" ";
			GetPrivateProfileStringA( "Config", "LibraryPath", ".\\", szValue, _countof(szValue), ".\\config.ini" );
			_fullpath( szFullpath, szValue, sizeof(szFullpath) );
			f << "/LIBPATH:\"" << szFullpath << "\" ";
			GetPrivateProfileStringA( "Config", "LinkLib", "Common.lib", szValue, _countof(szValue), ".\\config.ini" );
			f << "/MACHINE:X86 " << szValue << endl;
			f << strfiles;
			f.close();

			_tchar szCommandline[] = _T("link.exe @link.rsp /nologo");

			BOOL ret = CreateProcess( 
				NULL,
				szCommandline,
				NULL, 
				NULL, 
				TRUE, 
				0,
				NULL, 
				NULL, 
				&si, 
				&pi );
		}

		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
}