#include "src/pch.h"
#include <iostream>
#include "src/core/common.h"
#include "src/engine/DoomLexer.h"
#include "src/engine/Engine.h"
#include "src/engine/State.h"
#include "MD5Model.h"

// NOTE: MD5s are *not* guaranteed to be closed - one-winged edges require the *first* triangle to face the light

namespace energonsoftware {

void MD5Mesh::destroy(MD5Mesh* const mesh, MemoryAllocator* const allocator)
{
    mesh->~MD5Mesh();
    operator delete(mesh, 16, *allocator);
}

MD5Mesh::MD5Mesh(const std::string& material, size_t vcount, boost::shared_array<Vertex> vertices, size_t tcount, boost::shared_array<Triangle> triangles, size_t wcount, boost::shared_array<Weight> weights)
    : Mesh(material, vcount, vertices, tcount, triangles, wcount, weights)
{
}

MD5Mesh::~MD5Mesh() throw()
{
}

Logger& MD5Model::logger(Logger::instance("gled.engine.renderer.MD5Model"));

void MD5Model::destroy(MD5Model* const model, MemoryAllocator* const allocator)
{
    model->~MD5Model();
    operator delete(model, 16, *allocator);
}

MD5Model::MD5Model(const std::string& name)
    : Model(name), _version(0)
{
}

MD5Model::~MD5Model() throw()
{
}

bool MD5Model::on_load(const boost::filesystem::path& path)
{
    boost::filesystem::path filename(model_dir() / path / (name() + extension()));
    LOG_INFO("Loading model from '" << filename << "'\n");

    DoomLexer lexer;
    if(!lexer.load(filename)) {
        return false;
    }

    if(!scan_version(lexer)) {
        return false;
    }

    if(!scan_commandline(lexer)) {
        return false;
    }

    int jcount = scan_num_joints(lexer);
    if(jcount < 0) return false;

    int mcount = scan_num_meshes(lexer);
    if(mcount < 0) return false;

    if(!scan_joints(lexer, jcount)) {
        return false;
    }

    if(!scan_meshes(lexer, mcount)) {
        return false;
    }

    return true;
}

void MD5Model::on_unload() throw()
{
    _version = 0;
    _commandline.erase();
}

bool MD5Model::scan_version(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::MD5VERSION)) {
        return false;
    }

    if(!lexer.int_literal(_version)) {
        return false;
    }

    // only version 10 is supported
    return _version == 10;
}

bool MD5Model::scan_commandline(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::COMMANDLINE)) {
        return false;
    }
    return lexer.string_literal(_commandline);
}

int MD5Model::scan_num_joints(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::NUM_JOINTS)) {
        return -1;
    }

    int value;
    if(!lexer.int_literal(value)) {
        return -1;
    }
    return value;
}

int MD5Model::scan_num_meshes(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::NUM_MESHES)) {
        return -1;
    }

    int value;
    if(!lexer.int_literal(value)) {
        return -1;
    }
    return value;
}

bool MD5Model::scan_joints(Lexer& lexer, int count)
{
    if(!lexer.match(DoomLexer::JOINTS)) {
        return false;
    }

    if(!lexer.match(DoomLexer::OPEN_BRACE)) {
        return false;
    }

    for(int i=0; i<count; ++i) {
        if(!scan_joint(lexer)) {
            return false;
        }
    }

    return lexer.match(DoomLexer::CLOSE_BRACE);
}

bool MD5Model::scan_joint(Lexer& lexer)
{
    std::string name;
    if(!lexer.string_literal(name)) {
        return false;
    }

    int parent;
    if(!lexer.int_literal(parent)) {
        return false;
    }

    if(!lexer.match(DoomLexer::OPEN_PAREN)) {
        return false;
    }

    // position
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

    if(!lexer.match(DoomLexer::CLOSE_PAREN)) {
        return false;
    }

    if(!lexer.match(DoomLexer::OPEN_PAREN)) {
        return false;
    }

    // orientation
    Vector3 orientation;

    if(!lexer.float_literal(value)) {
        return false;
    }
    orientation.x(value);

    if(!lexer.float_literal(value)) {
        return false;
    }
    orientation.y(value);

    if(!lexer.float_literal(value)) {
        return false;
    }
    orientation.z(value);

    if(!lexer.match(DoomLexer::CLOSE_PAREN)) {
        return false;
    }

    // animations go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    boost::shared_ptr<Skeleton::Joint> joint(new(16, allocator) Skeleton::Joint(),
        boost::bind(&Skeleton::Joint::destroy, _1, &allocator));
    joint->name = name;
    joint->parent = parent;
    joint->position = swizzle(position);
    joint->orientation = Quaternion(swizzle(orientation));
    skeleton().add_joint(joint);

    return true;
}

bool MD5Model::scan_meshes(Lexer& lexer, int count)
{
    LOG_INFO("Reading " << count << " meshes...\n");
    for(int i=0; i<count; ++i) {
        if(!scan_mesh(lexer)) {
            return false;
        }
    }

    return true;
}

