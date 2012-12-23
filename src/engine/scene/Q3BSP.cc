#include "src/pch.h"
#include <fstream>
#include <iostream>
#include "src/core/common.h"
#include "src/engine/Engine.h"
#include "src/engine/ResourceManager.h"
#include "src/engine/State.h"
#include "src/engine/renderer/Camera.h"
#include "src/engine/renderer/Renderer.h"
#include "src/engine/renderer/Shader.h"
#include "Q3BSP.h"

// http://www.mralligator.com/q3/
// http://graphics.cs.brown.edu/games/quake/quake3.html

// TODO: vertices and what not need to be swizzled

namespace energonsoftware {

Logger& Q3BSP::logger(Logger::instance("gled.engine.scene.Q3BSP"));

void Q3BSP::destroy(Q3BSP* const map, MemoryAllocator* const allocator)
{
    map->~Q3BSP();
    operator delete(map, 16, *allocator);
}

Q3BSP::Texture* Q3BSP::create_textures(size_t count, MemoryAllocator& allocator)
{
    Texture* textures = reinterpret_cast<Texture*>(allocator.allocate(sizeof(Texture) * count));

    Texture *texture = textures, *end = textures + count;
    while(texture != end) {
        new(texture) Texture();
        texture++;
    }

    return textures;
}

void Q3BSP::destroy_textures(Texture* const textures, size_t count, MemoryAllocator* const allocator)
{
    Texture* texture = textures + count;
    while(texture > textures) {
        (--texture)->~Texture();
    }
    operator delete[](textures, *allocator);
}

Q3BSP::Plane* Q3BSP::create_planes(size_t count, MemoryAllocator& allocator)
{
    Plane* planes = reinterpret_cast<Plane*>(allocator.allocate(sizeof(Plane) * count));

    Plane *plane = planes, *end = planes + count;
    while(plane != end) {
        new(plane) Plane();
        plane++;
    }

    return planes;
}

void Q3BSP::destroy_planes(Plane* const planes, size_t count, MemoryAllocator* const allocator)
{
    Plane* plane = planes + count;
    while(plane > planes) {
        (--plane)->~Plane();
    }
    operator delete[](planes, *allocator);
}

Q3BSP::Node* Q3BSP::create_nodes(size_t count, MemoryAllocator& allocator)
{
    Node* nodes = reinterpret_cast<Node*>(allocator.allocate(sizeof(Node) * count));

    Node *node = nodes, *end = nodes + count;
    while(node != end) {
        new(node) Node();
        node++;
    }

    return nodes;
}

void Q3BSP::destroy_nodes(Node* const nodes, size_t count, MemoryAllocator* const allocator)
{
    Node* node = nodes + count;
    while(node > nodes) {
        (--node)->~Node();
    }
    operator delete[](nodes, *allocator);
}

Q3BSP::Leaf* Q3BSP::create_leaves(size_t count, MemoryAllocator& allocator)
{
    Leaf* leaves = reinterpret_cast<Leaf*>(allocator.allocate(sizeof(Leaf) * count));

    Leaf *leaf = leaves, *end = leaves + count;
    while(leaf != end) {
        new(leaf) Leaf();
        leaf++;
    }

    return leaves;
}

void Q3BSP::destroy_leaves(Leaf* const leaves, size_t count, MemoryAllocator* const allocator)
{
    Leaf* leaf = leaves + count;
    while(leaf > leaves) {
        (--leaf)->~Leaf();
    }
    operator delete[](leaves, *allocator);
}

Q3BSP::LeafFace* Q3BSP::create_leaf_faces(size_t count, MemoryAllocator& allocator)
{
    LeafFace* leaf_faces = reinterpret_cast<LeafFace*>(allocator.allocate(sizeof(LeafFace) * count));

    LeafFace *leaf_face = leaf_faces, *end = leaf_faces + count;
    while(leaf_face != end) {
        new(leaf_face) LeafFace();
        leaf_face++;
    }

    return leaf_faces;
}

void Q3BSP::destroy_leaf_faces(LeafFace* const faces, size_t count, MemoryAllocator* const allocator)
{
    LeafFace* face = faces + count;
    while(face > faces) {
        (--face)->~LeafFace();
    }
    operator delete[](faces, *allocator);
}

Q3BSP::LeafBrush* Q3BSP::create_leaf_brushes(size_t count, MemoryAllocator& allocator)
{
    LeafBrush* brushes = reinterpret_cast<LeafBrush*>(allocator.allocate(sizeof(LeafBrush) * count));

    LeafBrush *brush = brushes, *end = brushes + count;
    while(brush != end) {
        new(brush) LeafBrush();
        brush++;
    }

    return brushes;
}

void Q3BSP::destroy_leaf_brushes(LeafBrush* const brushes, size_t count, MemoryAllocator* const allocator)
{
    LeafBrush* brush = brushes + count;
    while(brush > brushes) {
        (--brush)->~LeafBrush();
    }
    operator delete[](brushes, *allocator);
}

Q3BSP::Model* Q3BSP::create_models(size_t count, MemoryAllocator& allocator)
{
    Model* models = reinterpret_cast<Model*>(allocator.allocate(sizeof(Model) * count));

    Model *model = models, *end = models + count;
    while(model != end) {
        new(model) Model();
        model++;
    }

    return models;
}

void Q3BSP::destroy_models(Model* const models, size_t count, MemoryAllocator* const allocator)
{
    Model* model = models + count;
    while(model > models) {
        (--model)->~Model();
    }
    operator delete[](models, *allocator);
}

Q3BSP::Brush* Q3BSP::create_brushes(size_t count, MemoryAllocator& allocator)
{
    Brush* brushes = reinterpret_cast<Brush*>(allocator.allocate(sizeof(Brush) * count));

    Brush *brush = brushes, *end = brushes + count;
    while(brush != end) {
        new(brush) Brush();
        brush++;
    }

    return brushes;
}

void Q3BSP::destroy_brushes(Brush* const brushes, size_t count, MemoryAllocator* const allocator)
{
    Brush* brush = brushes + count;
    while(brush > brushes) {
        (--brush)->~Brush();
    }
    operator delete[](brushes, *allocator);
}

Q3BSP::BrushSide* Q3BSP::create_brush_sides(size_t count, MemoryAllocator& allocator)
{
    BrushSide* sides = reinterpret_cast<BrushSide*>(allocator.allocate(sizeof(BrushSide) * count));

    BrushSide *side = sides, *end = sides + count;
    while(side != end) {
        new(side) BrushSide();
        side++;
    }

    return sides;
}

void Q3BSP::destroy_brush_sides(BrushSide* const sides, size_t count, MemoryAllocator* const allocator)
{
    BrushSide* side = sides + count;
    while(side > sides) {
        (--side)->~BrushSide();
    }
    operator delete[](sides, *allocator);
}

Q3BSP::BSPVertex* Q3BSP::create_bsp_vertices(size_t count, MemoryAllocator& allocator)
{
    BSPVertex* vertices = reinterpret_cast<BSPVertex*>(allocator.allocate(sizeof(BSPVertex) * count));

    BSPVertex *vertex = vertices, *end = vertices + count;
    while(vertex != end) {
        new(vertex) BSPVertex();
        vertex++;
    }

    return vertices;
}

void Q3BSP::destroy_bsp_vertices(BSPVertex* const vertices, size_t count, MemoryAllocator* const allocator)
{
    BSPVertex* vertex = vertices + count;
    while(vertex > vertices) {
        (--vertex)->~BSPVertex();
    }
    operator delete[](vertices, *allocator);
}

Q3BSP::MeshVert* Q3BSP::create_mesh_vertices(size_t count, MemoryAllocator& allocator)
{
    MeshVert* vertices = reinterpret_cast<MeshVert*>(allocator.allocate(sizeof(MeshVert) * count));

    MeshVert *vertex = vertices, *end = vertices + count;
    while(vertex != end) {
        new(vertex) MeshVert();
        vertex++;
    }

    return vertices;
}

void Q3BSP::destroy_mesh_vertices(MeshVert* const vertices, size_t count, MemoryAllocator* const allocator)
{
    MeshVert* vertex = vertices + count;
    while(vertex > vertices) {
        (--vertex)->~MeshVert();
    }
    operator delete[](vertices, *allocator);
}

Q3BSP::Effect* Q3BSP::create_effects(size_t count, MemoryAllocator& allocator)
{
    Effect* effects = reinterpret_cast<Effect*>(allocator.allocate(sizeof(Effect) * count));

    Effect *effect = effects, *end = effects + count;
    while(effect != end) {
        new(effect) Effect();
        effect++;
    }

    return effects;
}

void Q3BSP::destroy_effects(Effect* const effects, size_t count, MemoryAllocator* const allocator)
{
    Effect* effect = effects + count;
    while(effect > effects) {
        (--effect)->~Effect();
    }
    operator delete[](effects, *allocator);
}

Q3BSP::Face* Q3BSP::create_faces(size_t count, MemoryAllocator& allocator)
{
    Face* faces = reinterpret_cast<Face*>(allocator.allocate(sizeof(Face) * count));

    Face *face = faces, *end = faces + count;
    while(face != end) {
        new(face) Face();
        face++;
    }

    return faces;
}

void Q3BSP::destroy_faces(Face* const faces, size_t count, MemoryAllocator* const allocator)
{
    Face* face = faces + count;
    while(face > faces) {
        (--face)->~Face();
    }
    operator delete[](faces, *allocator);
}

Q3BSP::LightMap* Q3BSP::create_light_maps(size_t count, MemoryAllocator& allocator)
{
    LightMap* maps = reinterpret_cast<LightMap*>(allocator.allocate(sizeof(LightMap) * count));

    LightMap *map = maps, *end = maps + count;
    while(map != end) {
        new(map) LightMap();
        map++;
    }

    return maps;
}

void Q3BSP::destroy_light_maps(LightMap* const maps, size_t count, MemoryAllocator* const allocator)
{
    LightMap* map = maps + count;
    while(map > maps) {
        (--map)->~LightMap();
    }
    operator delete[](maps, *allocator);
}

Q3BSP::LightVol* Q3BSP::create_light_vols(size_t count, MemoryAllocator& allocator)
{
    LightVol* vols = reinterpret_cast<LightVol*>(allocator.allocate(sizeof(LightVol) * count));

    LightVol *vol = vols, *end = vols + count;
    while(vol != end) {
        new(vol) LightVol();
        vol++;
    }

    return vols;
}

void Q3BSP::destroy_light_vols(LightVol* const vols, size_t count, MemoryAllocator* const allocator)
{
    LightVol* vol = vols + count;
    while(vol > vols) {
        (--vol)->~LightVol();
    }
    operator delete[](vols, *allocator);
}

Q3BSP::Q3BSP(const std::string& name)
    : Map(name), _leaf_count(0)
{
    ZeroMemory(_vbo, sizeof(GLuint) * VBOCount);
    ZeroMemory(&_header, sizeof(Header));

    _vis_data.n_vecs = 0;
    _vis_data.sz_vecs = 0;
}

Q3BSP::~Q3BSP() throw()
{
    unload();
}

bool Q3BSP::load(const boost::filesystem::path& path)
{
    unload();

    boost::filesystem::path filename(map_dir() / path / (name() + ".bsp"));
    LOG_INFO("Loading q3bsp from '" << filename << "'\n");

    std::ifstream f(filename.string().c_str(), std::ios::binary);
    if(!f) {
        return false;
    }

    f.read(reinterpret_cast<char*>(&_header), sizeof(Header));
    if(strncmp(_header.magic, "IBSP", 4)) {
        LOG_ERROR("Invalid magic!\n");
        return false;
    }

    if(0x2e != _header.version) {
        LOG_ERROR("Invalid version!\n");
        return false;
    }

    if(!read_entities(f)) {
        return false;
    }

    if(!read_textures(f)) {
        return false;
    }

    if(!read_planes(f)) {
        return false;
    }

    if(!read_nodes(f)) {
        return false;
    }

    if(!read_leaves(f)) {
        return false;
    }

    if(!read_leaf_faces(f)) {
        return false;
    }

    if(!read_leaf_brushes(f)) {
        return false;
    }

    if(!read_models(f)) {
        return false;
    }

    if(!read_brushes(f)) {
        return false;
    }

    if(!read_brush_sides(f)) {
        return false;
    }

    if(!read_vertices(f)) {
        return false;
    }

    if(!read_mesh_verts(f)) {
        return false;
    }

    if(!read_effects(f)) {
        return false;
    }

    if(!read_faces(f)) {
        return false;
    }

    if(!read_light_maps(f)) {
        return false;
    }

    if(!read_light_vols(f)) {
        return false;
    }

    if(!read_vis_data(f)) {
        return false;
    }

    glGenBuffers(VBOCount, _vbo);

    return true;
}

void Q3BSP::on_unload()
{
    glDeleteBuffers(VBOCount, _vbo);

    ZeroMemory(&_header, sizeof(Header));

    _entities.ents.erase();

    _textures.reset();
    _planes.reset();
    _nodes.reset();

    _leaf_count = 0;
    _leaves.reset();

    _leaf_faces.reset();
    _models.reset();
    _brushes.reset();
    _brush_sides.reset();
    _vertices.reset();
    _mesh_verts.reset();
    _effects.reset();
    _faces.reset();
    _light_maps.reset();
    _light_vols.reset();

    _vis_data.n_vecs = 0;
    _vis_data.sz_vecs = 0;
    _vis_data.vecs.reset();
}

void Q3BSP::render(const Camera& camera, Shader& shader) const
{
    std::vector<int> faces;
    visible_faces(camera, faces);

    Engine::instance().renderer().init_shader_matrices(shader);

    BOOST_FOREACH(int face, faces) {
        //render_face(_faces[face], shader);
    }
}

void Q3BSP::render(const Camera& camera, Shader& shader, const Light& light) const
{
    std::vector<int> faces;
    visible_faces(camera, faces);

    Engine::instance().renderer().init_shader_matrices(shader);
    Engine::instance().renderer().init_shader_light(shader, material(), light, camera);

    BOOST_FOREACH(int face, faces) {
        //render_face(_faces[face], shader);
    }
}

void Q3BSP::render_normals(const Camera& camera) const
{
// TODO: write meh
}

void Q3BSP::visible_faces(const Camera& camera, std::vector<int>& faces) const
{
    int leaf = find_leaf(camera.position());
    for(size_t i=0; i<_leaf_count; ++i) {
        const Leaf& tl(_leaves[i]);
        const AABB bounds(Point3(tl.mins), Point3(tl.maxs));
        if(!cluster_visible(leaf, tl.cluster) || !camera.visible(bounds)) {
            continue;
        }

        for(int j=0; j<tl.n_leaffaces; ++j) {
// TODO: can we avoid this find check by using a std::set?
            const int f = _leaf_faces[tl.leafface + j].face;
            if(faces.end() == std::find(faces.begin(), faces.end(), f)) {
                faces.push_back(f);
            }
        }
    }
}

int Q3BSP::find_leaf(const Position& pos) const
{
    int idx = 0;
    while(idx >= 0) {
        const Node& node = _nodes[idx];
        const Plane& plane = _planes[node.plane];

        // distance from pos to the plane along the plane normal
        const float d = (Vector3(plane.normal) * pos) - plane.distance;
// TODO: is 0 == front and 1 == back?
        if(d >= 0) {
            // pos is in front of the plane
            idx = node.children[0];
        } else {
            // pos is behind the plane
            idx = node.children[1];
        }
    }
    return -idx - 1;
}

bool Q3BSP::cluster_visible(int from, int cluster) const
{
    if(from < 0 || cluster < 0) {
        return true;
    }

    // vis data bitset index based on the from and the cluster
    const int idx = (from * _vis_data.sz_vecs) + (cluster >> 3);

    // check bit ('from' mod 8)
    return (_vis_data.vecs[idx] & (1 << (cluster & 7))) != 0;
}

void Q3BSP::render_face(const Face& face, Shader& shader) const
{
    // store the temporary vertex data on the frame allocator
    MemoryAllocator& allocator(Engine::instance().frame_allocator());

    size_t vcount = 0;
    boost::shared_array<float> vertices;
    boost::shared_array<float> textures;
    switch(face.type)
    {
    case 1:
    case 3:
        {
            vcount = face.n_meshverts;
            vertices.reset(new(allocator) float[vcount * 3], boost::bind(&MemoryAllocator::release, &allocator, _1));
            textures.reset(new(allocator) float[vcount * 2], boost::bind(&MemoryAllocator::release, &allocator, _1));

            // setup the vertex buffer
            float *va = vertices.get(), *ta = textures.get();
            for(int i=0; i<face.n_meshverts; ++i) {
                const MeshVert& meshvert(_mesh_verts[face.meshvert + i]);
                const BSPVertex& vertex(_vertices[face.vertex + meshvert.offset]);

                const size_t idx = i * 3;

                *(va + idx + 0) = vertex.position[0];
                *(va + idx + 1) = vertex.position[1];
                *(va + idx + 2) = vertex.position[2];

                const size_t tidx = i * 2;

                *(ta + tidx + 0) = vertex.texcoord[0][0];
                *(ta + tidx + 1) = vertex.texcoord[0][1];
            }
        }
        break;
    case 2:
        // TODO: handle patches
        return;
    case 4:
        // TODO: handle billboards
        return;
    default:
        LOG_WARNING("Unknown face type: " << face.type << "\n");
        return;
    }

    // setup the vertex array
    glBindBuffer(GL_ARRAY_BUFFER, _vbo[VertexArray]);
    glBufferData(GL_ARRAY_BUFFER, vcount * 3 * sizeof(float), vertices.get(), GL_DYNAMIC_DRAW);

    // setup the texture array
    glBindBuffer(GL_ARRAY_BUFFER, _vbo[TextureArray]);
    glBufferData(GL_ARRAY_BUFFER, vcount * 2 * sizeof(float), textures.get(), GL_DYNAMIC_DRAW);

    // setup the detail texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Engine::instance().resource_manager().texture(ResourceManager::DEFAULT_DETAIL_TEXTURE));
    shader.uniform1i("detail_texture", 0);

    // setup the normal map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Engine::instance().resource_manager().texture(ResourceManager::DEFAULT_NORMALMAP_TEXTURE));
    shader.uniform1i("normal_map", 1);

    // get the attribute locations
    GLint vloc = shader.attrib_location("vertex");
    GLint tloc = shader.attrib_location("texture_coord");

    // render the mesh
    glEnableVertexAttribArray(vloc);
    glEnableVertexAttribArray(tloc);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo[TextureArray]);
        glVertexAttribPointer(tloc, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo[VertexArray]);
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, vcount);
    glDisableVertexAttribArray(tloc);
    glDisableVertexAttribArray(vloc);
}

