slb.using( slb.HGE )

setting = 
{
	resource = "..\\resource",
	font = 
	{
		fontname = "..\\resource\\ARIALN1.ttf",
		fontsize = 18,
		fontcolor = -1,
	},
};

texture = 
{
	background = Texture( "background.png" );
	block1 = Texture( "blocks1.png" );
	block2 = Texture( "blocks2.png" );
};

sprites = 
{
	wood1 = Sprite( texture.block1, 310, 644, 204, 20 );
	wood2 = Sprite( texture.block1, 310, 666, 204, 20 );
};
