slb.using( slb )
slb.using( slb.HGE )
slb.using( slb.b2 )
slb.using( slb.game)

texture = 
{
	background = Texture( "background.png" );
	blocks1 = Texture( "blocks1.png" );
	blocks2 = Texture( "blocks2.png" );
};

sprites = 
{
	background = Sprite( texture.background, 0, 0, TextureWidth(texture.background), TextureHeight(texture.background) ); 
	
	wood =
	{
		Sprite( texture.blocks1, 310, 644, 205, 20 ),
		Sprite( texture.blocks1, 310, 666, 205, 20 ),
	},
};

animation = 
{
};

gravity = Vector2( 0, -10 );
zoom = 50;
offset = { x = -10, y = 0 };

function ground( world )
	bd = BodyDef();
	shape = EdgeShape();
	shape:Set(Vector2(-20.0, 0.0), Vector2(20.0, 0.0));
	ground = world:CreateBody(bd);
	ground:CreateFixtureEx(shape,0);
end;

function wood( world, x, y )
	local bd = BodyDef();
	bd.type = b2dynamic;
	bd.position:Set( x, y );
	
	local shape = PolygonShape();
	shape:SetAsBox( 2.00, 0.2 );
	
	local wood = world:CreateBody(bd);
	fixture = wood:CreateFixtureEx( shape, 0.7 );
	
	local data = GameObject();
	data.type = 1;
	data.hp = 100;
	data.st = 0
	data.sprite = sprites.wood[1];
	fixture.userdata = rawptr(data);
end;

function CreateLevel( world )
	ground( world );
	wood( world, 0, 0.8*3 );
	wood( world, 3.5, 0.8*4 );
	
	wood( world, 0, 1.6*3 );
	wood( world, 0, 2.4*3 );
	wood( world, 10.5, 2.4*3 );
end;