bool Q3BSP::read_entities(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryEntities].length;
    LOG_DEBUG("Reading entities (" << len << ")...\n");

    // store the temporary data on the frame allocator
    MemoryAllocator& allocator(Engine::instance().frame_allocator());

    boost::shared_array<char> ents(new(allocator) char[len+1], boost::bind(&MemoryAllocator::release, &allocator, _1));
    f.seekg(_header.direntries[DirEntryEntities].offset);
    f.read(ents.get(), len);
    ents[len] = '\0';

    // copy over the string
    _entities.ents = ents.get();
    return true;
}

bool Q3BSP::read_textures(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryTextures].length;
    LOG_DEBUG("Reading textures (" << len << ")...\n");

    // store the texture data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(Texture);
    _textures.reset(create_textures(count, allocator), boost::bind(&destroy_textures, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryTextures].offset);
    f.read(reinterpret_cast<char*>(_textures.get()), len);

    return true;
}

bool Q3BSP::read_planes(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryPlanes].length;
    LOG_DEBUG("Reading planes (" << len << ")...\n");

    // store the plane data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(Plane);
    _planes.reset(create_planes(count, allocator), boost::bind(&destroy_planes, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryPlanes].offset);
    f.read(reinterpret_cast<char*>(_planes.get()), len);

    return true;
}

