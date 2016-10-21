#pragma once

#include "renderinterface.h"

#include <cstdint>
#include <list>
#include <map>
#include <SDL.h>
#include <unordered_map>

#define STBTT_DEF extern
#include "stb/stb_truetype.h"

struct RenderOp;
class RendererGL : public RenderInterface
{
public:
    RendererGL();
    virtual ~RendererGL();
    
    virtual void create_window(int w, int h, const char* title);
    virtual void destroy_window();
    virtual void begin();
    virtual void end();
    
    virtual bool load_data_file(const std::string& filename);

    virtual void draw_sprite(uint32_t id, float x, float y, int layer=0);
    virtual void draw_sprite_size(uint32_t id, float x, float y, float w, float h, int layer=0);
    virtual void draw_rectangle(const Color& color, int from_x, int from_y, int to_x, int to_y, bool fill = true, int layer=0);
    virtual void draw_line(const Color& color, int from_x, int from_y, int to_x, int to_y);
    virtual void draw_text(const std::string& text, uint32_t size, int from_x, int from_y, int layer = 0);

    virtual void load_sprites(const char* filename);
    virtual bool load_texture(const char* filename);
    virtual void load_font(const char*filename, const std::vector<uint32_t>& sizes);
    virtual void free_sprites();
    virtual void free_fonts();

private:
    typedef struct
    {
        float x, y, w, h;
    } FloatRect;

    typedef struct
    {
        uint32_t tex;
        FloatRect size;
    } Sprite;

    typedef struct 
    {
        uint32_t handle;
        int width;
        int height;
        int channels;
    } Texture;
    
    uint32_t current_texture_;

    SDL_Window *window_;
    
    bool inFrame_;
    
    typedef struct
    {
        uint32_t texture;
        stbtt_bakedchar cdata[96]; 
    } FontData;
    
    std::map<std::string, Texture> textures_;
    std::unordered_map<uint32_t, Sprite> sprites_;
    std::map<uint32_t, FontData*> fonts_;

    std::list<RenderOp> render_ops_;

    void store_render_op(RenderOp& op);

    float width_;
    float height_;
};