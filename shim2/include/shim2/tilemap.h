#ifndef NOO_TILEMAP_H
#define NOO_TILEMAP_H

#include "shim2/main.h"

namespace noo {

namespace gfx {

class Image;

class SHIM2_EXPORT Tilemap
{
public:
	struct Animation_Data
	{
		util::Point<int> topleft;
		util::Size<int> size;
		int delay;
		std::vector< util::Point<int> > frames;
	};

	static float elapsed;

	static void static_start();
	static void release_sheets();
	static void reload_sheets(bool load_from_filesystem = false);
	static void update_all();
	static Image *get_sheet(int sheet);

	Tilemap(std::string map_filename, bool load_from_filesystem = false);
	~Tilemap();

	int get_num_layers();
	util::Size<int> get_size();

	// in tiles
	bool is_solid(int layer, util::Point<int> position);
	void set_solid(int layer, util::Point<int> position, util::Size<int> size, bool solid);
	// in pixels
	bool collides(int layer, util::Point<int> topleft, util::Point<int> bottomright);

	void draw(int start_layer, int end_layer, util::Point<float> position, bool clip_small_tilemaps = false);
	void draw(int layer, util::Point<float> position, bool clip_small_tilemaps = false);

	void add_animation_data(Animation_Data data);

	void swap_tiles(int layer1, int layer2, util::Point<int> topleft, util::Size<int> swap_size); // swap tiles between layers

	bool get_tile(int layer, util::Point<int> position, util::Point<int> &tile_xy, bool &solid); // returns false OOB, -1 x on nothing
	bool set_tile(int layer, util::Point<int> position, util::Point<int> tile_xy, bool solid); // returns false OOB

	void save(std::string filename);
	
	util::Point<int> get_animated_tile(util::Point<int> tile);

private:
	struct Layer
	{
		int **x;
		int **y;
	};
	bool **solid;

	Animation_Data *get_animation_data(util::Point<int> tile);
	bool has_alpha(int col, int row); // col/row in tilemap tile has any non-255 alpha pixels
	bool all_clear(int col, int row); // col/row in tilemap is entirely clear pixels (to be removed)

	util::Size<int> size; // in tiles
	int num_layers;

	Layer *layers;

	std::vector<Animation_Data> animation_data;
};

} // End namespace gfx

} // End namespace noo

#endif // NOO_TILEMAP_H
