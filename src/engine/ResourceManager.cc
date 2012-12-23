#include "src/pch.h"
#include "src/core/common.h"
#include "src/core/util/fs_util.h"
#include "src/core/util/PNG.h"
#include "src/core/util/Targa.h"
#include "Engine.h"
#include "State.h"
#include "renderer/Font.h"
#include "renderer/Material.h"
#include "renderer/Renderer.h"
#include "renderer/Shader.h"
#include "ResourceManager.h"

namespace energonsoftware {

void ResourceManager::FontResource::destroy(FontResource* const resource, MemoryAllocator* const allocator)
{
    resource->~FontResource();
    operator delete(resource, *allocator);
}

void ResourceManager::LightResource::destroy(LightResource* const resource, MemoryAllocator* const allocator)
{
    resource->~LightResource();
    operator delete(resource, *allocator);
}

void ResourceManager::MapResource::destroy(MapResource* const resource, MemoryAllocator* const allocator)
{
    resource->~MapResource();
    operator delete(resource, *allocator);
}

void ResourceManager::MaterialResource::destroy(MaterialResource* const resource, MemoryAllocator* const allocator)
{
    resource->~MaterialResource();
    operator delete(resource, *allocator);
}

bool ResourceManager::MaterialResource::load(MemoryAllocator* const allocator)
{
    if(loaded()) {
        return true;
    }

    LOG_DEBUG("Loading material from " << path() << "...\n");
    _material.reset(new(*allocator) Material(path_to_key(path(), material_dir())),
        boost::bind(&Material::destroy, _1, allocator));
    return _material->load(path());
}

void ResourceManager::AnimationResource::destroy(AnimationResource* const resource, MemoryAllocator* const allocator)
{
    resource->~AnimationResource();
    operator delete(resource, *allocator);
}

void ResourceManager::ModelResource::destroy(ModelResource* const resource, MemoryAllocator* const allocator)
{
    resource->~ModelResource();
    operator delete(resource, *allocator);
}

void ResourceManager::SceneResource::destroy(SceneResource* const resource, MemoryAllocator* const allocator)
{
    resource->~SceneResource();
    operator delete(resource, *allocator);
}

void ResourceManager::ShaderResource::destroy(ShaderResource* const resource, MemoryAllocator* const allocator)
{
    resource->~ShaderResource();
    operator delete(resource, *allocator);
}

bool ResourceManager::ShaderResource::load(MemoryAllocator* const allocator)
{
    if(loaded()) {
        return true;
    }

    try {
        LOG_DEBUG("Loading shader from " << path() << "...\n");
        _shader.reset(new(*allocator) Shader(path_to_key(path(), shader_dir())),
            boost::bind(&Shader::destroy, _1, allocator));
        _shader->load(path());
    } catch(const ShaderError&) {
        // TODO: log something here
        return false;
    }

    return true;
}

void ResourceManager::TextureResource::destroy(TextureResource* const resource, MemoryAllocator* const allocator)
{
    resource->~TextureResource();
    operator delete(resource, *allocator);
}

bool ResourceManager::TextureResource::load(MemoryAllocator* const allocator)
{
    if(loaded()) {
        return true;
    }

    LOG_DEBUG("Loading texture from " << path() << "...\n");
    const std::string ext(boost::algorithm::to_lower_copy(path().extension()));
    if(".png" == ext) {
        _texture.reset(new(*allocator) PNG(),
            boost::bind(&PNG::destroy, _1, allocator));
    } else if(".tga" == ext) {
        _texture.reset(new(*allocator) Targa(),
            boost::bind(&Targa::destroy, _1, allocator));
    } else {
        LOG_ERROR("Filetype " << ext << " is not supported!\n");
        return false;
    }

    // texture data is stored in the scene allocator
    if(!_texture->load(path(), *allocator)) {
        LOG_ERROR("Could not load " << path() << "!\n");
        return false;
    }

    return true;
}

void ResourceManager::TextureImageResource::destroy(TextureImageResource* const resource, MemoryAllocator* const allocator)
{
    resource->~TextureImageResource();
    operator delete(resource, *allocator);
}

bool ResourceManager::TextureImageResource::load(MemoryAllocator* const allocator)
{
    if(loaded()) {
        return true;
    }

    if(!ready()) {
        return false;
    }

    LOG_DEBUG("Loading texture image to " << _texid << "\n");

    glBindTexture(GL_TEXTURE_2D, _texid);

    // TODO: texture format may not need to be BGR anymore?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _material->no_mipmap() ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _material->no_repeat() ? GL_CLAMP : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _material->no_repeat() ? GL_CLAMP : GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D,
        0,
        _texture->Bpp() == 4
            ? (_material->no_compress() ? GL_RGBA : GL_COMPRESSED_RGBA)
            : (_material->no_compress() ? GL_RGB : GL_COMPRESSED_RGB),
        _texture->width(), _texture->height(),
        _material->border() ? 1 : 0,
        _texture->Bpp() == 4 ? GL_BGRA : GL_BGR,
        GL_UNSIGNED_BYTE,
        _texture->pixels().get());