bool Q3BSP::read_nodes(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryNodes].length;
    LOG_DEBUG("Reading nodes (" << len << ")...\n");

    // store the node data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(Node);
    _nodes.reset(create_nodes(count, allocator), boost::bind(&destroy_nodes, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryNodes].offset);
    f.read(reinterpret_cast<char*>(_nodes.get()), len);

    return true;
}

bool Q3BSP::read_leaves(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryLeaves].length;
    LOG_DEBUG("Reading leaves (" << len << ")...\n");

    // store the leaf data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    _leaf_count = len / sizeof(Leaf);
    _leaves.reset(create_leaves(_leaf_count, allocator), boost::bind(&destroy_leaves, _1, _leaf_count, &allocator));
    f.seekg(_header.direntries[DirEntryLeaves].offset);
    f.read(reinterpret_cast<char*>(_leaves.get()), len);

    return true;
}

bool Q3BSP::read_leaf_faces(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryLeafFaces].length;
    LOG_DEBUG("Reading leaf faces (" << len << ")...\n");

    // store the leaf face data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(LeafFace);
    _leaf_faces.reset(create_leaf_faces(count, allocator), boost::bind(&destroy_leaf_faces, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryLeafFaces].offset);
    f.read(reinterpret_cast<char*>(_leaf_faces.get()), len);

    return true;
}

