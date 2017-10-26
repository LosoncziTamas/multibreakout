#ifndef FontButton_hpp
#define FontButton_hpp

#include <string>

struct Renderer;
struct GameInput;

struct FontButton {
    std::string text;
    int x;
    int y;
    void (*onclick)(void);
};

#endif
