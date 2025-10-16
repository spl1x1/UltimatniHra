#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int* gFrameBuffer;
SDL_Window* gSDLWindow;
SDL_Renderer* gSDLRenderer;
SDL_Texture* gSDLTexture;
static int gDone;
const int WINDOW_WIDTH = 1920 / 2;
const int WINDOW_HEIGHT = 1080 / 2;

bool update()
{
  SDL_Event e;
  if (SDL_PollEvent(&e))
  {
    if (e.type == SDL_EVENT_QUIT)
    {
      return false;
    }
    if (e.type == SDL_EVENT_KEY_UP && e.key.key == SDLK_ESCAPE)
    {
      return false;
    }
  }
  return true;
}


void loop()
{
  if (!update())
  {
    gDone = 1;
  }
}

int main(int argc, char** argv)
{
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
  {
    return -1;
  }

  gFrameBuffer = new int[WINDOW_WIDTH * WINDOW_HEIGHT];
  gSDLWindow = SDL_CreateWindow("SDL3 window", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  gSDLRenderer = SDL_CreateRenderer(gSDLWindow, NULL);
  gSDLTexture = SDL_CreateTexture(gSDLRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

  if ( !gSDLWindow || !gSDLRenderer || !gSDLTexture)
    return -1;

  gDone = 0;
  while (!gDone)
  {
    loop();
  }

  SDL_DestroyTexture(gSDLTexture);
  SDL_DestroyRenderer(gSDLRenderer);
  SDL_DestroyWindow(gSDLWindow);
  SDL_Quit();

  return 0;
}