#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

typedef struct // Player object
{
	int x, y, dy;
	int facing_right;
	int won_game;
	int going_down;
	SDL_Texture *texture_R;
	SDL_Texture *texture_L;
	SDL_Texture *texture_R_down;
	SDL_Texture *texture_L_down;
	SDL_Texture *texture_win;
} Character;

Character player; // Declaration of player object

// Declare textures
SDL_Texture *background_texture;
SDL_Texture *rod_L_texture;
SDL_Texture *rod_R_texture;
SDL_Texture *mine_1_texture;
SDL_Texture *mine_2_texture;
SDL_Texture *shark_R_texture;
SDL_Texture *shark_L_texture;
SDL_Texture *krill_texture;

void render_game_and_collisions(SDL_Renderer *, Character *);

void return_to_origin(Character*);

void process_events(SDL_Window *, Character *, int *);

SDL_bool check_collision(SDL_Rect *, SDL_Rect *);

SDL_Texture *load_texture(const char *, SDL_Renderer *);

int main()
{
	// Initialize SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);

	// Create Window
	SDL_Window *window = SDL_CreateWindow("Wail", 300, 300, 1600, 900, SDL_WINDOW_SHOWN);

	// Create renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0)
	{
		printf(stderr, "Mix error: %s", Mix_GetError());
	}

	// Configure Music
	Mix_Music *background_music = Mix_LoadMUS("assets/background_music.mp3");
	Mix_Music *end_music = Mix_LoadMUS("assets/win_music.mp3");
	Mix_VolumeMusic(10);

	Mix_PlayMusic(background_music, -1);
	if (Mix_PlayMusic(background_music, -1) != 0)
	{
		printf("Play error: %s\n", Mix_GetError());
	}

	// Load textures
	player.texture_L = load_texture("assets/wailord_L.png", renderer);
	player.texture_R = load_texture("assets/wailord_R.png", renderer);
	player.texture_L_down = load_texture("assets/wailord_L_down.png", renderer);
	player.texture_R_down = load_texture("assets/wailord_R_down.png", renderer);
	player.texture_win = load_texture("assets/wailord_win.png", renderer);
	background_texture = load_texture("assets/background2.png", renderer);
	rod_L_texture = load_texture("assets/rod_L.png", renderer);
	rod_R_texture = load_texture("assets/rod_R.png", renderer);
	mine_1_texture = load_texture("assets/mine_1.png", renderer);
	mine_2_texture = load_texture("assets/mine_2.png", renderer);
	shark_R_texture = load_texture("assets/shark_R.png", renderer);
	shark_L_texture = load_texture("assets/shark_L.png", renderer);
	krill_texture = load_texture("assets/krill2.png", renderer);

	// Define initial player configuration
	player.x = 50;
	player.y = 10;
	player.facing_right = 1;
	player.won_game = 0;
	player.going_down = 1;

	int running = 1;
	while (running)
	{
		process_events(window, &player, &running);

		render_game_and_collisions(renderer, &player);

		player.going_down = 1;

		SDL_Delay(20);

		if (player.won_game)
		{
			Mix_HaltMusic();
			Mix_PlayMusic(end_music, -1);
			while (running)
			{
				process_events(window, &player, &running);
				render_game_and_collisions(renderer, &player);
				SDL_Delay(20);
			}
		}
	}

	// Destroy SDL & quit
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(player.texture_L);
	SDL_DestroyTexture(player.texture_R);
	SDL_DestroyTexture(player.texture_L_down);
	SDL_DestroyTexture(player.texture_R_down);
	SDL_DestroyTexture(player.texture_win);
	SDL_DestroyTexture(background_texture);
	SDL_DestroyTexture(rod_L_texture);
	SDL_DestroyTexture(rod_R_texture);
	SDL_DestroyTexture(mine_1_texture);
	SDL_DestroyTexture(mine_2_texture);
	SDL_DestroyTexture(shark_L_texture);
	SDL_DestroyTexture(shark_R_texture);
	SDL_DestroyTexture(krill_texture);

	Mix_Quit();
	SDL_Quit();
	return 0;
}

