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
"usage format: MessageCreator filename1[ filename2[ filename3 [...] ] ] [options]\n"
" option = [[-d|-r] [-mt|-md] [-dll|-lib] [-config:configname][-h]]\n"
"\t-d\t\tdebug version.\n"
"\t-r\t\trelease version.\n"
"\t-mt\t\tmutithread stand library.\n"
"\t-md\t\tmutithread dll library.\n"
"\t-dll\t\tcreate dll library.\n"
"\t-lib\t\tcreate static library\n"
"\t-config\t\tconfig compiler params\n"
"\t-h\t\tdisplay usage.\n"
"-----------------------------------------------------------------------------------------\n";
void compiler( root *proot );
void link( root *proot );

int main(int argc, char* argv[])
{
	root r;
	int ierror = 0;

	char szValue[1024];
	char szFullpath[1024];

	r.opt.version = "/D\"NDEBUG\" /O2";
	r.opt._debug = false;
	r.opt.runtime = "/MD";
	r.opt._runtime = false;
	r.opt.linktar = "/D_LIB";
	r.opt._library = true;

	for( int i = 1; i < argc; ++i )
	{
		if( argv[i][0] != '-' && argv[i][0] != '/' )
		{
			ierror += analysefile( &r, argv[i] );
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
			else if( _strnicmp( argv[i], "-config:", 8 ) == 0 )
			{
				char* begin = strchr( argv[i], ':' );
				++begin;
				if( begin )
				{
					char ini[1024];
					_fullpath( ini, begin, sizeof(szFullpath) );
					
					DWORD nRead = GetPrivateProfileStringA( "Config", "BinDir", ".\\lib", szValue, _countof(szValue), ini );
					_fullpath( szFullpath, szValue, sizeof(szFullpath) );
					CreateDirectoryA( szFullpath, NULL );
					r.config.bindir = szFullpath;

					nRead = GetPrivateProfileStringA( "Config", "OutDir", ".\\include", szValue, _countof(szValue), ini );
					_fullpath( szFullpath, szValue, sizeof(szFullpath) );
					CreateDirectoryA( szFullpath, NULL );
					r.config.outdir = szFullpath;

					nRead = GetPrivateProfileStringA( "Config", "LibDir", ".\\lib", szValue, _countof(szValue), ini );
					_fullpath( szFullpath, szValue, sizeof(szFullpath) );
					CreateDirectoryA( szFullpath, NULL );
					r.config.libdir = szFullpath;

					GetPrivateProfileStringA( "Config", "OutName", "Message", szValue, _countof(szValue), ini );
					r.config.outname = szValue;

					GetPrivateProfileStringA( "Config", "LibName", "Message", szValue, _countof(szValue), ini );
					r.config.libname = szValue;

					GetPrivateProfileStringA( "Config", "LibraryPath", ".\\", szValue, _countof(szValue), ini );
					char *token = NULL, *end = szValue+nRead, *next = NULL;
					token = strtok_s( szValue, ";", &next );
					while( token )
					{
						r.config.lib << "/LIBPATH:\"" << token << "\" ";
						token = strtok_s( NULL, ";", &next );
					}

					GetPrivateProfileStringA( "Config", "LinkLib", "", szValue, _countof(szValue), ini );
					r.config.lib << "/MACHINE:X86 " << szValue << " ";

					nRead = GetPrivateProfileStringA( "Config", "IncludePath", ".\\", szValue, _countof(szValue), ini );
					end = szValue+nRead;
					next = NULL;
					token = strtok_s( szValue, ";", &next );
					while( token )
					{
						r.config.inc << "/I\"" << token << "\" ";
						token = strtok_s( NULL, ";", &next );
					}
				}
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
		string strfiles( proot->config.outdir );
		strfiles += "\\structsdef.cpp\n";
		for( list< string >::iterator i = proot->files.begin(); i != proot->files.end(); ++i )
		{
			strfiles += proot->config.outdir + "\\";
			strfiles += *i;
			strfiles += ".cpp\n";
		}

		f << "/I\"" << proot->config.outdir << "\" "
			<< proot->config.inc.rdbuf() << " "
			<< proot->opt.version << " "
			<< proot->opt.runtime << (proot->opt._debug?"d ":" ")
			<< proot->opt.linktar << " "
			<< " /EHsc /GR "
			<< "/Fp\".\\temp\\message.pch\" "
			<< "/Fo\".\\temp\\\\\" "
			<< "/Fd\"" << ".\\temp\\vc90.pdb\" /c /ZI /TP "
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
			f << "/OUT:" << proot->config.libdir << "\\" << proot->config.libname << " "
				<< proot->config.lib.rdbuf() << " " << endl
				<< strfiles;
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
			f << "/OUT:" << proot->config.bindir << "\\" << proot->config.outname << " /DLL "
				<< "/IMPLIB:" << proot->config.libdir << "\\" << proot->config.libname << ".lib "
				<< (proot->opt._debug?"/DEBUG ":" ") << "/PDB:" << proot->config.bindir << "\\" << proot->config.outname << ".pdb "
				<< proot->config.lib.rdbuf() << " "
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