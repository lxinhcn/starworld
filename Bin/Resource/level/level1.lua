slb.using( slb.HGE )
slb.using( slb.b2 )
slb.using( slb )

texture = 
{
	background = Texture( "background.png" );
};

sprites = 
{
	background = Sprite( texture.background, 0, 0, TextureWidth(texture.background), TextureHeight(texture.background) ); 
};

animation = 
{
};
