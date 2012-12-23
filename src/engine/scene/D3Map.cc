#include "src/pch.h"
#include "src/core/common.h"
#include "src/engine/DoomLexer.h"
#include "src/engine/Engine.h"
#include "src/engine/ResourceManager.h"
#include "src/engine/State.h"
#include "src/engine/renderer/Camera.h"
#include "src/engine/renderer/Renderable.h"
#include "src/engine/renderer/Renderer.h"
#include "src/engine/renderer/Shader.h"
#include "D3Map.h"

namespace energonsoftware {

D3Map::Surface::Surface()
{
    glGenBuffers(Renderable::RenderBuffers::GeometryVBOCount, vbo);
}

D3Map::Surface::~Surface() throw()
{
    glDeleteBuffers(Renderable::RenderBuffers::GeometryVBOCount, vbo);
    glDeleteTextures(Renderable::TextureBuffers::TextureBufferCount, textures);
}

void D3Map::Surface::init()
{
    // store the temporary vectors on the frame allocator
    compute_tangents(triangles, triangle_count, vertices, vertex_count, Engine::instance().frame_allocator());

    // geometry goes on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());
    geometry.reset(new(allocator) Geometry(triangle_count, vertex_count, allocator),
        boost::bind(&Geometry::destroy, _1, &allocator));
    geometry->copy_triangles(triangles.get(), triangle_count, vertices.get(), vertex_count, 0);

    // setup the vertex array
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Renderable::RenderBuffers::VertexArray]);
    glBufferData(GL_ARRAY_BUFFER, geometry->vertex_buffer_size() * sizeof(float), geometry->vertex_buffer().get(), GL_STATIC_DRAW);

    // setup the normal array
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Renderable::RenderBuffers::NormalArray]);
    glBufferData(GL_ARRAY_BUFFER, geometry->normal_buffer_size() * sizeof(float), geometry->normal_buffer().get(), GL_STATIC_DRAW);

    // setup the tangent array
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Renderable::RenderBuffers::TangentArray]);
    glBufferData(GL_ARRAY_BUFFER, geometry->tangent_buffer_size() * sizeof(float), geometry->tangent_buffer().get(), GL_STATIC_DRAW);

    // setup the texture array
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Renderable::RenderBuffers::TextureArray]);
    glBufferData(GL_ARRAY_BUFFER, geometry->texture_buffer_size() * sizeof(float), geometry->texture_buffer().get(), GL_STATIC_DRAW);

    // setup the normal line array
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Renderable::RenderBuffers::NormalLineArray]);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 2 * 3 * sizeof(float), geometry->normal_line_buffer().get(), GL_STATIC_DRAW);

    // setup the tangent line array
    glBindBuffer(GL_ARRAY_BUFFER, vbo[Renderable::RenderBuffers::TangentLineArray]);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * 2 * 3 * sizeof(float), geometry->tangent_line_buffer().get(), GL_STATIC_DRAW);
}

bool D3Map::Surface::load_textures()
{
    // TODO: move this into a load_textures() method of the Surface class
    textures[ResourceManager::DetailTexture] = Engine::instance().resource_manager().texture(material);
    if(0 == textures[ResourceManager::DetailTexture]) {
        return false;
    }

    textures[ResourceManager::NormalMap] = Engine::instance().resource_manager().texture(material + "_local");
    if(0 == textures[ResourceManager::NormalMap]) {
        return false;
    }

    textures[ResourceManager::SpecularMap] = Engine::instance().resource_manager().texture(material + "_s");
    if(0 == textures[ResourceManager::SpecularMap]) {
        return false;
    }

    /*textures[ResourceManager::EmissionMap] = Engine::instance().resource_manager().texture(material + "_e");
    if(0 == textures[ResourceManager::EmissionMap]) {
        return false;
    }*/

    return true;
}

Logger& D3Map::logger(Logger::instance("gled.engine.scene.D3Map"));

void D3Map::destroy(D3Map* const map, MemoryAllocator* const allocator)
{
    map->~D3Map();
    operator delete(map, 16, *allocator);
}

void D3Map::destroy_entity(Entity* const entity, MemoryAllocator* const allocator)
{
    entity->~Entity();
    operator delete(entity, 16, *allocator);
}

void D3Map::destroy_brush(Brush* const brush, MemoryAllocator* const allocator)
{
    brush->~Brush();
    operator delete(brush, 16, *allocator);
}

