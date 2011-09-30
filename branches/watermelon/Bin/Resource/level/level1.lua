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

function CreateLevel( world )
	bd = BodyDef();
	shape = EdgeShape();
	shape:Set(Vector2(-20.0, 0.0), Vector2(20.0, 0.0));
	
	ground = world:CreateBody(bd);


	ground:CreateFixtureEx(shape,0);
end;