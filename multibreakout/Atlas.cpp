#include <string>
#include <cstdlib>
#include <SDL2/SDL_assert.h>

#include "Renderer.hpp"
#include "Atlas.hpp"
#include "jsmn.h"

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int countFrames(jsmntok_t *tokens, int num_of_tokens, const char *json_str)
{
    int ret = 0;
    for (int i = 1; i < num_of_tokens; i++)
    {
        if (jsoneq(json_str, &tokens[i], "frame") == 0)
        {
            ++ret;
        }
    }
    return ret;
}

int tokenToInt(jsmntok_t* token, const char* json_str)
{
    char *tmp = strndup(json_str + token->start, token->end - token->start);
    int ret = atoi(tmp);
    free(tmp);
    
    return ret;
}

void initAtlas(Atlas& atlas, std::string& json, const Renderer& renderer) {
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
    
    if (ret == JSMN_ERROR_INVAL)
    {
        printf("Invalid JSON string.");
    }
    if (ret == JSMN_ERROR_PART)
    {
        printf("Truncated JSON string.");
    }
    
    SDL_assert(tokens[0].type == JSMN_OBJECT);
    
    int framesSize = countFrames(tokens, ret, json_c);
    
    for (int i = 1; i < ret; i++)
    {
        if (jsoneq(json_c, &tokens[i], "filename") == 0)
        {
            if(jsoneq(json_c, &tokens[i+2], "frame") == 0)
            {
                int x = tokenToInt(&tokens[i+5], json_c);
                int y = tokenToInt(&tokens[i+7], json_c);
                int w = tokenToInt(&tokens[i+9], json_c);
                int h = tokenToInt(&tokens[i+11], json_c);
                
                SDL_Rect src_rect = {x, y, w, h};
                atlas.frames.push_back(src_rect);
                i += 11;
            }
            else
            {
                printf("Unexpected key.");
                SDL_assert(false);
            }
        }
        
        if (jsoneq(json_c, &tokens[i], "image") == 0)
        {
            char* texturePath = strndup(json_c + tokens[i+1].start, tokens[i+1].end - tokens[i+1].start);
            atlas.texture = createTexture(texturePath, renderer);
            free(texturePath);
        }
    }
    
    free(tokens);
}