bool Q3BSP::read_leaf_brushes(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryLeafBrushes].length;
    LOG_DEBUG("Reading leaf brushes (" << len << ")...\n");

    // store the leaf brush data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(LeafBrush);
    _leaf_brushes.reset(create_leaf_brushes(count, allocator), boost::bind(&destroy_leaf_brushes, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryLeafBrushes].offset);
    f.read(reinterpret_cast<char*>(_leaf_brushes.get()), len);

    return true;
}

bool Q3BSP::read_models(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryModels].length;
    LOG_DEBUG("Reading models (" << len << ")...\n");

    // store the model data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(Model);
    _models.reset(create_models(count, allocator), boost::bind(&destroy_models, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryModels].offset);
    f.read(reinterpret_cast<char*>(_models.get()), len);

    return true;
}

bool Q3BSP::read_brushes(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryBrushes].length;
    LOG_DEBUG("Reading brushes (" << len << ")...\n");

    // store the brush data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(Brush);
    _brushes.reset(create_brushes(count, allocator), boost::bind(&destroy_brushes, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryBrushes].offset);
    f.read(reinterpret_cast<char*>(_brushes.get()), len);

    return true;
}

bool Q3BSP::read_brush_sides(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryBrushSides].length;
    LOG_DEBUG("Reading brush sides (" << len << ")...\n");

    // store the brush side data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(BrushSides);
    _brush_sides.reset(create_brush_sides(count, allocator), boost::bind(&destroy_brush_sides, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryBrushSides].offset);
    f.read(reinterpret_cast<char*>(_brush_sides.get()), len);

    return true;
}

