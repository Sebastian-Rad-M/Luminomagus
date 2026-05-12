#include "RaylibRenderInterface.h"
#include <raylib.h>
#include <rlgl.h>

void RaylibRenderInterface::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, 
                                           Rml::TextureHandle texture, const Rml::Vector2f& translation) {
   (void)num_vertices;
    rlPushMatrix();
    rlTranslatef(translation.x, translation.y, 0.0f);

    // ADD THIS: Apply the RmlUi transform matrix if one exists for this element
    if (has_transform) {
        rlMultMatrixf(transform_matrix.data());
    }

    // 1. Bind the texture (if RmlUi gave us one)
    Texture2D* tex = (Texture2D*)texture;
    if (tex != nullptr) {
        rlSetTexture(tex->id);
    } else {
        rlSetTexture(rlGetTextureIdDefault()); // Draw solid colors
    }
    rlBegin(RL_TRIANGLES);
    for (int i = 0; i < num_indices; i++) {
        int index = indices[i];
        Rml::Vertex& v = vertices[index];
        
        rlColor4ub(v.colour.red, v.colour.green, v.colour.blue, v.colour.alpha);
        rlTexCoord2f(v.tex_coord.x, v.tex_coord.y);
        rlVertex2f(v.position.x, v.position.y);
    }
    rlEnd();

    rlSetTexture(0); // Unbind
    rlPopMatrix();
}

void RaylibRenderInterface::EnableScissorRegion(bool enable) {
    if (!enable) EndScissorMode();
}

void RaylibRenderInterface::SetScissorRegion(int x, int y, int width, int height) {
    BeginScissorMode(x, y, width, height);
}

bool RaylibRenderInterface::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) {
    Texture2D tex = ::LoadTexture(source.c_str());
    if (tex.id == 0) return false;

    Texture2D* allocated_tex = new Texture2D(tex);
    texture_handle = (Rml::TextureHandle)allocated_tex;
    texture_dimensions.x = tex.width;
    texture_dimensions.y = tex.height;
    return true;
}

bool RaylibRenderInterface::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) {
        Image img; 
        img.data = (void*)source; 
        img.width = source_dimensions.x;
        img.height = source_dimensions.y;
        img.mipmaps = 1;
        img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        
        Texture2D tex = LoadTextureFromImage(img);
        if (tex.id == 0) return false;

        Texture2D* allocated_tex = new Texture2D(tex);
        texture_handle = (Rml::TextureHandle)allocated_tex;
        return true;
    }

void RaylibRenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle) {
    Texture2D* tex = (Texture2D*)texture_handle;
    UnloadTexture(*tex);
    delete tex;
}

void RaylibRenderInterface::SetTransform(const Rml::Matrix4f* transform) {
    if (transform) {
        transform_matrix = *transform;
        has_transform = true;
    } else {
        has_transform = false;
    }
}