    if(!_material->no_mipmap()) {
        // this requires opengl 3.0
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    _loaded = true;

    return _loaded;
}

void ResourceManager::TextureImageResource::unload()
{
    if(!ready()) {
        _loaded = false;
        return;
    }

    boost::shared_ptr<DeleteTextureRenderCommand> command(
        boost::dynamic_pointer_cast<DeleteTextureRenderCommand, RenderCommand>(
            RenderCommand::new_render_command(RenderCommand::RC_DELETE_TEXTURE)));
    command->texture = _texid;
    Engine::instance().renderer().command_queue().push(command);

    _texid = 0;
    _loaded = false;
}

void ResourceManager::destroy(ResourceManager* const manager, MemoryAllocator* const allocator)
{
    manager->~ResourceManager();
    operator delete(manager, *allocator);
}

std::string ResourceManager::path_to_key(const boost::filesystem::path& path, const boost::filesystem::path& dir)
{
    const boost::filesystem::path basename(boost::algorithm::erase_first_copy(path.parent_path().string(), dir.string()));
    return boost::to_lower_copy((basename / path.stem()).string());
}

std::string ResourceManager::texture_image_resource(boost::shared_ptr<Material> material, Renderable::TextureBuffers::TextureBuffer texture)
{
    std::stringstream ss;
    ss << material->name() << "-" << texture;
    return boost::to_lower_copy(ss.str());
}

const std::string ResourceManager::DEFAULT_DETAIL_TEXTURE("textures/no-texture");
const std::string ResourceManager::DEFAULT_NORMALMAP_TEXTURE("textures/no-texture_n");

Logger& ResourceManager::logger(Logger::instance("gled.engine.ResourceManager"));

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager() throw()
{
}

bool ResourceManager::init()
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    LOG_INFO("Initializing resources...\n");

    if(!init_fonts()) {
        return false;
    }

    if(!init_lights()) {
        return false;
    }

    if(!init_maps()) {
        return false;
    }

    if(!init_materials()) {
        return false;
    }

    if(!init_animations()) {
        return false;
    }

    if(!init_models()) {
        return false;
    }

    if(!init_scenes()) {
        return false;
    }

    if(!init_shaders()) {
        return false;
    }

    if(!init_textures()) {
        return false;
    }

    return true;
}

