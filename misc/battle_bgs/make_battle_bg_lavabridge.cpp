#define ALLEGRO_STATICLINK

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <stdio.h>

int main(int argc, char **argv)
{
	srand(atoi(argv[1]));

	int lava_num = atoi(argv[2]);

	al_init();
	al_init_image_addon();

	ALLEGRO_DISPLAY *display = al_create_display(320, 180);

<<<<<<< Updated upstream:tools/battle_bgs/make_battle_bg_lavabridge.cpp
	ALLEGRO_BITMAP *tiles = al_load_bitmap("ground.png");

	ALLEGRO_BITMAP *lava[3];
	lava[0] = al_load_bitmap("lava1.png");
	lava[1] = al_load_bitmap("lava2.png");
	lava[2] = al_load_bitmap("lava3.png");
=======
	ALLEGRO_BITMAP *ground = al_load_bitmap("ground.png");
	ALLEGRO_BITMAP *lava = al_load_bitmap("lava.png");
>>>>>>> Stashed changes:tools/make_battle_bg_lavabridge.cpp

	float max = 0;
	ALLEGRO_COLOR darkest;

	for (int y = 0; y < al_get_bitmap_height(lava); y++) {
		for (int x = 0; x < al_get_bitmap_width(lava); x++) {
			ALLEGRO_COLOR c = al_get_pixel(lava, x, y);
			float tot = c.r + c.g + c.b;
			if (tot > max) {
				darkest = c;
			}
		}
	}

	ALLEGRO_BITMAP *tmp =  al_create_bitmap(4096, 4096);

	int x = 0;
	int y = 0;

	al_set_target_bitmap(tmp);

	while (y < 4096) {
		while (x < 4096) {
<<<<<<< Updated upstream:tools/battle_bgs/make_battle_bg_lavabridge.cpp
			int tx = rand() % 2;
			int ty = rand() % 2;
			al_draw_bitmap_region(tiles, tx*16, ty*16, 16, 16, x, y, 0);
=======
			/*
			if (rand() % 2 == 0) {
				al_draw_bitmap(tile1, x, y, 0);
			}
			else {
				al_draw_bitmap(tile2, x, y, 0);
			}
			*/
			int tx = rand() % 2;
			int ty = rand() % 2;
			al_draw_bitmap_region(ground, tx*16, ty*16, 16, 16, x, y, 0);
>>>>>>> Stashed changes:tools/make_battle_bg_lavabridge.cpp
			x += 16;
		}
		y += 16;
		x = 0;
	}

<<<<<<< Updated upstream:tools/battle_bgs/make_battle_bg_lavabridge.cpp
	for (int y = 0; y < 75; y++) {
=======

	for (int y = 74; y >= 0; y--) {
>>>>>>> Stashed changes:tools/make_battle_bg_lavabridge.cpp
		x = 0;
		int yy = 74 - y;
		int ty = yy % 3;
		if (ty == 0 && y != 74) {
			ty = 3;
		}
		ty = 3 - ty;
		while (x < 4096) {
			int xx = x / 16;
			int tx = xx % 3;
			al_draw_bitmap_region(lava, tx*16, ty*16, 16, 16, x, y);
			x += 16;
		}
	}

	int nh = 4096 / 16;

	int yo = 30;

	for (int y = 0; y < 10; y++) {
		x = 0;
		int yy = nh - yo;
		int yyy = 74 - y;
		int ty = yyy % 3;
		if (ty == 0 && y != 0) {
			ty = 3;
		}
		while (x < 4096) {
			int xx = x / 16;
			int tx = xx % 3;
			al_draw_bitmap_region(lava, tx*16, ty*16, 16, 16, x, y);
			x += 16;
		}
	}

	al_set_target_backbuffer(display);

	al_clear_to_color(darkest);

	ALLEGRO_TRANSFORM t;
	al_identity_transform(&t);
	al_perspective_transform(&t, -1, -1, 0.1f, 1, 1, 1000.0f);
	al_use_projection_transform(&t);

	ALLEGRO_TRANSFORM t2;
	al_identity_transform(&t2);
	al_translate_transform_3d(&t2, -2048, -3584, -10.0f);
	al_rotate_transform_3d(&t2, 1.0f, 0.0f, 0.0f, ALLEGRO_PI/32.0f);
	al_use_transform(&t2);

	al_draw_bitmap(tmp, 0, 0, 0);

	al_save_bitmap("bg.tga", al_get_backbuffer(display));}
