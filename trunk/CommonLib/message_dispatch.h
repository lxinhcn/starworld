#pragma once

#ifndef _MESSAGE_DISPATCH_H
#define _MESSAGE_DISPATCH_H

#include <vector>
#include <map>
struct message_dispatch_base
{
	virtual size_t process( const char* data, size_t size ) = 0;
};

template< class handler >
struct message_dispatch_base_impl	:	public message_dispatch_base
{
	typedef size_t ( handler::* proc )( const char* data, size_t size );
};

template< class dispatch_base, class idtype >
struct functionRegister
{
	typedef typename dispatch_base::proc proc;
	void regist( idtype id, proc func )
	{
		message[id] = func;
	}

	inline proc get( idtype id )
	{
		std::map< idtype, proc >::iterator i = message.find( id );

		if( i != message.end() )
			return i->second;
		else
			return NULL;
	}

	inline proc dismiss( idtype id )
	{
		std::map< idtype, proc >::iterator i = message.find( id );

		if( i != message.end() )
		{
			proc ret = i->second;
			message.erase( i );
			return ret;
		}
		else
			return NULL;
	}

	std::map< idtype, proc >	message;
};

template< class dispatch_base >
struct functionRegister< dispatch_base, unsigned char >
{
	typedef typename dispatch_base::proc proc;
	void regist( unsigned char id, proc func )
	{
		if( id >= message.size() )
		{
			message.resize( id + 1, NULL );
		}

		message[id] = func;
	}

	inline proc get( unsigned char id )
	{
		return id < message.size()?message[id]:NULL;
	}

	inline proc dismiss( unsigned char id )
	{
		proc ret = id < message.size()?message[id]:NULL;
		if( ret )
		{
			message[id] = NULL;
		}
		return ret;
	}

	std::vector< proc >	message;
};

template
< 
	class handler, 
	class dispatch_base = message_dispatch_base_impl< handler >, 
	class function_register = functionRegister< dispatch_base, unsigned char >
> 
class message_dispatch :	public dispatch_base
{
protected:
	message_dispatch(void){}
	~message_dispatch(void){}

protected:
	typedef typename function_register functionImpl;
	static functionImpl		Impl;
};

#endif // _MESSAGE_DISPATCH_H