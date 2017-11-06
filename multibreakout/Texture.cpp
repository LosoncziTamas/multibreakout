#include "Texture.hpp"
#include "Renderer.hpp"
#include "jsmn.h"

void readFile(const char* path, std::string& content) {
    char *fileContent = NULL;
    SDL_RWops *file = SDL_RWFromFile(path, "r");
    if (file) {
        Sint64 size = SDL_RWsize(file);
        fileContent = (char*) malloc(size + 1);
        if(SDL_RWread(file, fileContent, sizeof(char), size)) {
            fileContent[size] = '\0';
            content.assign(fileContent);
        } else {
            printf("Error: Unable to read file! SDL Error: %s\n", SDL_GetError());
            free(fileContent);
        }
        free(fileContent);
        SDL_RWclose(file);
    }
    else {
        printf("Error: Unable to open file! SDL Error: %s\n", SDL_GetError());
    }
}

SDL_Texture *createTexture(const char *path, SDL_Renderer* renderer) {
    SDL_Surface *surface = IMG_Load(path);
    SDL_assert(surface);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_assert(texture);
    SDL_FreeSurface(surface);
    return texture;
}

int getBallTexture(PowerUp powerUp) {
    switch (powerUp) {
        case speedUp:
            return BLUE_BALL;
        case slowDown:
            return BROWN_BALL;
        case strech:
            return GREEN_BALL;
        case shrink:
            return PURPLE_BALL;
        case neutral:
        default:
            return GREY_BALL;
    }
}

int getBrickTexture(PowerUp powerUp) {
    switch (powerUp) {
        case speedUp:
            return BLUE_BRICK;
        case slowDown:
            return BROWN_BRICK;
        case strech:
            return GREEN_BRICK;
        case shrink:
            return PURPLE_BRICK;
        case neutral:
        default:
            return YELLOW_BRICK;
    }
}

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int countFrames(jsmntok_t *tokens, int numOfTokens, const char *jsonStr) {
    int ret = 0;
    for (int i = 1; i < numOfTokens; i++) {
        if (jsoneq(jsonStr, &tokens[i], "frame") == 0) {
            ++ret;
        }
    }
    return ret;
}

int tokenToInt(jsmntok_t* token, const char* jsonStr) {
    char *tmp = strndup(jsonStr + token->start, token->end - token->start);
    int ret = atoi(tmp);
    free(tmp);
    
    return ret;
}

void initTextures(SDL_Renderer* renderer, Atlas& atlas, World& world) {
    std::string json;
    readFile("assets/texture_atlas.json", json);
    int bufferSize = 512;
    
    const char* json_c = json.c_str();
    
    jsmn_parser parser;
    jsmn_init(&parser);
    jsmntok_t *tokens = (jsmntok_t*)malloc(sizeof(jsmntok_t) * bufferSize);
    
    int ret = jsmn_parse(&parser, json_c, json.length(), tokens, bufferSize);
    while (ret == JSMN_ERROR_NOMEM) {
        bufferSize = bufferSize * 2 + 1;
        tokens = (jsmntok_t*) realloc(tokens, sizeof(jsmntok_t) * bufferSize);
        ret = jsmn_parse(&parser, json_c, json.length(), tokens, bufferSize);
    }
    
    if (ret == JSMN_ERROR_INVAL) {
        printf("Invalid JSON string.");
    }
    if (ret == JSMN_ERROR_PART) {
        printf("Truncated JSON string.");
    }
    
    SDL_assert(tokens[0].type == JSMN_OBJECT);
    
    //TODO: use it for animations
#if 0
    int framesSize = countFrames(tokens, ret, json_c);
#endif
    
    for (int i = 1; i < ret; i++) {
        if (jsoneq(json_c, &tokens[i], "filename") == 0) {
            if(jsoneq(json_c, &tokens[i+2], "frame") == 0) {
                int x = tokenToInt(&tokens[i+5], json_c);
                int y = tokenToInt(&tokens[i+7], json_c);
                int w = tokenToInt(&tokens[i+9], json_c);
                int h = tokenToInt(&tokens[i+11], json_c);
                
                SDL_Rect src_rect = {x, y, w, h};
                atlas.frames.push_back(src_rect);
                i += 11;
            } else {
                printf("Unexpected key.");
                SDL_assert(false);
            }
        }
        
        if (jsoneq(json_c, &tokens[i], "image") == 0) {
            char* texturePath = strndup(json_c + tokens[i+1].start, tokens[i+1].end - tokens[i+1].start);
            atlas.texture = createTexture(texturePath, renderer);
            free(texturePath);
        }
    }
    
    free(tokens);
}