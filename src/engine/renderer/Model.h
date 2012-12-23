#if !defined __MODEL_H__
#define __MODEL_H__

#include "src/core/math/Quaternion.h"
#include "src/core/math/Vector.h"
#include "src/core/physics/AABB.h"

namespace energonsoftware {

class Lexer;
class Mesh;
class Geometry;
struct Triangle;
struct Vertex;

class Skeleton
{
public:
    struct Joint
    {
        static void destroy(Joint* const joint, MemoryAllocator* const allocator);

        std::string name;
        int parent;
        Position position;
        Quaternion orientation;
    };

public:
    static void destroy(Skeleton* const skeleton, MemoryAllocator* const allocator);

public:
    Skeleton();
    virtual ~Skeleton() throw();

public:
    size_t joint_count() const { return _joints.size(); }
    size_t nonroot_joint_count() const { return _nonroot_joint_count; }

    Joint& joint(size_t idx) { return *_joints[idx]; }
    const Joint& joint(size_t idx) const { return *_joints[idx]; }

    void add_joint(boost::shared_ptr<Joint> joint);

    void reset();

private:
    std::vector<boost::shared_ptr<Joint> > _joints;

    size_t _nonroot_joint_count;

private:
    DISALLOW_COPY_AND_ASSIGN(Skeleton);
};

class Model
{
public:
    static std::string extension() { return ".mdl"; }
    static void destroy(Model* const model, MemoryAllocator* const allocator);

private:
    static Logger& logger;

public:
    explicit Model(const std::string& name);
    virtual ~Model() throw();

public:
    const std::string& name() const { return _name; }

    size_t joint_count() const { return _skeleton.joint_count(); }
    const Skeleton::Joint& joint(size_t idx) const { return _skeleton.joint(idx); }

    Skeleton& skeleton() { return _skeleton; }
    const Skeleton& skeleton() const { return _skeleton; }

    size_t mesh_count() const { return _meshes.size(); }
    const Mesh& mesh(size_t idx) const { return *(_meshes[idx]); }

    size_t vertex_count() const { return _vcount; }
    size_t triangle_count() const { return _tcount; }
    size_t edge_count() const { return _ecount; }

    // pose-position bounds
    const AABB& bounds() const { return _bounds; }

public:
    bool load(const boost::filesystem::path& path);
    void init_textures();
    void unload() throw();

    void calculate_vertices(const Skeleton& skeleton, boost::shared_array<Vertex> vertices, Geometry& geometry) const;

protected:
    void add_mesh(boost::shared_ptr<Mesh> mesh, bool has_normals, bool has_edges);

    virtual bool on_load(const boost::filesystem::path& path);
    virtual void on_unload() throw() {}

private:
    bool scan_header(Lexer& lexer);
    bool scan_meshes(Lexer& lexer);
    bool scan_mesh(Lexer& lexer);
    bool scan_vertices(Lexer& lexer, bool has_normals, boost::shared_array<Vertex>& vertices, int count);
    bool scan_triangles(Lexer& lexer, boost::shared_array<Vertex>& vertices, boost::shared_array<Triangle>& triangles, int count);

private:
    std::string _name;

    std::vector<boost::shared_ptr<Mesh> > _meshes;
    Skeleton _skeleton;

    size_t _vcount, _tcount, _ecount;

    // pose-position bounds
    AABB _bounds;

private:
    Model();
    DISALLOW_COPY_AND_ASSIGN(Model);
};

}

#endif
