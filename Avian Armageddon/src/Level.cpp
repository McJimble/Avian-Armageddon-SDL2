#include "Level.h"

const char* Level::BASE_MAP_LOCATION = "./maps/";

Level::Level()
{

}

Level::~Level()
{
	for (auto& col : environmentCollisions)
	{
		col->Deregister();
	}
}

void Level::LoadMap(const std::string& mapFileName)
{
	XMLDocument map;
	
	std::string fullFileName = BASE_MAP_LOCATION;
	fullFileName.append(mapFileName);
	map.LoadFile(fullFileName.c_str());

	XMLElement* mapNode = map.FirstChildElement("map");
	
	mapNode->QueryIntAttribute("width", &this->levelWidth);
	mapNode->QueryIntAttribute("height", &this->levelHeight);

	// Load and create tilemaps.
	XMLElement* tileset = mapNode->FirstChildElement("tileset");
	if (tileset != nullptr)
	{
		while (tileset)
		{
			int firstGid;		
			const char* srcTSX;	// .tsx file where srcImg location is stored
			const char* srcImg;	// Actual file name of src image to use.

			tileset->QueryIntAttribute("firstgid", &firstGid);
			tileset->QueryAttribute("source", &srcTSX);

			// Src image information is in a different file, so we need
			// to grab it a few elements down in that separate doc.
			XMLDocument tilesetInfoDoc;
			std::string srcTSXActual = BASE_MAP_LOCATION;
			srcTSXActual.append(srcTSX);
			if (tilesetInfoDoc.LoadFile(srcTSXActual.c_str()) != 0)
				std::cout << "Unable to load tileset\n";
			
			XMLElement* tileNode = tilesetInfoDoc.FirstChildElement("tileset");
			srcImg = tileNode->FirstChildElement("image")->Attribute("source");
			std::string srcImgActual = srcImg;	// srcImg has a period at beginning of path,
			srcImgActual.erase(0, 1);			// so i'm manually removing that first.

			SDL_Texture* tex = SDL_CreateTextureFromSurface(Graphics::Instance()->Get_Renderer(), 
				Graphics::Instance()->LoadImage(srcImgActual, false));

			std::unique_ptr<Tileset> set = std::make_unique<Tileset>();
			set->srcTexture = tex;
			set->firstGid = firstGid;
			tilesets.push_back(std::move(set));

			tileset = tileset->NextSiblingElement("tileset");
		}
	}

	int totalTileScale = (Graphics::SPRITE_SCALE + 1) * Graphics::TILE_SIZE;

	// Load layers; this is where each tilemap will get created.
	XMLElement* layer = mapNode->FirstChildElement("layer");
	while (layer != nullptr)
	{
		int layerNum = layer->IntAttribute("id") - 1;
		Tilemap* newMap = new Tilemap();

		XMLElement* data = layer->FirstChildElement("data");
		while (data != nullptr)
		{
			XMLElement* tile = data->FirstChildElement("tile");
			int tileCount = 0;
			while (tile != nullptr)
			{
				// Query gid; if unchanged (still zero), then there is no
				// tile and we continue to next element.
				int gid = 0;
				tile->QueryIntAttribute("gid", &gid);
				if (gid == 0)
				{
					tileCount++;
					tile = tile->NextSiblingElement("tile");
					continue;
				}

				// Find tileset corresponding to current gid.
				// This is based on first gid
				Tileset* setRef = nullptr;
				for (int i = (int)tilesets.size() - 1; i >= 0; i--)
				{
					if (tilesets[i]->firstGid <= gid)
					{
						setRef = tilesets[i].get();
						gid -= setRef->firstGid;
						//std::cout << "using texture: " << i << "\n";
						break;
					}
				}

				// No gid found (shouldn't happen)
				if (setRef == nullptr)
				{
					tileCount++;
					tile = tile->NextSiblingElement("tile");
					continue;
				}

				// Find tile position
				int tileX, tileY;
				tileX = (tileCount % levelWidth) - (levelWidth / 2);
				tileY = (tileCount / levelWidth) - (levelHeight / 2);
				tileX *= totalTileScale;
				tileY *= totalTileScale;

				// Find tile element in texture
				int texW, texH;
				int elX, elY;
				SDL_QueryTexture(setRef->srcTexture, NULL, NULL, &texW, &texH);
				elX = gid % (texW / Graphics::TILE_SIZE);
				if (elX < 0) 
					elX = texW - Graphics::TILE_SIZE;
				elY = (gid / (texW / Graphics::TILE_SIZE));
				elX *= Graphics::TILE_SIZE;
				elY *= Graphics::TILE_SIZE;

				newMap->AddTile(setRef->srcTexture, elX, elY, tileX, tileY);

				tileCount++;

				tile = tile->NextSiblingElement("tile");
			}
			data = data->NextSiblingElement("data");
		}

		tilemaps[layerNum] = std::unique_ptr<Tilemap>(newMap);
		layer = layer->NextSiblingElement("layer");
	}

	// NOW get collision rects + obstacle points.
	XMLElement* objGroup = mapNode->FirstChildElement("objectgroup");
	while (objGroup != nullptr)
	{
		// Get Collision layer from property tag.
		// I realized far too late that if there are any more or less properties on
		// and object group, the whole thing breaks. Will fix if given time.
		CollisionLayer wallLayer = CollisionLayer::Wall;
		XMLElement* waterProp = objGroup->FirstChildElement("properties")->FirstChildElement("property");
		if (waterProp)
		{
			std::string propName = "isWater";
			if (propName.compare(waterProp->Attribute("name")) == 0 &&
				waterProp->BoolAttribute("value") == true)
			{
				wallLayer = CollisionLayer::Water;
			}
		}

		XMLElement* object = objGroup->FirstChildElement("object");
		while (object != nullptr)
		{
			float w = object->FloatAttribute("width") * (Graphics::SPRITE_SCALE + 1);
			float h = object->FloatAttribute("height") * (Graphics::SPRITE_SCALE + 1);
			float x = object->FloatAttribute("x") - ((levelWidth / 2) * Graphics::TILE_SIZE);
			float y = object->FloatAttribute("y") - ((levelHeight / 2) * Graphics::TILE_SIZE);
			x *= (Graphics::SPRITE_SCALE + 1);
			y *= (Graphics::SPRITE_SCALE + 1);
			x += w / 2.0f;
			y += h / 2.0f;

			Vector2D pos = Vector2D(x, y);
			Vector2D size = Vector2D(w, h);

			Collider* col = new Collider(size, nullptr, false, false,
				ColliderShape::Box, ColliderType::Static, wallLayer);
			col->Set_BoundsPos(pos);
			environmentCollisions.push_back(col);

			object = object->NextSiblingElement("object");
		}

		objGroup = objGroup->NextSiblingElement("objectgroup");
	}

	levelWidth *= totalTileScale;
	levelHeight *= totalTileScale;
}

