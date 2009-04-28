dofile( script.."ui\\utility.lua" )

function UIUpdateEntry( timer, delta )
	DrawText( string.format( "timer = %f, delta = %f", timer, delta ), nil, 10, 10 );
end;

function DesktopUpdate( element, timer, delta )
	local tx = math.sin(timer);
	local ty = math.cos(timer);
	DrawText( string.format( "desktop tx= %f, ty = %f", tx, ty ), nil, 10, 40 );
	local pBackground = element:GetBackground();
	
	pBackground:SetUV( tx, ty, 1+tx, 1+ty );
end;

return true;