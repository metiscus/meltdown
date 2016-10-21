#pragma once

#include <cstdint>
#include <vector>
#include "color.h"
#include <string>


class RenderInterface
{
public:
    RenderInterface() = default;
    virtual ~RenderInterface() = default;
    
    virtual void create_window(int w, int h, const char* title) = 0;
    virtual void destroy_window() = 0;
    virtual void begin() = 0;
    virtual void end() = 0;

	virtual bool load_data_file(const std::string& filename) = 0;

    virtual void draw_sprite(uint32_t id, float x, float y, int layer=0) = 0;
    virtual void draw_sprite_size(uint32_t id, float x, float y, float w, float h, int layer=0) = 0;
    virtual void draw_rectangle(const Color& color, int from_x, int from_y, int to_x, int to_y, bool fill = true, int layer=0) = 0;
    virtual void draw_line(const Color& color, int from_x, int from_y, int to_x, int to_y) = 0;
    virtual void draw_text(const std::string& text, uint32_t size, int from_x, int from_y, int layer = 0) = 0;

    virtual void load_sprites(const char* filename) = 0;
    virtual bool load_texture(const char* filename) = 0;
    virtual void load_font(const char*filename, const std::vector<uint32_t>& sizes) = 0;
    virtual void free_sprites() = 0;
    virtual void free_fonts() = 0;

};