bool Q3BSP::read_vertices(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryVertices].length;
    LOG_DEBUG("Reading vertices (" << len << ")...\n");

    // store the vertex data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(BSPVertex);
    _vertices.reset(create_bsp_vertices(count, allocator), boost::bind(&destroy_bsp_vertices, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryVertices].offset);
    f.read(reinterpret_cast<char*>(_vertices.get()), len);

    return true;
}

bool Q3BSP::read_mesh_verts(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryMeshVerts].length;
    LOG_DEBUG("Reading mesh vertices (" << len << ")...\n");

    // store the mesh vertex data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(MeshVert);
    _mesh_verts.reset(create_mesh_vertices(count, allocator), boost::bind(&destroy_mesh_vertices, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryMeshVerts].offset);
    f.read(reinterpret_cast<char*>(_mesh_verts.get()), len);

    return true;
}

bool Q3BSP::read_effects(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryEffects].length;
    LOG_DEBUG("Reading effects (" << len << ")...\n");

    // store the effect data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(Effect);
    _effects.reset(create_effects(count, allocator), boost::bind(&destroy_effects, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryEffects].offset);
    f.read(reinterpret_cast<char*>(_effects.get()), len);

    return true;
}

bool Q3BSP::read_faces(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryFaces].length;
    LOG_DEBUG("Reading faces (" << len << ")...\n");

    // store the face data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(Face);
    _faces.reset(create_faces(count, allocator), boost::bind(&destroy_faces, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryFaces].offset);
    f.read(reinterpret_cast<char*>(_faces.get()), len);

    return true;
}

