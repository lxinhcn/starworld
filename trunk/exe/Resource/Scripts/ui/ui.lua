dofile( script.."ui\\utility.lua" )

function UIUpdateEntry( timer, delta )
	DrawTextA( string.format( "timer = %f, delta = %f", timer, delta ), nil, 10, 10 );
end;

return true;