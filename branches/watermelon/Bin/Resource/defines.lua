slb.using( slb )
slb.using( slb.HGE )

texture = 
{
	cursor = slb.Texture( "cursors.png" );
};

animation = 
{
	cursor = Animation( texture.cursor, 1, 4, 12, 1.0, 0.0, 0.0, 42.0, 55.0 );
};
