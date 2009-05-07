function class_attrib( item, name, indent )
	local attrib = item[name];
	if attrib == nil then return "Nil" end;
	if type( attrib ) == "userdata" then
		return attrib:members( indent + 1 );
	else
		return tostring( attrib );
	end;
end;

function save( item, name, node )
	local attrib = item[name];
	if attrib == nil then return end;
	if type( attrib ) == "userdata" then
		local element = Element( name );
		attrib:save( element );
		node:InsertChild( element );
	elseif type( attrib ) == "function" then
		slb.print( "lua function" );
	else
		local element = Element( name );
		element:SetAttribute( "value", tostring( attrib ) );
		node:InsertChild( element );
	end;
end;

function load( item, name, node )
	if node then
		local attrib = item[name];
		if attrib == nil then 
			local class = node:Attribute( "class" );
			--slb.print( string.format( "create class %s", class ) );
			if class and class == "sprite" then
				local sprite_attr = ui.XUI_SpriteAttribute("", 0, 0, 0, 0);
				sprite_attr:load( node );
				--slb.print( string.format( "create %s, %f, %f", sprite_attr.f, sprite_attr.x, sprite_attr.y ) );
				item[name] = CreateSpriteEx( sprite_attr );
			end;
		elseif type( attrib ) == "userdata" then
			attrib:load( node );
		else
			local value = node:Attribute( "value" );
			item[name] = value;
		end;
	end
end;
