#if !defined __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "renderer/Renderable.h"

namespace energonsoftware {

class Material;
class Shader;
class Texture;

class ResourceManager
{
public:
    static const std::string DEFAULT_DETAIL_TEXTURE;
    static const std::string DEFAULT_NORMALMAP_TEXTURE;

private:
    class Resource
    {
    public:
        explicit Resource(const boost::filesystem::path& path) : _path(path) { }
        virtual ~Resource() throw() { }

    public:
        const boost::filesystem::path& path() const { return _path; }
        virtual bool loaded() const = 0;
        virtual bool load(MemoryAllocator* const allocator) = 0;
        virtual void unload() = 0;

    private:
        boost::filesystem::path _path;

    private:
        Resource();
    };

    class FontResource : public Resource
    {
    public:
        static void destroy(FontResource* const resource, MemoryAllocator* const allocator);

    public:
        explicit FontResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~FontResource() throw() { }

    public:
        virtual bool loaded() const { return true; }
        virtual bool load(MemoryAllocator* const allocator) { return true; }
        virtual void unload() { }

    private:
        FontResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<FontResource> > FontResourceMap;

    class LightResource : public Resource
    {
    public:
        static void destroy(LightResource* const resource, MemoryAllocator* const allocator);

    public:
        explicit LightResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~LightResource() throw() { }

    public:
        virtual bool loaded() const { return true; }
        virtual bool load(MemoryAllocator* const allocator) { return true; }
        virtual void unload() { }

    private:
        LightResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<LightResource> > LightResourceMap;

    class MapResource : public Resource
    {
    public:
        static void destroy(MapResource* const resource, MemoryAllocator* const allocator);

    public:
        explicit MapResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~MapResource() throw() { }

    public:
        virtual bool loaded() const { return true; }
        virtual bool load(MemoryAllocator* const allocator) { return true; }
        virtual void unload() { }

    private:
        MapResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<MapResource> > MapResourceMap;

    class MaterialResource : public Resource
    {
    public:
        static void destroy(MaterialResource* const resource, MemoryAllocator* const allocator);

    public:
        explicit MaterialResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~MaterialResource() throw() { }

    public:
        boost::shared_ptr<Material> material() const { return _material; }
        virtual bool loaded() const { return static_cast<bool>(_material); }
        virtual bool load(MemoryAllocator* const allocator);
        virtual void unload() { _material.reset(); }

    private:
        boost::shared_ptr<Material> _material;

    private:
        MaterialResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<MaterialResource> > MaterialResourceMap;

    class AnimationResource : public Resource
    {
    public:
        static void destroy(AnimationResource* const resource, MemoryAllocator* const allocator);

    public:
        explicit AnimationResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~AnimationResource() throw() { }

    public:
        virtual bool loaded() const { return true; }
        virtual bool load(MemoryAllocator* const allocator) { return true; }
        virtual void unload() { }

    private:
        AnimationResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<AnimationResource> > AnimationResourceMap;

    class ModelResource : public Resource
    {
    public:
        static void destroy(ModelResource* const resource, MemoryAllocator* const allocator);

    public:
        explicit ModelResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~ModelResource() throw() { }

    public:
        virtual bool loaded() const { return true; }
        virtual bool load(MemoryAllocator* const allocator) { return true; }
        virtual void unload() { }

    private:
        ModelResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<ModelResource> > ModelResourceMap;

    class SceneResource : public Resource
    {
    public:
        static void destroy(SceneResource* const resource, MemoryAllocator* const allocator);

    public:
        explicit SceneResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~SceneResource() throw() { }

    public:
        virtual bool loaded() const { return true; }
        virtual bool load(MemoryAllocator* const allocator) { return true; }
        virtual void unload() { }

    private:
        SceneResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<SceneResource> > SceneResourceMap;

    class ShaderResource : public Resource
    {
    public:
        static void destroy(ShaderResource* const resource, MemoryAllocator* const allocator);

    public:
        explicit ShaderResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~ShaderResource() throw() { }

    public:
        boost::shared_ptr<Shader> shader() const { return _shader; }
        virtual bool loaded() const { return static_cast<bool>(_shader); }
        virtual bool load(MemoryAllocator* const allocator);
        virtual void unload() { _shader.reset(); }

    private:
        boost::shared_ptr<Shader> _shader;

    private:
        ShaderResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<ShaderResource> > ShaderResourceMap;