void render_game_and_collisions(SDL_Renderer *renderer, Character *player)
{
	/* Rects */
	SDL_Rect background_rect = {0, 0, 1600, 900};
	SDL_Rect side_rect = {1, 0, 1, 900};
	SDL_Rect border_rect = {0, 0, 1600, 1};
	SDL_Rect player_rect = {player->x, player->y, 128, 56};
	SDL_Rect rod_1_rect = {225, 0, 128, 384};
	SDL_Rect mine_1_rect = {375, 600, 98, 196};
	SDL_Rect shark_L_rect = {590, 250, 320, 220};
	SDL_Rect rod_2_rect = {1075, 0, 96, 288};
	SDL_Rect mine_2_rect = {1100, 460, 192, 384};
	SDL_Rect shark_R_rect = {1300, 150, 192, 132};
	SDL_Rect krill_rect = {1400, 450, 200, 125};

	// Rendering
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, background_texture, NULL, &background_rect); //background
	SDL_RenderCopy(renderer, NULL, NULL, &border_rect);					  // border
	SDL_RenderCopy(renderer, NULL, NULL, &side_rect);					  // left hand side

	// Which player model to load
	if (!player->won_game)
	{
		if (player->facing_right == 1 && player->going_down == 0)
			SDL_RenderCopy(renderer, player->texture_R, NULL, &player_rect);
		else if (player->facing_right == 1 && player->going_down == 1)
			SDL_RenderCopy(renderer, player->texture_R_down, NULL, &player_rect);
		else if (player->facing_right == 0 && player->going_down == 0)
			SDL_RenderCopy(renderer, player->texture_L, NULL, &player_rect);
		else if ((player->facing_right == 0 && player->going_down == 1))
			SDL_RenderCopy(renderer, player->texture_L_down, NULL, &player_rect);
	}
	else
		SDL_RenderCopy(renderer, player->texture_win, NULL, &player_rect);

	SDL_RenderCopy(renderer, rod_L_texture, NULL, &rod_1_rect);		// rod 1
	SDL_RenderCopy(renderer, mine_1_texture, NULL, &mine_1_rect);	// mine 1
	SDL_RenderCopy(renderer, shark_L_texture, NULL, &shark_L_rect); // shark_L
	SDL_RenderCopy(renderer, shark_R_texture, NULL, &shark_R_rect); // shark_R
	SDL_RenderCopy(renderer, rod_R_texture, NULL, &rod_2_rect);
	SDL_RenderCopy(renderer, mine_2_texture, NULL, &mine_2_rect); // mine 2
	SDL_RenderCopy(renderer, krill_texture, NULL, &krill_rect);

	SDL_RenderPresent(renderer);

	// Collision detection
	if (check_collision(&player_rect, &side_rect))
	{
		return_to_origin(player);
	}
	else if (check_collision(&player_rect, &border_rect))
	{
		return_to_origin(player);
	}
	else if (check_collision(&player_rect, &rod_1_rect))
	{
		return_to_origin(player);
	}
	else if (check_collision(&player_rect, &mine_1_rect))
	{
		return_to_origin(player);
	}
	else if (check_collision(&player_rect, &shark_L_rect))
	{
		return_to_origin(player);
	}
	else if (check_collision(&player_rect, &mine_2_rect))
	{
		return_to_origin(player);
	}
	else if (check_collision(&player_rect, &rod_2_rect))
	{
		return_to_origin(player);
	}
	else if (check_collision(&player_rect, &shark_R_rect))
	{
		return_to_origin(player);
	}

	else if (check_collision(&player_rect, &krill_rect))
	{
		player->won_game = 1;
	}
}

void return_to_origin(Character *player)
{
	player->x = 50;
	player->y = 10;
	player->facing_right = 1;
	SDL_Delay(50);
}

void process_events(SDL_Window *window, Character *player, int *p_running)
{
	SDL_Event event;

	// Handle miscellaneous events
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_WINDOWEVENT_CLOSE:
			*p_running = 0;
			SDL_DestroyWindow(window);
			window = NULL;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				*p_running = 0;
				break;
			}
		}
	}

	// Handle keyboard events
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_W])
	{
		player->dy = -8;
		player->going_down = 0;
	}
	if (keystate[SDL_SCANCODE_A])
	{
		player->x -= 5;
		player->facing_right = 0;
	}
	if (keystate[SDL_SCANCODE_D])
	{
		player->x += 5;
		player->facing_right = 1;
	}
	if (keystate[SDL_SCANCODE_C]) // Cheat button
	{
		player->x = 1300;
		player->y = 600;
	}

	// Gravity
	player->y += player->dy;
	player->dy += 1;
	if (player->y > 700)
	{
		player->y = 700;
		player->dy = 0;
	}
}

SDL_bool check_collision(SDL_Rect *player_rect, SDL_Rect *object_rect)
{
	SDL_bool collision = SDL_HasIntersection(player_rect, object_rect);
	if (collision)
		printf("Collision!\n");
	return collision;
}

SDL_Texture *load_texture(const char *filename, SDL_Renderer *renderer)
{
	SDL_Surface *temp_surface = IMG_Load(filename);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
	SDL_FreeSurface(temp_surface);

	return texture;
}
