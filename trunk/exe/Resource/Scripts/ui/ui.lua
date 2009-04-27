dofile( script.."ui\\utility.lua" )

function UIUpdateEntry( timer, delta )
	DrawTextA( string.format( "timer = %f, delta = %f", timer, delta ), nil, 10, 10 );
end;

function DesktopUpdate( timer, delta )
	DrawTextA( "DesktopUpdate", nil, 10, 40 );
end;

return true;