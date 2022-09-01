#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char key_event[64];
  if (!NDL_PollEvent(key_event, 64)) {
    return 0;
  }

  if (key_event[1] == 'u') {
    ev->type = SDL_KEYUP;
  } else {
    ev->type = SDL_KEYDOWN;
  }

  for (int i = 0; i <= SDLK_PAGEDOWN; i++)  {
    if (!strcmp(key_event + 3, keyname[i])) {
      ev->key.keysym.sym = i;
      return 1;
    }
  }
  assert(NULL);
}

int SDL_WaitEvent(SDL_Event *event) {
  char key_event[64];
  while (!NDL_PollEvent(key_event, 64));

  if (key_event[1] == 'u') {
    event->type = SDL_KEYUP;
  } else {
    event->type = SDL_KEYDOWN;
  }

  for (int i = 0; i <= SDLK_PAGEDOWN; i++)  {
    if (!strcmp(key_event + 3, keyname[i])) {
      event->key.keysym.sym = i;
      return 1;
    }
  }
  assert(NULL);
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