void Level::RenderLevelObjects(SDL_Rect* camera)
{
	// Render tilemaps first, starting from lowest to highest layer.
	// std::map is ordered on its own, so this should work.
	for (auto it = tilemaps.rbegin(); it != tilemaps.rend(); it++)
	{
		(*it).second->RenderTilemap(camera);
	}

	for (auto& col : environmentCollisions)
	{
		/*
		SDL_FRect test = SDL_FRect();
		test.w = col->Get_BoundsSize()[0];
		test.h = col->Get_BoundsSize()[1];
		test.x = (col->Get_BoundsPos()[0] - test.w / 2.0) - (float)camera->x;
		test.y = (col->Get_BoundsPos()[1] - test.h / 2.0) - (float)camera->y;

		SDL_SetRenderDrawColor(Graphics::Instance()->Get_Renderer(), 255, 255, 255, 255);
		SDL_RenderDrawRectF(Graphics::Instance()->Get_Renderer(), &test);
		*/
	}
}

void Level::UpdateLevelObjects(float timestep)
{

}

void Level::AddStaticRect(const Vector2D& pos, const Vector2D& size, CollisionLayer layer)
{

}

// Old function I used to spawn some collectibles around during testing.
// Kept it in in case some of the logic is helpful to refer to.
/*
void Level::SpawnRandomCollectibles(int amount, int tilemapLayer)
{
	Tilemap* tilemap = this->GetTilemap(tilemapLayer);

	// Caching these to save resources.
	int accountForTileSize = Graphics::TILE_SIZE * Graphics::SPRITE_SCALE;
	int maxX = tilemap->GetWorldMax_X() - accountForTileSize;
	int maxY = tilemap->GetWorldMax_Y() - accountForTileSize;
	int minX = tilemap->GetWorldMin_X() + accountForTileSize;
	int minY = tilemap->GetWorldMin_Y() + accountForTileSize;

	//std::cout << maxX << "\n" << maxY << "\n" << minX << "\n" << minY << "\n\n";
	int randMax_x = maxX - minX + 1;
	int randMax_y = maxY - minY + 1;

	for (int i = 0; i < amount / 2; i++)
	{
		int rand_x = (rand() % randMax_x) + minX;
		int rand_y = (rand() % randMax_y) + minY;

		//std::cout << rand_x << " " << rand_y << "\n";

		std::unique_ptr<Collectible> collectible =
			std::make_unique<Collectible>("TestCollectible.png", rand_x, rand_y, 11, 13, Vector2D(1.0, 1.0), ColliderShape::Box);
		collectible->ObjInit();

		collectibles.push_back(std::move(collectible));
	}

	for (int i = 0; i < amount / 2; i++)
	{
		int rand_x = (rand() % randMax_x) + minX;
		int rand_y = (rand() % randMax_y) + minY;

		//std::cout << rand_x << " " << rand_y << "\n";

		std::unique_ptr<Collectible> collectible =
			std::make_unique<Collectible>("TestCollectibleGreen.png", rand_x, rand_y, 11, 13, Vector2D(1.0, 1.0), ColliderShape::Circle);
		collectible->ObjInit();

		collectibles.push_back(std::move(collectible));
	}
}
*/

Tilemap* Level::GetTilemap(int layer)
{
	return tilemaps[layer].get();
}