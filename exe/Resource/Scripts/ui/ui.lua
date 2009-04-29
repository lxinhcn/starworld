dofile( script.."ui\\utility.lua" )

function UIUpdateEntry( timer, delta )
	DrawText( string.format( "timer = %f, delta = %f", timer, delta ), nil, 10, 10 );
end;

function DesktopUpdate( element, timer, delta )
	local tx = 2*math.sin(timer/60);
	local ty = 2*math.cos(timer/60);
	DrawText( string.format( "desktop tx= %f, ty = %f", tx, ty ), nil, 10, 40 );
	local pBackground = element:GetBackground();
	pBackground:SetUV( tx, ty, tx+1, ty+1 );
end;

return true;