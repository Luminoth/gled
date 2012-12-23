#if !defined __RENDERER__
#define __RENDERER__

#include "src/core/math/Matrix4.h"
#include "src/engine/scene/Map.h"
#include "RenderCommandQueue.h"

namespace energonsoftware {

class AABB;
class Font;
class Geometry;
class Renderable;
class Sphere;

class Renderer
{
private:
    enum
    {
        AmbientBuffer,
        DetailBuffer,
        PickBuffer,
        BufferCount
    };

    enum
    {
        VertexArray,
        VBOCount
    };

public:
    static void destroy(Renderer* const renderer, MemoryAllocator* const allocator);

private:
    static Logger& logger;

public:
    virtual ~Renderer() throw();

public:
    int viewport_width() const { return _width; }
    int viewport_height() const { return _height; }
    Size viewport_size() const { return Size(_width, _height); }
    Size half_viewport_size() const { return Size(_width >> 1, _height >> 1); }
    int viewport_bpp() const { return _bpp; }
    int viewport_Bpp() const { return _bpp >> 3; }

    RenderCommandQueue& command_queue() { return _command_queue; }

    const Matrix4& projection_matrix() const { return _projection; }
    void push_projection_matrix();
    void pop_projection_matrix();
    void projection_identity();

    // fov is in degrees
    void perspective(float fov, float aspect, float near=0.1f, float far=1000.0f);
    void orthographic(float left, float right, float bottom, float top, float near=-1.0f, float far=1.0f);
    void frustum(float left, float right, float bottom, float top, float near=0.1f, float far=1000.0f);
    void infinite_frustum(float left, float right, float bottom, float top, float near=0.1f);

    // extracts the view frustum planes
    void frustum(Point4& left, Point4& right, Point4& bottom, Point4& top, Point4& near, Point4& far) const;

    const Matrix4& view_matrix() const { return _view; }
    void push_view_matrix() { _view_stack.push(_view); }
    void pop_view_matrix() { _view = _view_stack.top(); _view_stack.pop(); }
    void view_identity() { _view.identity(); }
    void view_rotation_identity() { _view.rotation_identity(); }

    void lookat(const Position& eye, const Position& lookat, const Direction& up=Direction(0.0f, 1.0f, 0.0f));

    const Matrix4& model_matrix() const { return _model; }
    void push_model_matrix() { _model_stack.push(_model); }
    void pop_model_matrix() { _model = _model_stack.top(); _model_stack.pop(); }
    void model_identity() { _model.identity(); }
    void model_rotation_identity() { _model.rotation_identity(); }
    void multiply_model_matrix(const Matrix4& matrix) { _model *= matrix; }

    Matrix4 modelview_matrix() const { return _view * _model; }
    void push_modelview_matrix() { push_view_matrix(); push_model_matrix(); }
    void pop_modelview_matrix() { pop_view_matrix(); pop_model_matrix(); }
    void modelview_identity() { view_identity(); model_identity(); }
    void modelview_rotation_identity() { view_rotation_identity(); model_rotation_identity(); }

    Matrix4 clipping_matrix() const { return _projection * _view; }

    Matrix4 mvp_matrix() const { return _projection * modelview_matrix(); }
    void push_mvp_matrix() { push_projection_matrix(); push_modelview_matrix(); }
    void pop_mvp_matrix() { pop_projection_matrix(); pop_modelview_matrix(); }
    void mvp_identity() { projection_identity(); modelview_identity(); }

public:
    size_t frame_count() const { return _frame_count; }

    void resize_viewport(int width, int height, int fov);

    void start_frame();
    void render_frame();
    void end_frame();


/// OLD
    void render_text(const Font& font, const Position& position, const std::string& text) const;

    /*void render(const Camera& camera);

    // this clears the current list of renderables
    // when it's finished rendering everything
    // also handles visibility determination
    void render(const Camera& camera, Map& map);

    // render simple objects (these are *extremely* slow to use)
    void render_triangle() const;
    void render_cube(const AABB& cube) const;
    void render_cube() const;
    void render_sphere(const Sphere& sphere) const;
    void render_sphere() const;

    // renders geometry
    // NOTE: this is currently incomplete and only for internal use
    void render_geometry(const Geometry& geometry, Shader& shader) const;

    // NOTE: this assumes a deferred-style shader is being used
    // that is, one that uses no matrices in order to render the quad
    void render_fullscreen_quad(Shader& shader);*/
//// END OLD

    void print_video_memory_details();
    void dump_matrices() const;

    void screenshot(const boost::filesystem::path& filename);
    void save_depth_stencil(const boost::filesystem::path& filename);
    void save_stencil(const boost::filesystem::path& filename);

    void init_shader_matrices(Shader& shader) const;
    void init_shader_ambient(Shader& shader, const Material& material) const;
    void init_shader_light(Shader& shader, const Material& material, const Light& light, const Camera& camera) const;

private:
    friend class Engine;
    bool init(int width, int height, int bpp);

private:
    bool init_framebuffers(int width, int height);
    void print_info();
    bool check_extensions();

    /*void render_ambient(const Camera& camera, Map& map) const;
    void render_shadows(const Light& light, const Camera& camera);
    void render_shadow(const Renderable& renderable, const Light& light, const Camera& camera, size_t vcount) const;
    bool require_shadow_volume_cap(const Renderable& renderable, const Light& light) const;
    void render_detail(const Camera& camera, Map& map, const Light& light) const;
    void render_unlit(const Camera& camera, const Map& map) const;
    void render_picking() const;
    void render_deferred();
    void render_transparent() const;*/

private:
    // window properties
    std::string _extensions;
    int _width, _height, _bpp;

    // stats
    double _frame_start;
    size_t _frame_count;

    // render commands
    RenderCommandQueue _command_queue;

    // matrix state
    Matrix4 _projection;
    std::stack<Matrix4> _projection_stack;

    // NOTE: all of these get pushed/popped with the projection matrix
    float _near_plane, _far_plane, _aspect_ratio, _fov;
    std::stack<float> _near_plane_stack, _far_plane_stack, _aspect_ratio_stack, _fov_stack;

    Matrix4 _view;
    std::stack<Matrix4> _view_stack;

    Matrix4 _model;
    std::stack<Matrix4> _model_stack;

    // frame buffers
    GLuint _fbo[BufferCount], _rbo[BufferCount], _tbo[BufferCount], _vbo[VBOCount];

private:
    Renderer();
    DISALLOW_COPY_AND_ASSIGN(Renderer);
};

}

#endif