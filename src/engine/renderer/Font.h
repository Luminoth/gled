#if !defined __FONT_H__
#define __FONT_H__

#include "src/core/math/Vector.h"

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

class Camera;

namespace energonsoftware {

// TODO: this doesn't handle non-printable characters
class Font
{
private:
    enum FontVBO
    {
        VertexArray,
        TextureArray,
        VBOCount
    };

private:
    static Logger& logger;
    static FT_Library freetype;
    static GLuint texture;
    static GLuint vbo[VBOCount];

private:
    // NOTE: must be called *after* a GL context is available
    friend class ResourceManager;
    static bool init();
    static void shutdown();

public:
    Font();
    virtual ~Font() throw();

public:
    const Color& color() const { return _color; }
    void color(const Color& color) { _color = color; }

    bool load(const std::string& name, size_t height);
    void render(const std::string& text, const Position& position=Position(), const Vector2& scale=Vector2(1.0f, 1.0f), bool center=false) const;

    std::string str() const;

private:
    void calculate_width_height(const std::string& text, float& width, float& height) const;

private:
    FT_Face _face;
    Color _color;

private:
    DISALLOW_COPY_AND_ASSIGN(Font);
};

}

#endif