void D3Map::destroy_patch2(Patch2* const patch, MemoryAllocator* const allocator)
{
    patch->~Patch2();
    operator delete(patch, 16, *allocator);
}

void D3Map::destroy_patch3(Patch3* const patch, MemoryAllocator* const allocator)
{
    patch->~Patch3();
    operator delete(patch, 16, *allocator);
}

void D3Map::destroy_model(Model* const model, MemoryAllocator* const allocator)
{
    model->~Model();
    operator delete(model, 16, *allocator);
}

void D3Map::destroy_surface(Surface* const surface, MemoryAllocator* const allocator)
{
    surface->~Surface();
    operator delete(surface, 16, *allocator);
}

D3Map::D3Map(const std::string& name)
    : Map(name), _version(0), _acount(0)
{
}

D3Map::~D3Map() throw()
{
    unload();
}

Position D3Map::player_spawn_position() const
{
    Position position;
    try {
        boost::shared_ptr<Entity> player_start(_entities.at("info_player_start_1"));
        const std::string data(player_start->properties.at("origin"));

        DoomLexer lexer(data);

        float value;
        if(!lexer.float_literal(value)) {
            return position;
        }
        position.x(value);

        if(!lexer.float_literal(value)) {
            return position;
        }
        position.y(value);

        if(!lexer.float_literal(value)) {
            return position;
        }
        position.z(value);
    } catch(const std::out_of_range&) {
        LOG_WARNING("Missing info_player_start origin!\n");
    }

    return swizzle(position);
}

float D3Map::player_spawn_angle() const
{
    float angle = 0.0f;
    try {
        boost::shared_ptr<Entity> player_start(_entities.at("info_player_start_1"));
        const std::string data(player_start->properties.at("angle"));

        DoomLexer lexer(data);
        if(!lexer.float_literal(angle)) {
            return angle;
        }
    } catch(const std::out_of_range&) {
        LOG_WARNING("Missing info_player_start angle!\n");
    }

    return angle;
}

bool D3Map::load(const boost::filesystem::path& path)
{
    unload();

    if(!load_map(path)) {
        return false;
    }

    if(!load_proc(path)) {
        return false;
    }

    return true;
}

void D3Map::on_unload()
{
    _version = 0;

    _acount = 0;
    _models.clear();

    _entities.clear();
    _worldspawn.reset();
}

void D3Map::render(const Camera& camera, Shader& shader) const
{
    Matrix4 matrix;
    matrix.translate(Position());

    Engine::instance().renderer().push_model_matrix();
    Engine::instance().renderer().multiply_model_matrix(matrix);

    Engine::instance().renderer().init_shader_matrices(shader);

    // TODO: BSP and portal this shit
    BOOST_FOREACH(boost::shared_ptr<Model> model, _models) {
        if(model->is_area()) {
            render_area(camera, *model, shader);
        }
    }

    Engine::instance().renderer().pop_model_matrix();
}


void D3Map::render(const Camera& camera, Shader& shader, const Light& light) const
{
    Engine::instance().renderer().init_shader_matrices(shader);
    Engine::instance().renderer().init_shader_light(shader, material(), light, camera);

    // TODO: BSP and portal this shit
    // for now we'll just do a bounds check
    BOOST_FOREACH(boost::shared_ptr<Model> model, _models) {
        if(model->is_area() && camera.visible(model->bounds)) {
            render_area(camera, *model, shader);
        }
    }
}

void D3Map::render_normals(const Camera& camera) const
{
    // TODO: BSP and portal this shit
    // for now we'll just do a bounds check
    BOOST_FOREACH(boost::shared_ptr<Model> model, _models) {
        if(model->is_area() && camera.visible(model->bounds)) {
            render_area_normals(camera, *model);
        }
    }
}

void D3Map::render_area(const Camera& camera, const Model& area, Shader& shader) const
{
    for(int i=0; i<area.surface_count; ++i) {
        const Surface& surface(*(area.surfaces[i]));
        if(camera.visible(surface.bounds)) {
            render_surface(surface, shader);
        }
    }
}

void D3Map::render_area_normals(const Camera& camera, const Model& area) const
{
    for(int i=0; i<area.surface_count; ++i) {
        const Surface& surface(*(area.surfaces[i]));
        if(camera.visible(surface.bounds)) {
            render_surface_normals(surface);
        }
    }
}