    // TODO: texture data is held in RAM way too long,
    // we should load/unload it as necessary better
    class TextureResource : public Resource
    {
    public:
        static void destroy(TextureResource* const resource, MemoryAllocator* const allocator);

    public:
        TextureResource(const boost::filesystem::path& path) : Resource(path) { }
        virtual ~TextureResource() throw() { }

    public:
        boost::shared_ptr<Texture> texture() const { return _texture; }
        virtual bool loaded() const { return static_cast<bool>(_texture); }
        virtual bool load(MemoryAllocator* const allocator);
        virtual void unload() { _texture.reset(); }

    private:
        boost::shared_ptr<Texture> _texture;

    private:
        TextureResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<TextureResource> > TextureResourceMap;

    class TextureImageResource : public Resource
    {
    public:
        static void destroy(TextureImageResource* const resource, MemoryAllocator* const allocator);

    public:
        // NOTE: texture and material must *not* be null, they are *not* checked!
        TextureImageResource(boost::shared_ptr<TextureResource> texture, boost::shared_ptr<Material> material)
            : Resource(texture->path()), _texture(texture->texture()), _material(material), _loaded(false), _texid(0) { }
        virtual ~TextureImageResource() throw() { }

    public:
        void texture(GLuint texture) { _texid = texture; }
        GLuint texture() const { return _texid; }

        bool ready() const { return 0 != _texid; }
        virtual bool loaded() const { return _loaded; }
        virtual bool load(MemoryAllocator* const allocator);

        // NOTE: this free's the OpenGL texture
        // so that will need to be re-allocated
        // if the TextureImage is to be re-used
        virtual void unload();

    private:
        boost::shared_ptr<Texture> _texture;
        boost::shared_ptr<Material> _material;

        bool _loaded;
        GLuint _texid;

    private:
        TextureImageResource();
    };
    typedef boost::unordered_map<std::string, boost::shared_ptr<TextureImageResource> > TextureImageResourceMap;

public:
    static void destroy(ResourceManager* const manager, MemoryAllocator* const allocator);

private:
    static std::string path_to_key(const boost::filesystem::path& path, const boost::filesystem::path& dir);
    static std::string texture_image_resource(boost::shared_ptr<Material> material, Renderable::TextureBuffers::TextureBuffer texture);

private:
    static Logger& logger;

public:
    virtual ~ResourceManager() throw();

public:
    void dump() const;

    boost::filesystem::path font_path(const std::string& name);
    boost::shared_ptr<Material> material(const std::string& name);
    boost::shared_ptr<Shader> shader(const std::string& name);
    GLuint texture(boost::shared_ptr<Material> material, Renderable::TextureBuffers::TextureBuffer texture);

    // NOTE: msut be called from the render thread!
    // TODO: this should *not* need to be called from the render thread! make a new render command
    bool save_texture_image(boost::shared_ptr<Material> material, Renderable::TextureBuffers::TextureBuffer texture, size_t width, size_t height, size_t Bpp);

private:
    friend class Engine;
    bool init();
    void shutdown();

    // NOTE: these must be called from the render thread
    void load_shaders();
    void load_textures();

private:
    // NOTE: these do *not* lock the manager
    bool init_fonts();
    void add_font_resource(const boost::filesystem::path& path);

    bool init_lights();

    bool init_maps();

    bool init_materials();
    void add_material_resource(const boost::filesystem::path& path);

    bool init_animations();
    void add_animation_resource(const boost::filesystem::path& path);

    bool init_models();
    void add_model_resource(const boost::filesystem::path& path);

    bool init_scenes();

    bool init_shaders();
    void add_shader_resource(const boost::filesystem::path& path);

    bool init_textures();
    void add_texture_resource(const boost::filesystem::path& path, bool add_basename=true);

private:
    friend class Mesh;

    // these can be called from any thread
    void preload_material_textures(boost::shared_ptr<Material> material);
    void texture_buffer_callback(boost::shared_ptr<TextureResource> texture, boost::shared_ptr<Material> material, Renderable::TextureBuffers::TextureBuffer type, GLuint texid);

private:
    boost::recursive_mutex _mutex;

    FontResourceMap _fonts;
    LightResourceMap _lights;
    MapResourceMap _maps;
    MaterialResourceMap _materials;
    AnimationResourceMap _animations;
    ModelResourceMap _models;
    SceneResourceMap _scenes;
    ShaderResourceMap _shaders;
    TextureResourceMap _textures;
    TextureImageResourceMap _texture_images;

private:
    ResourceManager();
    DISALLOW_COPY_AND_ASSIGN(ResourceManager);
};

}

#endif