void ResourceManager::shutdown()
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    LOG_INFO("Freeing resources...\n");

    BOOST_FOREACH(const TextureImageResourceMap::value_type& value, _texture_images) {
        value.second->unload();
    }
    _texture_images.clear();

    BOOST_FOREACH(const TextureResourceMap::value_type& value, _textures) {
        value.second->unload();
    }
    _textures.clear();

    BOOST_FOREACH(const ShaderResourceMap::value_type& value, _shaders) {
        value.second->unload();
    }
    _shaders.clear();

    BOOST_FOREACH(const SceneResourceMap::value_type& value, _scenes) {
        value.second->unload();
    }
    _scenes.clear();

    BOOST_FOREACH(const ModelResourceMap::value_type& value, _models) {
        value.second->unload();
    }
    _models.clear();

    BOOST_FOREACH(const AnimationResourceMap::value_type& value, _animations) {
        value.second->unload();
    }
    _animations.clear();

    BOOST_FOREACH(const MaterialResourceMap::value_type& value, _materials) {
        value.second->unload();
    }
    _materials.clear();

    BOOST_FOREACH(const MapResourceMap::value_type& value, _maps) {
        value.second->unload();
    }
    _maps.clear();

    BOOST_FOREACH(const LightResourceMap::value_type& value, _lights) {
        value.second->unload();
    }
    _lights.clear();

    BOOST_FOREACH(const FontResourceMap::value_type& value, _fonts) {
        value.second->unload();
    }
    _fonts.clear();

    Font::shutdown();
}

void ResourceManager::dump() const
{
    LOG_INFO("Resources:\n");

    LOG_INFO("Fonts (" << _fonts.size() << "):\n");
    BOOST_FOREACH(const FontResourceMap::value_type& value, _fonts) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Lights (" << _lights.size() << "):\n");
    BOOST_FOREACH(const LightResourceMap::value_type& value, _lights) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Maps (" << _maps.size() << "):\n");
    BOOST_FOREACH(const MapResourceMap::value_type& value, _maps) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Materials (" << _materials.size() << "):\n");
    BOOST_FOREACH(const MaterialResourceMap::value_type& value, _materials) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Animations (" << _animations.size() << "):\n");
    BOOST_FOREACH(const AnimationResourceMap::value_type& value, _animations) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Models (" << _models.size() << "):\n");
    BOOST_FOREACH(const ModelResourceMap::value_type& value, _models) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Scenes (" << _scenes.size() << "):\n");
    BOOST_FOREACH(const SceneResourceMap::value_type& value, _scenes) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Shaders (" << _shaders.size() << "):\n");
    BOOST_FOREACH(const ShaderResourceMap::value_type& value, _shaders) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Textures (" << _textures.size() << "):\n");
    BOOST_FOREACH(const TextureResourceMap::value_type& value, _textures) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }

    LOG_INFO("Texture images (" << _texture_images.size() << "):\n");
    BOOST_FOREACH(const TextureImageResourceMap::value_type& value, _texture_images) {
        LOG_DEBUG(value.first << " => " << value.second->path() << "\n");
    }
}

boost::filesystem::path ResourceManager::font_path(const std::string& name)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    // TODO: duplicate code :(
    const std::string lower(boost::algorithm::to_lower_copy(name));
    try {
        boost::shared_ptr<FontResource> resource(_fonts.at(lower));
        return resource->path();
    } catch(const std::out_of_range&) {
        add_font_resource(lower);
        try {
            boost::shared_ptr<FontResource> resource(_fonts.at(lower));
            return resource->path();
        } catch(const std::out_of_range&) {
        }
    }
    return boost::filesystem::path();
}

boost::shared_ptr<Material> ResourceManager::material(const std::string& name)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    // materials go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    // TODO: duplicate code :(
    const std::string lower(boost::algorithm::to_lower_copy(name));
    try {
        boost::shared_ptr<MaterialResource> resource(_materials.at(lower));
        if(!resource->load(&allocator)) {
            // TODO: print something
        }
        return resource->material();
    } catch(const std::out_of_range&) {
        add_material_resource(lower);
        try {
            boost::shared_ptr<MaterialResource> resource(_materials.at(lower));
            if(!resource->load(&allocator)) {
                // TODO: print something
            }
            return resource->material();
        } catch(const std::out_of_range&) {
        }
    }
    return boost::shared_ptr<Material>();
}

