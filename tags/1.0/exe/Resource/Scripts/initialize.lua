slb.using( slb );
slb.using( slb.xml );
slb.print( "@ initialize script system ..." );
slb.print( "@ script path = "..script );

timercount = 0;
tfunc = function( _handle, _repeat, _timer )
	timercount = timercount + 1;
	return true;
end;

t = SetTimer( tfunc, 10000, 1 );

dofile( script.."ui\\ui.lua" )

