#if !defined __RENDERABLE_H__
#define __RENDERABLE_H__

#include "src/core/math/Geometry.h"
#include "src/core/physics/Physical.h"

namespace energonsoftware {

class Camera;
class Light;
class DirectionalLight;
class PositionalLight;
class Mesh;
class Model;
class Shader;
class Skeleton;

class Renderable : public Physical
{
public:
    class RenderBuffers
    {
    public:
        enum GeometryVBO
        {
            VertexArray,
            NormalArray,
            TangentArray,
            TextureArray,

            // debugging buffers
            NormalLineArray,
            TangentLineArray,

            GeometryVBOCount
        };

        enum ShadowVBO
        {
            ShadowVertexArray,
            ShadowVBOCount
        };

    public:
        virtual ~RenderBuffers() throw();

    public:
        bool ready() const { return has_geometry_buffers() && has_shadow_buffers(); }

        bool has_geometry_buffers() const { return static_cast<bool>(_geometry_buffers); }
        const GLuint* geometry_buffers() const { return _geometry_buffers.get(); }
        GLuint vertex_array() const { return _geometry_buffers[VertexArray]; }
        GLuint normal_array() const { return _geometry_buffers[NormalArray]; }
        GLuint tangent_array() const { return _geometry_buffers[TangentArray]; }
        GLuint texture_array() const { return _geometry_buffers[TextureArray]; }

        // debugging buffers
        GLuint normal_line_array() const { return _geometry_buffers[NormalLineArray]; }
        GLuint tangent_line_array() const { return _geometry_buffers[TangentLineArray]; }

        bool has_shadow_buffers() const { return static_cast<bool>(_shadow_buffers); }
        const GLuint* shadow_buffers() const { return _shadow_buffers.get(); }
        GLuint shadow_vertex_array() const { return _shadow_buffers[ShadowVertexArray]; }

    private:
        boost::shared_array<GLuint> _geometry_buffers, _shadow_buffers;

    private:
        friend class Renderable;
        RenderBuffers();

        void geometry_buffers_callback(boost::shared_array<GLuint> buffers) { _geometry_buffers = buffers; }
        void shadow_buffers_callback(boost::shared_array<GLuint> buffers) { _shadow_buffers = buffers; }
    };

    class TextureBuffers
    {
    public:
        enum TextureBuffer
        {
            DetailTexture,
            NormalMap,
            SpecularMap,
            EmissionMap,
            TextureBufferCount
        };

    public:
        virtual ~TextureBuffers() throw();

    public:
        const GLuint* buffers() const { return _buffers.get(); }
        GLuint detail_texture() const { return _buffers[DetailTexture]; }
        GLuint normal_map() const { return _buffers[NormalMap]; }
        GLuint specular_map() const { return _buffers[SpecularMap]; }
        GLuint emission_map() const { return _buffers[EmissionMap]; }

    private:
        boost::shared_array<GLuint> _buffers;

    private:
        friend class Mesh;
        friend class Renderable;
        TextureBuffers();

        void detail_texture(GLuint texture) { _buffers[DetailTexture] = texture; }
        void normal_map(GLuint texture) { _buffers[NormalMap] = texture; }
        void specular_map(GLuint texture) { _buffers[SpecularMap] = texture; }
        void emission_map(GLuint texture) { _buffers[EmissionMap] = texture; }
    };

public:
    virtual ~Renderable() throw();

public:
    const std::string& name() const { return _name; }

    bool ready() const { return _buffers.ready(); }

    void init_textures();

    void model(boost::shared_ptr<Model> model);
    const Model& model() const { return *_model; }
    bool has_model() const { return static_cast<bool>(_model); }

    const Vertex& vertex(size_t idx) const { return _vertices[idx]; }

    // NOTE: all of the following must be called from the render thread

    // returns the number of vertices in the silhouette
    // this needs to be called every frame
    size_t compute_silhouette(const Light& light);

    void render() const;
    void render(const Light& light, const Camera& camera) const;
    void render_shadow(boost::shared_ptr<Shader> shader, const Light& light, const Camera& camera, size_t vcount, bool cap) const;
    void render_unlit(const Camera& camera);

public:
    virtual bool is_transparent() const = 0;
    virtual bool is_static() const = 0;
    virtual bool has_shadow() const { return false; }

    virtual void animate() {}

protected:
    explicit Renderable(const std::string& name);
    void calculate_vertices(const Skeleton& skeleton);

    virtual void on_render_unlit(const Camera& camera) const {}

private:
    void render_mesh(const Mesh& mesh, size_t start) const;
    void render_mesh(const Mesh& mesh, size_t start, const Light& light, const Camera& camera) const;

    void render_shadow_directional(boost::shared_ptr<Shader> shader, const DirectionalLight& light, size_t vcount) const;
    void render_shadow_positional(boost::shared_ptr<Shader> shader, const PositionalLight& light, size_t vcount, bool cap) const;
    void render_normals() const;
    void render_normals(const Mesh& mesh, size_t start) const;

    bool is_silhouette_edge(const Mesh& mesh, const Edge& edge, const Vector4& light_position, size_t vstart, bool& faces_light1) const;
    size_t compute_silhouette_directional(const Direction& light_direction, boost::shared_array<float> varray);
    size_t compute_silhouette_positional(const Position& light_position, boost::shared_array<float> varray);

private:
    std::string _name;

    boost::shared_ptr<Model> _model;
    boost::shared_ptr<Geometry> _geometry;
    boost::shared_array<Vertex> _vertices;
    RenderBuffers _buffers;

private:
    Renderable();
    DISALLOW_COPY_AND_ASSIGN(Renderable);
};

struct CompareRenderablesOpaque
{
    CompareRenderablesOpaque(const Position& camera) : _camera(camera) {}
    virtual ~CompareRenderablesOpaque() throw() {}

    bool operator()(const boost::shared_ptr<const Renderable> lhs, const boost::shared_ptr<const Renderable> rhs) const
    {
        // closer objects get rendered first
        // to take advantage of early-out depth testing
        return lhs->absolute_bounds().distance(_camera) < rhs->absolute_bounds().distance(_camera);
    }

private:
    Position _camera;

private:
    CompareRenderablesOpaque();
};

struct CompareRenderablesTransparent
{
    CompareRenderablesTransparent(const Position& camera) : _camera(camera) {}
    virtual ~CompareRenderablesTransparent() throw() {}

    bool operator()(const boost::shared_ptr<const Renderable> lhs, const boost::shared_ptr<const Renderable> rhs) const
    {
        // further objects get rendered first
        return lhs->absolute_bounds().distance(_camera) > rhs->absolute_bounds().distance(_camera);
    }

private:
    Position _camera;

private:
    CompareRenderablesTransparent();
};

}

#endif
