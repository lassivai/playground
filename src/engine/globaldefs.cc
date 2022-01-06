//#include "util.h"
//#include "mathl.h"
#include "sketch.h"
// TODO Find out if a better way exists...


uint64_t Xoroshiro128plus::s[2] = {1, 2};
uint64_t Xorshift128plus::s[2] = {1, 2};

const Vec2d Vec2d::Zero = {0, 0};
const Vec3d Vec3d::Zero = {0, 0, 0};
const Vec4d Vec4d::Zero = {0, 0, 0, 0};

int Instrument::instrumentCounter = 0;


int GuiElement::screenW = 1;
int GuiElement::screenH = 1;

SDLInterface *SDLInterface::sdlInterface = NULL;
SDLInterface *GuiElement::sdlInterface = NULL;
SDLInterface *Texture::sdlInterface = NULL;