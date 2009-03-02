slb.using( slb.ui );
slb.print( "@ initialize script system ..." );
slb.print( "@ script path = "..script );
if dofile( script.."ui\\utility.lua" ) then
	slb.print( "@ initialize script system successful!" );
end