void D3Map::render_surface(const Surface& surface, Shader& shader) const
{
    // setup the detail texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, surface.textures[ResourceManager::DetailTexture]);
    shader.uniform1i("detail_texture", 0);

    // setup the normal map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, surface.textures[ResourceManager::NormalMap]);
    shader.uniform1i("normal_map", 1);

    // setup the specular map
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, surface.textures[ResourceManager::SpecularMap]);
    shader.uniform1i("specular_map", 2);

    // setup the emission map
    /*glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mesh.texture(ResourceManager::EmissionMap));
    shader.uniform1i("emission_map", 3);*/

    // get the attribute locations
    GLint vloc = shader.attrib_location("vertex");
    GLint nloc = shader.attrib_location("normal");
    GLint tnloc = shader.attrib_location("tangent");
    GLint tloc = shader.attrib_location("texture_coord");

    // render the mesh
    glEnableVertexAttribArray(vloc);
    glEnableVertexAttribArray(nloc);
    glEnableVertexAttribArray(tnloc);
    glEnableVertexAttribArray(tloc);
        glBindBuffer(GL_ARRAY_BUFFER, surface.vbo[Renderable::RenderBuffers::TextureArray]);
        glVertexAttribPointer(tloc, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, surface.vbo[Renderable::RenderBuffers::NormalArray]);
        glVertexAttribPointer(nloc, 3, GL_FLOAT, GL_TRUE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, surface.vbo[Renderable::RenderBuffers::TangentArray]);
        glVertexAttribPointer(tnloc, 4, GL_FLOAT, GL_TRUE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, surface.vbo[Renderable::RenderBuffers::VertexArray]);
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, surface.triangle_count * 3);
    glDisableVertexAttribArray(tloc);
    glDisableVertexAttribArray(tnloc);
    glDisableVertexAttribArray(nloc);
    glDisableVertexAttribArray(vloc);
}

void D3Map::render_surface_normals(const Surface& surface) const
{
    // render the normals
    boost::shared_ptr<Shader> rshader(Engine::instance().resource_manager().shader("red"));
    rshader->begin();
    Engine::instance().renderer().init_shader_matrices(*rshader);

    // get the attribute locations
    GLint vloc = rshader->attrib_location("vertex");

    // render the mesh
    glEnableVertexAttribArray(vloc);
        glBindBuffer(GL_ARRAY_BUFFER, surface.vbo[Renderable::RenderBuffers::NormalLineArray]);
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_LINES, 0, surface.vertex_count * 2);
    glDisableVertexAttribArray(vloc);

    rshader->end();

    // render the tangents
    boost::shared_ptr<Shader> gshader(Engine::instance().resource_manager().shader("green"));
    gshader->begin();
    Engine::instance().renderer().init_shader_matrices(*gshader);

    // get the attribute locations
    vloc = gshader->attrib_location("vertex");

    // render the mesh
    glEnableVertexAttribArray(vloc);
        glBindBuffer(GL_ARRAY_BUFFER, surface.vbo[Renderable::RenderBuffers::TangentLineArray]);
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_LINES, 0, surface.vertex_count * 2);
    glDisableVertexAttribArray(vloc);

    gshader->end();
}

bool D3Map::load_map(const boost::filesystem::path& path)
{
    boost::filesystem::path filename = map_dir() / path / (name() + ".map");
    LOG_INFO("Loading map from '" << filename << "'\n");

    DoomLexer lexer;
    if(!lexer.load(filename)) {
        return false;
    }

    if(!scan_map_version(lexer)) {
        return false;
    }

    // map is just a list of entities
    while(!lexer.check_token(END)) {
        if(!scan_map_entity(lexer)) {
            return false;
        }
        lexer.skip_whitespace();
    }

    if(!_worldspawn) {
        LOG_ERROR("Map missing worldspawn!\n");
        return false;
    }

    LOG_INFO("Read " << _entities.size() << " non-worldspawn entities\n");

    return true;
}

bool D3Map::scan_map_version(Lexer& lexer)
{
    if(!lexer.match(VERSION)) {
        return false;
    }

    if(!lexer.int_literal(_version)) {
        return false;
    }

    // only version 2 is supported
    return _version == 2;
}

