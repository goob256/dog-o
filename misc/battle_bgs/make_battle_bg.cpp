#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main(int argc, char **argv)
{
	int s = atoi(argv[1]);
	srand(s);

	al_init();
	al_init_image_addon();

	ALLEGRO_DISPLAY *display = al_create_display(320, 180);

	ALLEGRO_BITMAP *tile1 = al_load_bitmap("1.png");
	ALLEGRO_BITMAP *tile2 = al_load_bitmap("2.png");
	ALLEGRO_BITMAP *tile3 = al_load_bitmap("3.png");

	float max = 0;
	ALLEGRO_COLOR darkest;

	for (int y = 0; y < al_get_bitmap_height(tile1); y++) {
		for (int x = 0; x < al_get_bitmap_width(tile1); x++) {
			ALLEGRO_COLOR c = al_get_pixel(tile1, x, y);
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
			int r = rand() % 3;
			if (r == 0) {
				al_draw_bitmap(tile1, x, y, 0);
			}
			else if (r == 1) {
				al_draw_bitmap(tile2, x, y, 0);
			}
			else if (r == 2) {
				al_draw_bitmap(tile3, x, y, 0);
			}
			x += al_get_bitmap_width(tile1);
		}
		y += al_get_bitmap_height(tile1);
		x = 0;
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

	al_save_bitmap("bg.tga", al_get_backbuffer(display));
}