boost::shared_ptr<Shader> ResourceManager::shader(const std::string& name)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    // TODO: duplicate code :(
    const std::string lower(boost::algorithm::to_lower_copy(name));
    try {
        boost::shared_ptr<ShaderResource> resource(_shaders.at(lower));
        return resource->shader();
    } catch(const std::out_of_range&) {
        add_shader_resource(lower);
        try {
            boost::shared_ptr<ShaderResource> resource(_shaders.at(lower));
            return resource->shader();
        } catch(const std::out_of_range&) {
        }
    }
    return boost::shared_ptr<Shader>();
}

GLuint ResourceManager::texture(boost::shared_ptr<Material> material, Renderable::TextureBuffers::TextureBuffer texture)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    const std::string lower(texture_image_resource(material, texture));
    try {
        boost::shared_ptr<TextureImageResource> resource(_texture_images.at(lower));
        return resource->texture();
    } catch(const std::out_of_range&) {
        // can't add texture image resources
    }
    return 0;
}

bool ResourceManager::save_texture_image(boost::shared_ptr<Material> material, Renderable::TextureBuffers::TextureBuffer texture, size_t width, size_t height, size_t Bpp)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    boost::shared_ptr<TextureImageResource> resource;
    const std::string lower(texture_image_resource(material, texture));
    try {
        resource = _texture_images.at(lower);
        if(0 == resource->texture()) {
            return false;
        }
    } catch(const std::out_of_range&) {
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, resource->texture());

    // use the frame allocator to temporarily store the image data
    MemoryAllocator& allocator(Engine::instance().frame_allocator());
    boost::shared_array<unsigned char> pixels(new(allocator) unsigned char[width * height * Bpp],
        boost::bind(&MemoryAllocator::release, &allocator, _1));
    glGetTexImage(GL_TEXTURE_2D, 0, Bpp == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels.get());

    glBindTexture(GL_TEXTURE_2D, 0);

// TODO: don't assume PNG here
    PNG png(width, height, Bpp * 8, pixels);
    return png.save(resource->path());
}

void ResourceManager::load_shaders()
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    // shaders go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    // TODO: we shouldn't really load *every* shader, just the ones we need
    LOG_INFO("Loading shaders...\n");
    BOOST_FOREACH(const ShaderResourceMap::value_type& value, _shaders) {
        if(!value.second->load(&allocator)) {
            // TODO: print something
        }
    }
}

void ResourceManager::load_textures()
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    // texture images go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    LOG_INFO("Loading textures...\n");
    BOOST_FOREACH(const TextureImageResourceMap::value_type& value, _texture_images) {
        if(!value.second->load(&allocator)) {
            // TODO: print something
        }
    }
}

bool ResourceManager::init_fonts()
{
    LOG_DEBUG("Initializing fonts...\n");

    if(!Font::init()) {
        return false;
    }

    std::vector<boost::filesystem::path> files;
    list_directory(font_dir(), files, true);
    BOOST_FOREACH(const boost::filesystem::path& path, files) {
        add_font_resource(path);
    }

    return true;
}

void ResourceManager::add_font_resource(const boost::filesystem::path& path)
{
#if BOOST_VERSION >= 104600
    const std::string extension(boost::algorithm::to_lower_copy(path.extension().string()));
#else
    const std::string extension(boost::algorithm::to_lower_copy(path.extension()));
#endif

    // resources go on the system allocator
    MemoryAllocator& allocator(Engine::instance().system_allocator());
    if(".ttf" == extension) {
        _fonts[path_to_key(path, font_dir())] = boost::shared_ptr<FontResource>(new(allocator) FontResource(path),
            boost::bind(FontResource::destroy, _1, &allocator));
    }
}

bool ResourceManager::init_lights()
{
    LOG_DEBUG("Initializing lights...\n");

    return true;
}

