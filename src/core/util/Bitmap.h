#if !defined __BITMAP_H__
#define __BITMAP_H__

#include "Texture.h"

namespace energonsoftware {

// TODO: this class is not implemented!

class Bitmap : public Texture
{
public:
    Bitmap();
    Bitmap(size_t width, size_t height, size_t bpp, boost::shared_array<unsigned char> pixels);
    virtual ~Bitmap() throw();

public:
    virtual size_t width() const { return _width; }
    virtual size_t height() const { return _height; }
    virtual size_t bpp() const { return _bpp; }

public:
    virtual bool load(const boost::filesystem::path& filename, MemoryAllocator& allocator);
    virtual void unload() throw();

    virtual bool save(const boost::filesystem::path& filename) const;

private:
    size_t _width, _height, _bpp;

private:
    DISALLOW_COPY_AND_ASSIGN(Bitmap);
};

}

#endif
