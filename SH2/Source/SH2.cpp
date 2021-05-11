#include <iostream>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

//#include "pugixml.hpp"

#define WW 1280
#define WH 720
#define DEG 3.14/180	//0.0175f
#define MAX 100

/*
	0	[000000]	16	[010000]	32	[100000]	48	[110000]
	1	[000001]	17	[010001]	33	[100001]	49	[110001]
	2	[000010]	18	[010010]	34	[100010]	50	[110010]
	3	[000011]	19	[010011]	35	[100011]	51	[110011]
	4	[000100]	20	[010100]	36	[100100]	52	[110100]
	5	[000101]	21	[010101]	37	[100101]	53	[110101]
	6	[000110]	22	[010110]	38	[100110]	54	[110110]
	7	[000111]	23	[010111]	39	[100111]	55	[110111]
	8	[001000]	24	[011000]	40	[101000]	56	[111000]
	9	[001001]	25	[011001]	41	[101001]	57	[111001]
	10	[001010]	26	[011010]	42	[101010]	58	[111010]
	11	[001011]	27	[011011]	43	[101011]	59	[111011]
	12	[001100]	28	[011100]	44	[101100]	60	[111100]
	13	[001101]	29	[011101]	45	[101101]	61	[111101]
	14	[001110]	30	[011110]	46	[101110]	62	[111110]
	15	[001111]	31	[011111]	47	[101111]	63	[111111]
*/
class SH2
{
public:
	struct Hex { int r, b; } **hexagons = nullptr;

	float player_angle = 270.0f;
	float hexagon_angle = 0.0f;
	float angle_speed = 0.0f;
	float radius_speed = 1.0f;

	SH2()
	{
		hexagons = new Hex * [MAX];
		for (unsigned int i = 0; i < MAX; ++i) hexagons[i] = nullptr;

		hexagons[0] = new Hex{ WH / 14, 63 };
	}
	~SH2()
	{
		delete[] hexagons;
	}
	void Update(float dt, int* mouse, int* keyboard)
	{
		//system("cls");
		unsigned int count = 0;
		for (unsigned int i = 1; i < MAX; ++i)
		{
			if (hexagons[i]) ++count;
		}
		printf("p_a %.1f | h_a %.2f | a_s %.2f | r_s %.2f | dt %.3f | count %d\n", player_angle, hexagon_angle, angle_speed, radius_speed, dt, count);

		if (keyboard[SDL_SCANCODE_SPACE] == 2)
		{
			for (unsigned int i = 1; i < MAX; ++i)
				if (!hexagons[i])
				{
					hexagons[i] = new Hex{ WW / 2, rand() % 64 };
					break;
				}
		}

		if (mouse[1 + SDL_BUTTON_LEFT]) player_angle -= 300.0f * dt;
		if (mouse[1 + SDL_BUTTON_RIGHT]) player_angle += 300.0f * dt;

		for (unsigned int i = 1; i < MAX; i++)
		{
			if (hexagons[i])
			{
				hexagons[i]->r -= radius_speed;
				bool outOfBounds = !(hexagons[i]->r + 50 > hexagons[0]->r && hexagons[i]->r < hexagons[0]->r + (WW / 2));
				bool playerCollisionRadius = ((hexagons[0]->r + 32) > hexagons[i]->r && (hexagons[0]->r + 32) < hexagons[i]->r + 50);
				bool playerCollisionAngle = (hexagons[i]->b >> int(player_angle / 60) & 1);
				if (outOfBounds || (playerCollisionRadius && playerCollisionAngle))
				{
					delete hexagons[i];
					hexagons[i] = nullptr;
				}
			}
		}

		hexagon_angle += angle_speed * dt;
		if (hexagon_angle >= 360.0f || hexagon_angle <= -360.0f) hexagon_angle = 0.0f;
		if (player_angle >= 360.0f || player_angle <= -360.0f) player_angle = 0.0f;
	}
	void Draw(SDL_Renderer* renderer)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		SDL_RenderDrawLine(renderer, (WW / 2), (WH / 2), WW / 2 + (hexagons[0]->r + 32) * cosf(DEG * (0.0f + player_angle + hexagon_angle)), WH / 2 + (hexagons[0]->r + 32) * sinf(DEG * (0.0f + player_angle + hexagon_angle)));
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		for (unsigned int i = 0; i < MAX; ++i)
		{
			if (hexagons[i])
			{
				for (unsigned int ii = 0, n = hexagons[i]->b; ii < 6; ++ii, n /= 2)
				{
					if (bool(n % 2))
					{
						SDL_Point lp[4];
						int k = (i == 0) ? (WW / 2) : 50;
						for (unsigned int iii = 0; iii < 4; ++iii)
						{
							lp[iii].x = int(WW / 2 + (hexagons[i]->r + k * (iii / 2)) * cosf(float(DEG) * (60.0f * (ii + (iii % 2)) + hexagon_angle)));
							lp[iii].y = int(WH / 2 + (hexagons[i]->r + k * (iii / 2)) * sinf(float(DEG) * (60.0f * (ii + (iii % 2)) + hexagon_angle)));
						}
						//lp[iii].x *= (lp[iii].x < WW / 2) ? (1 / cosf(DEG * x)) : cosf(DEG * x);
						//lp[iii].y *= (lp[iii].y < WW / 2) ? (1 / cosf(DEG * y)) : cosf(DEG * y);
						SDL_RenderDrawLine(renderer, lp[0].x, lp[0].y, lp[1].x, lp[1].y);
						SDL_RenderDrawLine(renderer, lp[2].x, lp[2].y, lp[3].x, lp[3].y);
						SDL_RenderDrawLine(renderer, lp[0].x, lp[0].y, lp[2].x, lp[2].y);
						SDL_RenderDrawLine(renderer, lp[1].x, lp[1].y, lp[3].x, lp[3].y);
					}
				}
			}
		}
	}
};

