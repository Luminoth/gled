#if !defined __TARGA_H__
#define __TARGA_H__

#include "Texture.h"

namespace energonsoftware {

class Targa : public Texture
{
private:
    struct Header
    {
        uint8_t idlen;
        uint8_t cmtype;
        uint8_t type;
        uint8_t cmoffset1;
        uint8_t cmoffset2;
        uint8_t cmlen1;
        uint8_t cmlen2;
        uint8_t cmdepth;
        uint8_t xorigin1;
        uint8_t xorigin2;
        uint8_t yorigin1;
        uint8_t yorigin2;
        uint8_t width1;
        uint8_t width2;
        uint8_t height1;
        uint8_t height2;
        uint8_t bpp;
        uint8_t descriptor;
    };

    enum Type
    {
        NoImage        = 0,
        ColorMapped    = 1,
        RGB            = 2,
        Grayscale      = 3,
        RLEColorMapped = 9,
        RLERGB         = 10,
        RLEGrayscale   = 11
    };

public:
    static void destroy(Targa* const texture, MemoryAllocator* const allocator);

public:
    Targa();
    Targa(size_t width, size_t height, size_t bpp, boost::shared_array<unsigned char> pixels);
    virtual ~Targa() throw();

public:
    virtual size_t width() const { return _width; }
    virtual size_t height() const { return _height; }
    virtual size_t bpp() const { return _bpp; }

public:
    virtual bool load(const boost::filesystem::path& filename, MemoryAllocator& allocator);
    virtual void unload() throw();

    virtual bool save(const boost::filesystem::path& filename) const;

private:
    Header _header;
    size_t _width, _height, _bpp;

private:
    DISALLOW_COPY_AND_ASSIGN(Targa);
};

}

#endif
