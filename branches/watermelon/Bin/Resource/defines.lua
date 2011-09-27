slb.using( slb.HGE )

texture = 
{
	background = Texture( "background.png" );
	block1 = Texture( "blocks1.png" );
	block2 = Texture( "blocks2.png" );
	cursor = Texture( "cursors.png" );
};

sprites = 
{
	wood1 = Sprite( texture.block1, 310, 644, 204, 20 );
	wood2 = Sprite( texture.block1, 310, 666, 204, 20 );
};

animation = 
{
	cursor = Animation( texture.cursor, 1, 4, 12, 1.0, 0.0, 0.0, 42.0, 55.0 );
};