bool ResourceManager::init_maps()
{
    LOG_DEBUG("Initializing maps...\n");

    return true;
}

bool ResourceManager::init_materials()
{
    LOG_DEBUG("Initializing materials...\n");

    std::vector<boost::filesystem::path> files;
    list_directory(material_dir(), files, true);
    BOOST_FOREACH(const boost::filesystem::path& path, files) {
        add_material_resource(path);
    }

    return true;
}

void ResourceManager::add_material_resource(const boost::filesystem::path& path)
{
#if BOOST_VERSION >= 104600
    const std::string extension(boost::algorithm::to_lower_copy(path.extension().string()));
#else
    const std::string extension(boost::algorithm::to_lower_copy(path.extension()));
#endif

    // resources go on the system allocator
    MemoryAllocator& allocator(Engine::instance().system_allocator());
    if(".material" == extension) {
        _materials[path_to_key(path, material_dir())] = boost::shared_ptr<MaterialResource>(new(allocator) MaterialResource(path),
            boost::bind(MaterialResource::destroy, _1, &allocator));
    }
}

bool ResourceManager::init_animations()
{
    LOG_DEBUG("Initializing animations...\n");

    std::vector<boost::filesystem::path> files;
    list_directory(animation_dir(), files, true);
    BOOST_FOREACH(const boost::filesystem::path& path, files) {
        add_animation_resource(path);
    }

    return true;
}

void ResourceManager::add_animation_resource(const boost::filesystem::path& path)
{
#if BOOST_VERSION >= 104600
    const std::string extension(boost::algorithm::to_lower_copy(path.extension().string()));
#else
    const std::string extension(boost::algorithm::to_lower_copy(path.extension()));
#endif

    // resources go on the system allocator
    MemoryAllocator& allocator(Engine::instance().system_allocator());
    if(".md5anim" == extension || ".mdlanim" == extension) {
        _animations[path_to_key(path, animation_dir())] = boost::shared_ptr<AnimationResource>(new(allocator) AnimationResource(path),
            boost::bind(AnimationResource::destroy, _1, &allocator));
    }
}

bool ResourceManager::init_models()
{
    LOG_DEBUG("Initializing models...\n");

    std::vector<boost::filesystem::path> files;
    list_directory(model_dir(), files, true);
    BOOST_FOREACH(const boost::filesystem::path& path, files) {
        add_model_resource(path);
    }

    return true;
}

void ResourceManager::add_model_resource(const boost::filesystem::path& path)
{
#if BOOST_VERSION >= 104600
    const std::string extension(boost::algorithm::to_lower_copy(path.extension().string()));
#else
    const std::string extension(boost::algorithm::to_lower_copy(path.extension()));
#endif

    // resources go on the system allocator
    MemoryAllocator& allocator(Engine::instance().system_allocator());
    if(".md5anim" == extension || ".mdlanim" == extension) {
        _models[path_to_key(path, model_dir())] = boost::shared_ptr<ModelResource>(new(allocator) ModelResource(path),
            boost::bind(ModelResource::destroy, _1, &allocator));
    }
}

bool ResourceManager::init_scenes()
{
    LOG_DEBUG("Initializing scenes...\n");

    return true;
}

bool ResourceManager::init_shaders()
{
    LOG_DEBUG("Initializing shaders...\n");

    std::vector<boost::filesystem::path> files;
    list_directory(shader_dir(), files, true);
    BOOST_FOREACH(const boost::filesystem::path& path, files) {
        add_shader_resource(path);
    }

    return true;
}

