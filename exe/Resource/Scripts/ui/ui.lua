dofile( script.."ui\\utility.lua" )

function UIUpdateEntry( timer, delta )
	DrawText( string.format( "time = %f, delta = %f, timer = %d", timer, delta, timercount ), nil, 10, 10 );
end;

function DesktopUpdate( element, timer, delta )
	local tx = 2*math.sin(timer/60);
	local ty = 2*math.cos(timer/60);
	local p = gui:GetRoot();
	DrawText( string.format( "desktop tx= %f, ty = %f %s", tx, ty, p.name ), nil, 10, 40 );
	local pBackground = element:GetBackground();
	if pBackground then
		pBackground:SetUV( tx, ty, tx+1, ty+1 );
	end;
end;

return true;