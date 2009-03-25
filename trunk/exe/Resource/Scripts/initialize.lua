slb.using( slb );
slb.using( slb.xml );
slb.print( "@ initialize script system ..." );
slb.print( "@ script path = "..script );
dofile( script.."ui\\ui.lua" )