void ResourceManager::add_shader_resource(const boost::filesystem::path& path)
{
#if BOOST_VERSION >= 104600
    const std::string extension(boost::algorithm::to_lower_copy(path.extension().string()));
#else
    const std::string extension(boost::algorithm::to_lower_copy(path.extension()));
#endif

    // resources go on the system allocator
    MemoryAllocator& allocator(Engine::instance().system_allocator());
    if(".shader" == extension) {
        _shaders[path_to_key(path, shader_dir())] = boost::shared_ptr<ShaderResource>(new(allocator) ShaderResource(path),
            boost::bind(ShaderResource::destroy, _1, &allocator));
    }
}

bool ResourceManager::init_textures()
{
    LOG_DEBUG("Initializing textures...\n");

    std::vector<boost::filesystem::path> files;
    list_directory(texture_dir(), files, true);
    BOOST_FOREACH(const boost::filesystem::path& path, files) {
        add_texture_resource(path);
    }

    // gotta have the default textures
    try {
        _textures.at(DEFAULT_DETAIL_TEXTURE);
        _textures.at(DEFAULT_NORMALMAP_TEXTURE);
    } catch(const std::out_of_range&) {
        LOG_ERROR("Missing default texture!\n");
        return false;
    }

    return true;
}

void ResourceManager::add_texture_resource(const boost::filesystem::path& path, bool add_basename)
{
    // NOTE: only doing this because D3 models expect it
    if("" == path.extension()) {
        return add_texture_resource(path.string() + ".tga", false);
    }

#if BOOST_VERSION >= 104600
    const std::string extension(boost::algorithm::to_lower_copy(path.extension().string()));
#else
    const std::string extension(boost::algorithm::to_lower_copy(path.extension()));
#endif

    // resources go on the system allocator
    MemoryAllocator& allocator(Engine::instance().system_allocator());
    if(".png" == extension || ".tga" == extension) {
        // NOTE: only adding the basename because D3 maps expect it
        std::string key(add_basename ? (texture_dir_no_data().string() + "/" + path_to_key(path, texture_dir())) : path_to_key(path, texture_dir()));
        _textures[key] = boost::shared_ptr<TextureResource>(new(allocator) TextureResource(path),
            boost::bind(TextureResource::destroy, _1, &allocator));
    }
}

