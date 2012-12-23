#if !defined __TEXTURE_H__
#define __TEXTURE_H__

namespace energonsoftware {

class MemoryAllocator;

class Texture
{
public:
    enum TextureFormat
    {
        TextureFormatPNG,
        TextureFormatTGA,
        TextureFormatBMP,
    };

public:
    virtual ~Texture() throw();

public:
    boost::shared_array<unsigned char> pixels() { return _pixels; }
    const boost::shared_array<unsigned char> pixels() const { return _pixels; }

    virtual size_t width() const = 0;
    size_t pitch() const { return width() * Bpp(); }

    virtual size_t height() const = 0;

    virtual size_t bpp() const = 0;
    size_t Bpp() const { return bpp() >> 3; }

    size_t size() const { return width() * height() * Bpp(); }

public:
    virtual bool load(const boost::filesystem::path& filename, MemoryAllocator& allocator) = 0;
    virtual void unload() throw() = 0;

    void flip_vertical();

    virtual bool save(const boost::filesystem::path& filename) const = 0;

protected:
    void allocate(MemoryAllocator& allocator, size_t size);
    void release();

private:
    boost::shared_array<unsigned char> _pixels;

protected:
    Texture();
    explicit Texture(boost::shared_array<unsigned char> pixels);

private:
    DISALLOW_COPY_AND_ASSIGN(Texture);
};

}

#endif