bool Q3BSP::read_light_maps(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryLightMaps].length;
    LOG_DEBUG("Reading light maps (" << len << ")...\n");

    // store the light map data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(LightMap);
    _light_maps.reset(create_light_maps(count, allocator), boost::bind(&destroy_light_maps, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryLightMaps].offset);
    f.read(reinterpret_cast<char*>(_light_maps.get()), len);

    return true;
}

bool Q3BSP::read_light_vols(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryLightVols].length;
    LOG_DEBUG("Reading light volumes (" << len << ")...\n");

    // store the light volume data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = len / sizeof(LightVol);
    _light_vols.reset(create_light_vols(count, allocator), boost::bind(&destroy_light_vols, _1, count, &allocator));
    f.seekg(_header.direntries[DirEntryLightVols].offset);
    f.read(reinterpret_cast<char*>(_light_vols.get()), len);

    return true;
}

bool Q3BSP::read_vis_data(std::ifstream& f)
{
    const size_t len = _header.direntries[DirEntryVisData].length;
    LOG_DEBUG("Reading vis data (" << len << ")...\n");

    f.seekg(_header.direntries[DirEntryVisData].offset);
    f.read(reinterpret_cast<char*>(&_vis_data.n_vecs), sizeof(int));
    f.read(reinterpret_cast<char*>(&_vis_data.sz_vecs), sizeof(int));

    // store the vis data on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    const size_t count = _vis_data.n_vecs * _vis_data.sz_vecs;
    _vis_data.vecs.reset(new(allocator) unsigned char[count], boost::bind(&MemoryAllocator::release, &allocator, _1));
    f.read(reinterpret_cast<char*>(_vis_data.vecs.get()), count);

    return true;
}

}