void ResourceManager::preload_material_textures(boost::shared_ptr<Material> material)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    // textures go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    if(material->has_detail_texture()) {
        // check for an existing texture image
        const std::string texture(texture_image_resource(material, Renderable::TextureBuffers::DetailTexture));
        try {
            _texture_images.at(texture);
            return;
        } catch(const std::out_of_range&) {
        }

        // get the texture resource
        const std::string lower(boost::algorithm::to_lower_copy(material->detail_texture()));
        boost::shared_ptr<TextureResource> resource;
        try {
            resource = _textures.at(lower);
        } catch(const std::out_of_range&) {
            // TODO: print something
            resource = _textures.at(DEFAULT_DETAIL_TEXTURE);
        }

        // load the texture and allocate some space on the GPU
        if(resource && resource->load(&allocator)) {
            boost::shared_ptr<GenTextureRenderCommand> command(
                boost::dynamic_pointer_cast<GenTextureRenderCommand, RenderCommand>(
                    RenderCommand::new_render_command(RenderCommand::RC_GEN_TEXTURE)));
            command->callback = boost::bind(&ResourceManager::texture_buffer_callback, this, resource, material, Renderable::TextureBuffers::DetailTexture, _1);
            Engine::instance().renderer().command_queue().push(command);
        }
    }

    if(material->has_normal_map()) {
        // check for an existing texture image
        const std::string texture(texture_image_resource(material, Renderable::TextureBuffers::NormalMap));
        try {
            _texture_images.at(texture);
            return;
        } catch(const std::out_of_range&) {
        }

        // get the texture resource
        const std::string lower(boost::algorithm::to_lower_copy(material->normal_map()));
        boost::shared_ptr<TextureResource> resource;
        try {
            resource = _textures.at(lower);
        } catch(const std::out_of_range&) {
            // TODO: print something
            resource = _textures.at(DEFAULT_NORMALMAP_TEXTURE);
        }

        // load the texture and allocate some space on the GPU
        if(resource && resource->load(&allocator)) {
            boost::shared_ptr<GenTextureRenderCommand> command(
                boost::dynamic_pointer_cast<GenTextureRenderCommand, RenderCommand>(
                    RenderCommand::new_render_command(RenderCommand::RC_GEN_TEXTURE)));
            command->callback = boost::bind(&ResourceManager::texture_buffer_callback, this, resource, material, Renderable::TextureBuffers::NormalMap, _1);
            Engine::instance().renderer().command_queue().push(command);
        }
    }

    if(material->has_specular_map()) {
        // check for an existing texture image
        const std::string texture(texture_image_resource(material, Renderable::TextureBuffers::SpecularMap));
        try {
            _texture_images.at(texture);
            return;
        } catch(const std::out_of_range&) {
        }

        // get the texture resource
        const std::string lower(boost::algorithm::to_lower_copy(material->specular_map()));
        boost::shared_ptr<TextureResource> resource;
        try {
            resource = _textures.at(lower);
        } catch(const std::out_of_range&) {
            // TODO: print something
            //resource = _textures.at(DEFAULT_SPECULARMAP_TEXTURE);
        }

        // load the texture and allocate some space on the GPU
        if(resource && resource->load(&allocator)) {
            boost::shared_ptr<GenTextureRenderCommand> command(
                boost::dynamic_pointer_cast<GenTextureRenderCommand, RenderCommand>(
                    RenderCommand::new_render_command(RenderCommand::RC_GEN_TEXTURE)));
            command->callback = boost::bind(&ResourceManager::texture_buffer_callback, this, resource, material, Renderable::TextureBuffers::SpecularMap, _1);
            Engine::instance().renderer().command_queue().push(command);
        }
    }

    if(material->has_emission_map()) {
        // check for an existing texture image
        const std::string texture(texture_image_resource(material, Renderable::TextureBuffers::EmissionMap));
        try {
            _texture_images.at(texture);
            return;
        } catch(const std::out_of_range&) {
        }

        // get the texture resource
        const std::string lower(boost::algorithm::to_lower_copy(material->emission_map()));
        boost::shared_ptr<TextureResource> resource;
        try {
            resource = _textures.at(lower);
        } catch(const std::out_of_range&) {
            // TODO: print something
            //resource = _textures.at(DEFAULT_EMISSIONMAP_TEXTURE);
        }

        // load the texture and allocate some space on the GPU
        if(resource && resource->load(&allocator)) {
            boost::shared_ptr<GenTextureRenderCommand> command(
                boost::dynamic_pointer_cast<GenTextureRenderCommand, RenderCommand>(
                    RenderCommand::new_render_command(RenderCommand::RC_GEN_TEXTURE)));
            command->callback = boost::bind(&ResourceManager::texture_buffer_callback, this, resource, material, Renderable::TextureBuffers::EmissionMap, _1);
            Engine::instance().renderer().command_queue().push(command);
        }
    }
}

void ResourceManager::texture_buffer_callback(boost::shared_ptr<TextureResource> texture, boost::shared_ptr<Material> material, Renderable::TextureBuffers::TextureBuffer type, GLuint texid)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);

    // TODO: duplicate code :(
    const std::string lower(texture_image_resource(material, type));
    try {
        boost::shared_ptr<TextureImageResource> resource(_texture_images.at(lower));
        LOG_WARNING("Overwriting existing texture image!\n");
        resource->texture(texid);
    } catch(const std::out_of_range&) {
        // resources go on the system allocator
        MemoryAllocator& allocator(Engine::instance().system_allocator());
        boost::shared_ptr<TextureImageResource> resource(new(allocator) TextureImageResource(texture, material),
            boost::bind(TextureImageResource::destroy, _1, &allocator));
        resource->texture(texid);
        _texture_images[lower] = resource;
    }
}

}
