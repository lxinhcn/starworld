// MessageCreator.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "AnalyseFile.h"
#include <conio.h>
#include <direct.h>
#include <process.h>

using namespace std;
const char *usage = 
"-----------------------------------------------------------------------------------------\n"
"MessageCreator v1.1 write by albert.xu\n"
"@Copy right 2009\n"
"-----------------------------------------------------------------------------------------\n"
"usage format: MessageCreator <options> <filename1>[,<filename2>, ...]\n"
"\n"
" Options:\n"
"	-d			debug version.\n"
"	-r			release version.\n"
"	-mt			mutithread stand library.\n"
"	-md			mutithread dll library.\n"
"	-dll		create dll library.\n"
"	-lib		create static library.\n"
"	-prefix		set prefix for auto create files.\n"
"	-incdir		directory of auto make code files.\n"
"	-libdir		directory of implib file.\n"
"	-out		out file name.\n"
"	-unicode	using unicode character set.\n"
"	-h		display usage.\n"
"-----------------------------------------------------------------------------------------\n";
void compiler( root *proot );
void link( root *proot );

int main(int argc, char* argv[])
{
	root r;
	int ierror = 0;

	r.opt.version = "/D \"WIN32\" /D \"NDEBUG\" /O2 /Oi";
	r.opt._debug = false;
	r.opt.runtime = "/MD";
	r.opt._runtime = false;
	r.opt.linktar = "/D_LIB";
	r.opt._library = true;

	list< string > filelist;
	for( int i = 1; i < argc; ++i )
	{
		if( argv[i][0] != '-' && argv[i][0] != '/' )
		{
			filelist.push_back( argv[i] );
		}
		else
		{
			argv[i][0] = '-';
			if( _stricmp( argv[i], "--help" ) == 0 ||
				_stricmp( argv[i], "-h" ) == 0 || 
				_stricmp( argv[i], "-?" ) == 0 )
			{
				puts( usage );
				return 0;
			}
			else if( _stricmp( argv[i], "-d" ) == 0 )
			{
				r.opt.version = "/D \"WIN32\" /D \"_DEBUG\" /Od";
				r.opt._debug = true;
			}
			else if( _stricmp( argv[i], "-r" ) == 0 )
			{
				r.opt.version = "/D \"WIN32\" /D \"NDEBUG\" /O2 /Oi";
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
			else if( _stricmp( argv[i], "-unicode" ) == 0 )
			{
				r.opt.unicode = "/D \"_UNICODE\" /D \"UNICODE\" ";
			}
			else if( _strnicmp( argv[i], "-wait", 4 ) == 0 )
			{
				puts("wait for attach" );
				_getch();
			}
			else if( _strnicmp( argv[i], "-prefix", 7 ) == 0 && argv[i][7] == ':' )
			{
				r.config.prefix = argv[i]+8;
			}
			else if( _strnicmp( argv[i], "-incdir", 7 ) == 0 && argv[i][7] == ':' )
			{
				r.config.incdir = argv[i]+8;
			}
			else if( _strnicmp( argv[i], "-libdir", 7 ) == 0 && argv[i][7] == ':' )
			{
				r.config.libdir = argv[i]+8;
			}
			else if( _strnicmp( argv[i], "-out", 4 ) == 0 && argv[i][4] == ':' )
			{
				r.config.outname = argv[i]+5;
			}
			else if( _strnicmp( argv[i], "-implib", 7 ) == 0 && argv[i][7] == ':' )
			{
				r.config.libname = argv[i]+8;
			}
			else if( _strnicmp( argv[i], "-I", 2 ) == 0 )
			{
				r.config.inc += string( "/I\"" ) + string(argv[i]+2) + string("\" ");
			}
			else if( _strnicmp( argv[i], "-lib", 4 ) == 0 )
			{
				r.config.lib = string( "/MACHINE:X86 " ) + string( argv[i]+4 );
			}
			else if( _strnicmp( argv[i], "-libpath", 8 ) == 0 )
			{
				r.config.libpath = string( "/LIBPATH:" ) + string( argv[i]+8 );
			}
		}
	}

	for( list<string>::iterator i = filelist.begin(); i != filelist.end(); ++i )
	{
		ierror += analysefile( &r, i->c_str() );
	}

	if( ierror == 0 )
	{
		writefile( &r );
		compiler( &r );
		link( &r );
		destroyall( &r );

		 remove( "compiler.rsp" );
		 remove( "link.rsp" );
	}
	else
	{
		puts( "error found program abort!" );
	}

	return 0;
}

void compiler( root *proot )
{
	puts( "compiler code..." );
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
		string strfiles;
		for( list< string >::iterator i = proot->files.begin(); i != proot->files.end(); ++i )
		{
			strfiles += proot->config.incdir + "\\";
			strfiles += *i;
			strfiles += ".cpp\n";
		}

		f << "/I\"" << proot->config.incdir << "\" "
			<< proot->config.inc << " "
			<< proot->opt.version << " "
			<< proot->opt.unicode << " "
			<< proot->opt.runtime << (proot->opt._debug?"d ":" ")
			<< proot->opt.linktar << " "
			<< " /EHsc /Gy "
			<< "/Fp\".\\temp\\message.pch\" "
			<< "/Fo\".\\temp\\\\\" "
			<< "/Fd\"" << ".\\temp\\vc90.pdb\" /Zi /TP /c "
			<< strfiles;

		f.close();

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
	puts( "link..." );
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	fstream f;
	f.open( "link.rsp", ios_base::out|ios_base::trunc );
	if( f.is_open() )
	{
		string strfiles;
		//string strfiles( ".\\temp\\" );
		//strfiles += proot->config.prefix + "structsdef.obj \n";
		for( list< string >::iterator i = proot->files.begin(); i != proot->files.end(); ++i )
		{
			strfiles += ".\\temp\\";
			strfiles += *i;
			strfiles += ".obj \n";
		}

		if( proot->opt._library )
		{
			f << "/OUT:" << proot->config.libdir << "\\" << proot->config.libname << " "
				<< proot->config.lib << " "
				<< strfiles;
			f.flush();
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
			f << "/OUT:" << proot->config.outname << " /DLL "
				<< "/IMPLIB:" << proot->config.libdir << "\\" << proot->config.libname
				<< (proot->opt._debug?"/DEBUG ":" ") << "/PDB:" << proot->config.outname << ".pdb "
				<< proot->config.lib << " "
				<< strfiles;
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