bool MD5Model::scan_mesh(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::MESH)) {
        return false;
    }

    if(!lexer.match(DoomLexer::OPEN_BRACE)) {
        return false;
    }

    if(!lexer.match(DoomLexer::SHADER)) {
        return false;
    }

    std::string shader;
    if(!lexer.string_literal(shader)) {
        return false;
    }

    if(!lexer.match(DoomLexer::NUM_VERTS)) {
        return false;
    }

    // vertices
    int vcount;
    if(!lexer.int_literal(vcount)) {
        return false;
    }

    // meshes go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    boost::shared_array<Vertex> vertices(Vertex::create_array(vcount, allocator),
        boost::bind(&Vertex::destroy_array, _1, vcount, &allocator));
    if(!scan_vertices(lexer, vertices, vcount)) {
        return false;
    }

    // triangles
    if(!lexer.match(DoomLexer::NUM_TRIS)) {
        return false;
    }

    int tcount;
    if(!lexer.int_literal(tcount)) {
        return false;
    }

    boost::shared_array<Triangle> triangles(Triangle::create_array(tcount, allocator),
        boost::bind(&Triangle::destroy_array, _1, tcount, &allocator));
    if(!scan_triangles(lexer, vertices, triangles, tcount)) {
        return false;
    }

    // weights
    if(!lexer.match(DoomLexer::NUM_WEIGHTS)) {
        return false;
    }

    int wcount;
    if(!lexer.int_literal(wcount)) {
        return false;
    }

    boost::shared_array<Weight> weights(Weight::create_array(wcount, allocator),
        boost::bind(&Weight::destroy_array, _1, wcount, &allocator));
    if(!scan_weights(lexer, weights, wcount)) {
        return false;
    }

    if(!lexer.match(DoomLexer::CLOSE_BRACE)) {
        return false;
    }

    add_mesh(boost::shared_ptr<Mesh>(new(16, allocator) MD5Mesh(shader, vcount, vertices, tcount, triangles, wcount, weights),
        boost::bind(&MD5Mesh::destroy, _1, &allocator)), false, false);
    return true;
}

bool MD5Model::scan_vertices(Lexer& lexer, boost::shared_array<Vertex>& vertices, int count)
{
    LOG_INFO("Reading " << count << " vertices...\n");
    for(int i=0; i<count; ++i) {
        if(!lexer.match(DoomLexer::VERT)) {
            return false;
        }

        int index;
        if(!lexer.int_literal(index)) {
            return false;
        }

        if(!lexer.match(DoomLexer::OPEN_PAREN)) {
            return false;
        }

        // texture coords
        Vector2 texture_coords;

        float value;
        if(!lexer.float_literal(value)) {
            return false;
        }
        texture_coords.x(value);

        if(!lexer.float_literal(value)) {
            return false;
        }
        texture_coords.y(value);

        if(!lexer.match(DoomLexer::CLOSE_PAREN)) {
            return false;
        }

        // start weight
        int ws;
        if(!lexer.int_literal(ws)) {
            return false;
        }

        // weight count
        int wc;
        if(!lexer.int_literal(wc)) {
            return false;
        }

        // add the vertex
        Vertex& vertex(vertices[index]);
        vertex.index = index;
        vertex.texture_coords = texture_coords;
        vertex.weight_start = ws;
        vertex.weight_count = wc;
    }

    return true;
}

bool MD5Model::scan_triangles(Lexer& lexer, boost::shared_array<Vertex>& vertices, boost::shared_array<Triangle>& triangles, int count)
{
    LOG_INFO("Reading " << count << " triangles...\n");
    for(int i=0; i<count; ++i) {
        if(!lexer.match(DoomLexer::TRI)) {
            return false;
        }

        // triangle index
        int index;
        if(!lexer.int_literal(index)) {
            return false;
        }

        // vertices
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

        // add the triangle
        // TODO: find a better way to swizzle this
        Triangle& triangle(triangles[index]);
        triangle.index = index;
        triangle.v1 = c;
        triangle.v2 = b;
        triangle.v3 = a;
    }

    return true;
}

bool MD5Model::scan_weights(Lexer& lexer, boost::shared_array<Weight>& weights, int count)
{
    for(int i=0; i<count; ++i) {
        if(!lexer.match(DoomLexer::WEIGHT)) {
            return false;
        }

        // weight index
        int index;
        if(!lexer.int_literal(index)) {
            return false;
        }

        // joint index
        int ji;
        if(!lexer.int_literal(ji)) {
            return false;
        }

        // weight
        float w;
        if(!lexer.float_literal(w)) {
            return false;
        }

        if(!lexer.match(DoomLexer::OPEN_PAREN)) {
            return false;
        }

        // position
        Position pv;

        float value;
        if(!lexer.float_literal(value)) {
            return false;
        }
        pv.x(value);

        if(!lexer.float_literal(value)) {
            return false;
        }
        pv.y(value);

        if(!lexer.float_literal(value)) {
            return false;
        }
        pv.z(value);

        if(!lexer.match(DoomLexer::CLOSE_PAREN)) {
            return false;
        }

        // add the weight
        Weight& weight(weights[index]);
        weight.index = index;
        weight.joint = ji;
        weight.weight = w;
        weight.position = swizzle(pv);
    }

    return true;
}

}