bool D3Map::scan_map_entity(Lexer& lexer)
{
    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    // entities stored in the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());
    boost::shared_ptr<Entity> entity(new(16, allocator) Entity(), boost::bind(&D3Map::destroy_entity, _1, &allocator));

    // read the key/value pairs
    while(!lexer.check_token(OPEN_BRACE) && !lexer.check_token(CLOSE_BRACE)) {
        std::string key;
        if(!lexer.string_literal(key)) {
            return false;
        }

        std::string value;
        if(!lexer.string_literal(value)) {
            return false;
        }

        entity->properties[key] = value;
    }

    // scan brushes
    if(!lexer.check_token(CLOSE_BRACE)) {
        if(!scan_map_brushes(lexer, entity)) {
            return false;
        }
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

    try {
// TODO: make a mapping of property keys or something
        const std::string& classname(entity->properties.at("classname"));
        if(classname == "worldspawn") {
            if(_worldspawn) {
                LOG_ERROR("Only one worldspawn allowed!\n");
                return false;
            }
            _worldspawn = entity;
            LOG_INFO("World has " << entity->brushes.size() << " brushes and " << entity->patches.size() << " patches\n");
        } else {
            const std::string& name(entity->properties.at("name"));
            _entities[name] = entity;
        }
    } catch(std::out_of_range&) {
        return false;
    }

    return true;
}

bool D3Map::scan_map_brushes(Lexer& lexer, boost::shared_ptr<Entity> entity)
{
    while(!lexer.check_token(CLOSE_BRACE)) {
        if(!lexer.match(OPEN_BRACE)) {
            return false;
        }

        // either a brush or a patch
        if(lexer.check_token(BRUSHDEF3)) {
            if(!scan_map_brushdef3(lexer, entity)) {
                return false;
            }
        } else if(lexer.check_token(PATCHDEF2)) {
            if(!scan_map_patchdef2(lexer, entity)) {
                return false;
            }
        } else if(lexer.check_token(PATCHDEF3)) {
            if(!scan_map_patchdef3(lexer, entity)) {
                return false;
            }
        } else {
            LOG_ERROR("Unexpected brush token!\n");
            return false;
        }

        if(!lexer.match(CLOSE_BRACE)) {
            return false;
        }
    }

    return true;
}

bool D3Map::scan_map_brushdef3(Lexer& lexer, boost::shared_ptr<Entity> entity)
{
    if(!lexer.match(BRUSHDEF3)) {
        return false;
    }

    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    while(!lexer.check_token(CLOSE_BRACE)) {
        if(!scan_map_brush(lexer, entity)) {
            return false;
        }
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

    return true;
}

bool D3Map::scan_map_brush(Lexer& lexer, boost::shared_ptr<Entity> entity)
{
    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    // normal
    Vector3 normal;

    float value;
    if(!lexer.float_literal(value)) {
        return false;
    }
    normal.x(value);

    if(!lexer.float_literal(value)) {
        return false;
    }
    normal.y(value);

    if(!lexer.float_literal(value)) {
        return false;
    }
    normal.z(value);

    // distance
    float d;
    if(!lexer.float_literal(d)) {
        return false;
    }

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    // brushes stored in the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());
    boost::shared_ptr<Brush> brush(new(16, allocator) Brush(), boost::bind(&D3Map::destroy_brush, _1, &allocator));
    brush->plane = Plane(swizzle(normal), d);

    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    float xxscale;
    if(!lexer.float_literal(xxscale)) {
        return false;
    }

    float xyscale;
    if(!lexer.float_literal(xyscale)) {
        return false;
    }

    float xoffset;
    if(!lexer.float_literal(xoffset)) {
        return false;
    }

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    float yxscale;
    if(!lexer.float_literal(yxscale)) {
        return false;
    }

    float yyscale;
    if(!lexer.float_literal(yyscale)) {
        return false;
    }

    float yoffset;
    if(!lexer.float_literal(yoffset)) {
        return false;
    }

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    Matrix3 texture_matrix;
    texture_matrix(0, 0) = xxscale;
    texture_matrix(0, 1) = xyscale;
    texture_matrix(0, 2) = xoffset;
    texture_matrix(1, 0) = yxscale;
    texture_matrix(1, 1) = yyscale;
    texture_matrix(1, 2) = yoffset;
    brush->texture_matrix = texture_matrix;

    std::string material;
    if(!lexer.string_literal(material)) {
        return false;
    }

    brush->material = material;

    // swallow the unknown numbers that may follow the brushdef
    lexer.advance_line();

    entity->brushes.push_back(brush);

    return true;
}

bool D3Map::scan_map_patchdef2(Lexer& lexer, boost::shared_ptr<Entity> entity)
{
    if(!lexer.match(PATCHDEF2)) {
        return false;
    }

    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    std::string material;
    if(!lexer.string_literal(material)) {
        return false;
    }

    // patches stored in the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());
    boost::shared_ptr<Patch2> patch(new(16, allocator) Patch2(), boost::bind(&D3Map::destroy_patch2, _1, &allocator));
    patch->material = material;

    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    float x;
    if(!lexer.float_literal(x)) {
        return false;
    }

    float y;
    if(!lexer.float_literal(y)) {
        return false;
    }

    // TODO: swizzle??
    patch->offset = Vector2(x, y);

    float rotation;
    if(!lexer.float_literal(rotation)) {
        return false;
    }
    patch->rotation = rotation;

    if(!lexer.float_literal(x)) {
        return false;
    }

    if(!lexer.float_literal(y)) {
        return false;
    }

    // TODO: swizzle?
    patch->scale = Vector2(x, y);

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    if(!scan_patch_control_grid(lexer, patch)) {
        return false;
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

    entity->patches.push_back(patch);

    return true;
}

bool D3Map::scan_map_patchdef3(Lexer& lexer, boost::shared_ptr<Entity> entity)
{
    if(!lexer.match(PATCHDEF3)) {
        return false;
    }

    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    std::string material;
    if(!lexer.string_literal(material)) {
        return false;
    }

    // patches stored in the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());
    boost::shared_ptr<Patch3> patch(new(16, allocator) Patch3(), boost::bind(&D3Map::destroy_patch3, _1, &allocator));
    patch->material = material;

    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    float x;
    if(!lexer.float_literal(x)) {
        return false;
    }

    float y;
    if(!lexer.float_literal(y)) {
        return false;
    }

    float z;
    if(!lexer.float_literal(z)) {
        return false;
    }

    // TODO: swizzle??
    patch->offset = Vector3(x, y, z);

    float rotation;
    if(!lexer.float_literal(rotation)) {
        return false;
    }
    patch->rotation = rotation;

    if(!lexer.float_literal(x)) {
        return false;
    }

    if(!lexer.float_literal(y)) {
        return false;
    }

    if(!lexer.float_literal(z)) {
        return false;
    }

    // TODO: swizzle?
    patch->scale = Vector3(x, y, z);

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    if(!scan_patch_control_grid(lexer, patch)) {
        return false;
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

    entity->patches.push_back(patch);

    return true;
}

bool D3Map::scan_patch_control_grid(Lexer& lexer, boost::shared_ptr<Patch> patch)
{
    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    while(!lexer.check_token(CLOSE_PAREN)) {
        if(!scan_patch_control_grid_row(lexer, patch)) {
            return false;
        }
    }

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    return true;
}

bool D3Map::scan_patch_control_grid_row(Lexer& lexer, boost::shared_ptr<Patch> patch)
{
    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    while(!lexer.check_token(CLOSE_PAREN)) {
        if(!scan_patch_control_point(lexer, patch)) {
            return false;
        }
    }

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    return true;
}

bool D3Map::scan_patch_control_point(Lexer& lexer, boost::shared_ptr<Patch> patch)
{
    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    float x;
    if(!lexer.float_literal(x)) {
        return false;
    }

    float y;
    if(!lexer.float_literal(y)) {
        return false;
    }

    float z;
    if(!lexer.float_literal(z)) {
        return false;
    }

    float u;
    if(!lexer.float_literal(u)) {
        return false;
    }

    float v;
    if(!lexer.float_literal(v)) {
        return false;
    }

//LOG_ERROR("TODO: handle patch defs!\n");

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    return true;
}

bool D3Map::load_proc(const boost::filesystem::path& path)
{
    boost::filesystem::path filename(map_dir() / path / (name() + ".proc"));
    LOG_INFO("Loading geometry from '" << filename << "'\n");

    DoomLexer lexer;
    if(!lexer.load(filename)) {
        return false;
    }

    if(!scan_proc_header(lexer)) {
        return false;
    }

    while(!lexer.check_token(END)) {
        if(lexer.check_token(MODEL)) {
            if(!scan_proc_model(lexer)) {
                return false;
            }
        } else if(lexer.check_token(PORTALS)) {
            if(!scan_proc_portals(lexer)) {
                return false;
            }
        } else if(lexer.check_token(NODES)) {
            if(!scan_proc_nodes(lexer)) {
                return false;
            }
        } else if(lexer.check_token(SHADOW_MODEL)) {
            if(!scan_proc_shadow_model(lexer)) {
                return false;
            }
        }
    }

    return true;
}

bool D3Map::scan_proc_header(Lexer& lexer)
{
    if(!lexer.match(MAP_PROC_FILE)) {
        return false;
    }

    return true;
}

bool D3Map::scan_proc_model(Lexer& lexer)
{
    if(!lexer.match(MODEL)) {
        return false;
    }

    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    std::string name;
    if(!lexer.string_literal(name)) {
        return false;
    }

    int surface_count;
    if(!lexer.int_literal(surface_count)) {
        return false;
    }

    // models and surfaces stores in the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());
    boost::shared_ptr<Model> model(new(16, allocator) Model(), boost::bind(&D3Map::destroy_model, _1, &allocator));
    model->name = name;
    model->surface_count = surface_count;

    // TODO: need to put this on an allocator
    model->surfaces.reset(new boost::shared_ptr<Surface>[model->surface_count]);
    for(int i=0; i<model->surface_count; ++i) {
        boost::shared_ptr<Surface> surface(new(16, allocator) Surface(), boost::bind(&D3Map::destroy_surface, _1, &allocator));
        if(!scan_proc_surface(lexer, surface)) {
            return false;
        }
        model->surfaces[i] = surface;
        model->bounds.update(surface->bounds);
    }
    _models.push_back(model);

    if(model->is_area()) {
        _acount++;
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

    return true;
}

bool D3Map::scan_proc_surface(Lexer& lexer, boost::shared_ptr<Surface> surface)
{
    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    std::string material;
    if(!lexer.string_literal(material)) {
        return false;
    }

    int vertex_count;
    if(!lexer.int_literal(vertex_count)) {
        return false;
    }

    int index_count;
    if(!lexer.int_literal(index_count)) {
        return false;
    }

    // indices define triangles, so we need a multiple of 3
    if(0 != index_count % 3) {
        return false;
    }

    surface->material = material;

    // vertices and triangles stored on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());
    surface->vertex_count = vertex_count;
    surface->vertices.reset(Vertex::create_array(surface->vertex_count, allocator),
        boost::bind(&Vertex::destroy_array, _1, surface->vertex_count, &allocator));

    surface->triangle_count = index_count / 3;
    surface->triangles.reset(Triangle::create_array(surface->triangle_count, allocator),
        boost::bind(&Triangle::destroy_array, _1, surface->triangle_count, &allocator));

    for(int i=0; i<surface->vertex_count; ++i) {
        if(!scan_proc_vertex(lexer, surface, i)) {
            return false;
        }
    }

    for(int i=0; i<surface->triangle_count; ++i) {
        int a;
        if(!lexer.int_literal(a)) {
            return false;
        }

        int b;
        if(!lexer.int_literal(b)) {
            return false;
        }

        int c;
        if(!lexer.int_literal(c)) {
            return false;
        }

        // TODO: find a better way to swizzle this
        Triangle& triangle(surface->triangles[i]);
        triangle.index = i;
        triangle.v1 = c;
        triangle.v2 = b;
        triangle.v3 = a;
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

    surface->init();
    if(!surface->load_textures()) {
        return false;
    }

    return true;
}

bool D3Map::scan_proc_vertex(Lexer& lexer, boost::shared_ptr<Surface> surface, int index)
{
    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    Position position;

    float value;
    if(!lexer.float_literal(value)) {
        return false;
    }
    position.x(value);

    if(!lexer.float_literal(value)) {
        return false;
    }
    position.y(value);

    if(!lexer.float_literal(value)) {
        return false;
    }
    position.z(value);

    float texture_u;
    if(!lexer.float_literal(texture_u)) {
        return false;
    }

    float texture_v;
    if(!lexer.float_literal(texture_v)) {
        return false;
    }

    Vector3 normal;

    if(!lexer.float_literal(value)) {
        return false;
    }
    normal.x(value);

    if(!lexer.float_literal(value)) {
        return false;
    }
    normal.y(value);

    if(!lexer.float_literal(value)) {
        return false;
    }
    normal.z(value);

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    Vertex& vertex(surface->vertices[index]);
    vertex.index = index;
    vertex.position = swizzle(position);
    vertex.normal = swizzle(normal);
    vertex.texture_coords = Vector2(texture_u, texture_v);
    surface->bounds.update(vertex.position);

    return true;
}

bool D3Map::scan_proc_portals(Lexer& lexer)
{
    if(!lexer.match(PORTALS)) {
        return false;
    }

    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    int area_count;
    if(!lexer.int_literal(area_count)) {
        return false;
    }

    if(area_count != _acount) {
        LOG_ERROR("Area count mismatch!\n");
        return false;
    }

    int portal_count;
    if(!lexer.int_literal(portal_count)) {
        return false;
    }

    for(int i=0; i<portal_count; ++i) {
        if(!scan_proc_portal(lexer)) {
            return false;
        }
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

    return true;
}

bool D3Map::scan_proc_portal(Lexer& lexer)
{
    int vertex_count;
    if(!lexer.int_literal(vertex_count)) {
        return false;
    }

    int positive_area;
    if(!lexer.int_literal(positive_area)) {
        return false;
    }

    int negative_area;
    if(!lexer.int_literal(negative_area)) {
        return false;
    }

    for(int i=0; i<vertex_count; ++i) {
        if(!lexer.match(OPEN_PAREN)) {
            return false;
        }

        float x;
        if(!lexer.float_literal(x)) {
            return false;
        }

        float y;
        if(!lexer.float_literal(y)) {
            return false;
        }

        float z;
        if(!lexer.float_literal(z)) {
            return false;
        }

//LOG_ERROR("TODO: store portal!");

        if(!lexer.match(CLOSE_PAREN)) {
            return false;
        }
    }

    return true;
}

bool D3Map::scan_proc_nodes(Lexer& lexer)
{
    if(!lexer.match(NODES)) {
        return false;
    }

    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    int node_count;
    if(!lexer.int_literal(node_count)) {
        return false;
    }

    for(int i=0; i<node_count; ++i) {
        if(!scan_proc_node(lexer)) {
            return false;
        }
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

    return true;
}

bool D3Map::scan_proc_node(Lexer& lexer)
{
    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    float x;
    if(!lexer.float_literal(x)) {
        return false;
    }

    float y;
    if(!lexer.float_literal(y)) {
        return false;
    }

    float z;
    if(!lexer.float_literal(z)) {
        return false;
    }

    float d;
    if(!lexer.float_literal(d)) {
        return false;
    }

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    int pos_child;
    if(!lexer.int_literal(pos_child)) {
        return false;
    }

    int neg_child;
    if(!lexer.int_literal(neg_child)) {
        return false;
    }

//LOG_ERROR("TODO: store node!");

    return true;
}

bool D3Map::scan_proc_shadow_model(Lexer& lexer)
{
    if(!lexer.match(SHADOW_MODEL)) {
        return false;
    }

    if(!lexer.match(OPEN_BRACE)) {
        return false;
    }

    std::string name;
    if(!lexer.string_literal(name)) {
        return false;
    }

    int vertex_count;
    if(!lexer.int_literal(vertex_count)) {
        return false;
    }

    int cap_count;
    if(!lexer.int_literal(cap_count)) {
        return false;
    }

    int front_cap_count;
    if(!lexer.int_literal(front_cap_count)) {
        return false;
    }

    int index_count;
    if(!lexer.int_literal(index_count)) {
        return false;
    }

    int plane_bits;
    if(!lexer.int_literal(plane_bits)) {
        return false;
    }

    for(int i=0; i<vertex_count; ++i) {
        if(!scan_proc_shadow_vertex(lexer)) {
            return false;
        }
    }

    for(int i=0; i<index_count; ++i) {
        int index;
        if(!lexer.int_literal(index)) {
            return false;
        }
    }

    if(!lexer.match(CLOSE_BRACE)) {
        return false;
    }

//LOG_ERROR("TODO: store shadow model info!\n");

    return true;
}

bool D3Map::scan_proc_shadow_vertex(Lexer& lexer)
{
    if(!lexer.match(OPEN_PAREN)) {
        return false;
    }

    float value;
    if(!lexer.float_literal(value)) {
        return false;
    }

    if(!lexer.float_literal(value)) {
        return false;
    }

    if(!lexer.float_literal(value)) {
        return false;
    }

    if(!lexer.match(CLOSE_PAREN)) {
        return false;
    }

    return true;
}

}