class Timer
{
public:
	unsigned __int32 startTime;
	Timer() { Start(); }
	void Start() { startTime = SDL_GetTicks(); }
	unsigned __int32 ReadMs() const { return SDL_GetTicks() - startTime; }
	float ReadS() const { return float(SDL_GetTicks() - startTime) / 1000.0f; }
};

void DrawFont(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const char* text, float size = 1.0f, int style = 0, SDL_Color fg = { 0,0,0,255 }, SDL_Color bg = { 255,255,255,255 })
{
	SDL_Surface* surface = nullptr;
	SDL_Texture* texture = nullptr;
	switch (style)
	{
	case 0: surface = TTF_RenderText_Solid(font, text, fg); texture = SDL_CreateTextureFromSurface(renderer, surface); break;
	case 1: surface = TTF_RenderText_Shaded(font, text, fg, bg); texture = SDL_CreateTextureFromSurface(renderer, surface); break;
	case 2: surface = TTF_RenderText_Blended(font, text, fg); texture = SDL_CreateTextureFromSurface(renderer, surface); break;
	}
	SDL_Rect rect{ x,y,0,0 };
	SDL_QueryTexture(texture, 0, 0, &rect.w, &rect.h);
	rect.w *= size;
	rect.h *= size;
	SDL_RenderCopy(renderer, texture, 0, &rect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);

}

char* GetText(const char* _Format, ...)
{
	char _Buffer[4096];
	va_list _ArgList;
	va_start(_ArgList, _Format);
	vsprintf_s(_Buffer, 4096, _Format, _ArgList);
	va_end(_ArgList);
	return _Buffer;
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_FLAC);
	TTF_Init();

	SDL_Window* window = SDL_CreateWindow("SH2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WW, WH, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	srand(time(0));

	int* mouse = (int*)calloc(5, sizeof(int));
	int* keyboard = (int*)calloc(200, sizeof(int));
	float dt;
	float _dt = 0.016f;
	Timer timer;

	SH2* sh2 = new SH2;

	while(1)
	{
		int keyMap[2][4] = { 0,3,3,0,1,2,2,1 };

		const Uint8* keyboardState = SDL_GetKeyboardState(0);
		for (int i = 0; i < 200; ++i) keyboard[i] = keyMap[int(keyboardState[i])][keyboard[i]];

		Uint32 mouseState = SDL_GetMouseState(&mouse[0], &mouse[1]);
		for (int i = 2; i < 5; ++i) mouse[i] = keyMap[int((SDL_BUTTON(i - 1) & mouseState) > 0)][mouse[i]];

		SDL_Event event;
		while (SDL_PollEvent(&event))
			switch (event.type)
			{
			case SDL_QUIT:
				return false;
				break;
			case SDL_MOUSEWHEEL:
				if (event.wheel.y > 0);
				if (event.wheel.y < 0);
				break;
			}
		dt = timer.ReadS();
		timer.Start();
		if (dt < _dt) SDL_Delay((_dt - dt) * 1000);
		
		sh2->Update(dt, mouse, keyboard);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		sh2->Draw(renderer);

		SDL_RenderPresent(renderer);
	}

	delete sh2;
	sh2 = nullptr;

	free(mouse);
	free(keyboard);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}
