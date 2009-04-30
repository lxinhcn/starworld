slb.using( slb );
slb.using( slb.xml );
slb.print( "@ initialize script system ..." );
slb.print( "@ script path = "..script );
tfunc = function( _handle, _repeat, _timer )
	DrawText( string.format( "timer = %d, repeat = %d", _timer, _repeat ), nil, 10, 60 );
	return true;
end;

t = SetTimer( tfunc, 10000, 1 );

dofile( script.."ui\\ui.lua" )

