#include "src/pch.h"
#include "src/core/common.h"
#include "src/core/math/Plane.h"
#include "src/engine/Engine.h"
#include "src/engine/EngineConfiguration.h"
#include "src/engine/ResourceManager.h"
#include "src/engine/State.h"
#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Model.h"
#include "Renderer.h"
#include "Shader.h"
#include "Renderable.h"

namespace energonsoftware {

Renderable::RenderBuffers::RenderBuffers()
{
}

Renderable::RenderBuffers::~RenderBuffers() throw()
{
}

Renderable::TextureBuffers::TextureBuffers()
{
    _buffers.reset(new GLuint[TextureBufferCount]);
    std::memset(_buffers.get(), 0, TextureBufferCount * sizeof(GLuint));
}

Renderable::TextureBuffers::~TextureBuffers() throw()
{
}

Renderable::Renderable(const std::string& name)
    : Physical(), _name(name)
{
    boost::shared_ptr<GenBuffersRenderCommand> command(
        boost::dynamic_pointer_cast<GenBuffersRenderCommand, RenderCommand>(
            RenderCommand::new_render_command(RenderCommand::RC_GEN_BUFFERS)));
    command->n = RenderBuffers::GeometryVBOCount;
    command->callback = boost::bind(&RenderBuffers::geometry_buffers_callback, &_buffers, _1);
    Engine::instance().renderer().command_queue().push(command);

    command = boost::dynamic_pointer_cast<GenBuffersRenderCommand, RenderCommand>(
        RenderCommand::new_render_command(RenderCommand::RC_GEN_BUFFERS));
    command->n = RenderBuffers::ShadowVBOCount;
    command->callback = boost::bind(&RenderBuffers::shadow_buffers_callback, &_buffers, _1);
    Engine::instance().renderer().command_queue().push(command);
}

Renderable::~Renderable() throw()
{
    if(_buffers.shadow_buffers()) {
        boost::shared_ptr<DeleteBuffersRenderCommand> command(
            boost::dynamic_pointer_cast<DeleteBuffersRenderCommand, RenderCommand>(
                RenderCommand::new_render_command(RenderCommand::RC_DELETE_BUFFERS)));
        command->n = RenderBuffers::ShadowVBOCount;
        command->buffers = _buffers.shadow_buffers();
        Engine::instance().renderer().command_queue().push(command);
    }

    if(_buffers.geometry_buffers()) {
        boost::shared_ptr<DeleteBuffersRenderCommand> command(
            boost::dynamic_pointer_cast<DeleteBuffersRenderCommand, RenderCommand>(
                RenderCommand::new_render_command(RenderCommand::RC_DELETE_BUFFERS)));
        command->n = RenderBuffers::GeometryVBOCount;
        command->buffers = _buffers.geometry_buffers();
        Engine::instance().renderer().command_queue().push(command);
    }
}

void Renderable::model(boost::shared_ptr<Model> model)
{
    _model = model;

    // geometry goes on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());
    _geometry.reset(new(allocator) Geometry(_model->triangle_count() * 3, allocator),
        boost::bind(&Geometry::destroy, _1, &allocator));

    _vertices.reset(Vertex::create_array(model->vertex_count(), allocator),
        boost::bind(&Vertex::destroy_array, _1, model->vertex_count(), &allocator));

    calculate_vertices(_model->skeleton());
}

void Renderable::init_textures()
{
    if(_model) {
        _model->init_textures();
    }
}

size_t Renderable::compute_silhouette(const Light& light)
{
    Matrix4 matrix;
    transform(matrix);

    // allocate enough space for every edge, on the frame allocator
    MemoryAllocator& allocator(Engine::instance().frame_allocator());
    const size_t vsize = model().edge_count() * 4 * 4;
    boost::shared_array<float> varray(new(allocator) float[vsize], boost::bind(&MemoryAllocator::release, &allocator, _1));

    size_t vcount = 0;
    if(typeid(light) == typeid(DirectionalLight)) {
        const DirectionalLight& directional(dynamic_cast<const DirectionalLight&>(light));
        vcount = compute_silhouette_directional(-matrix * directional.direction(), varray);
    } else if(typeid(light) == typeid(PositionalLight) || typeid(light) == typeid(SpotLight)) {
        const PositionalLight& positional(dynamic_cast<const PositionalLight&>(light));
        vcount = compute_silhouette_positional(-matrix * positional.position().homogeneous_position(), varray);
    }

    if(0 == vcount) {
        return 0;
    }

    // setup the vertex array
    glBindBuffer(GL_ARRAY_BUFFER, _buffers.shadow_vertex_array());
    glBufferData(GL_ARRAY_BUFFER, vcount * 4 * sizeof(float), varray.get(), GL_DYNAMIC_DRAW);

    return vcount;
}

bool Renderable::is_silhouette_edge(const Mesh& mesh, const Edge& edge, const Vector4& light_position, size_t vstart, bool& faces_light1) const
{
    Plane p1;
    if(edge.t1 >= 0) {
        const Triangle& t(mesh.triangle(edge.t1));
        p1 = Plane(vertex(vstart + t.v1).position, vertex(vstart + t.v2).position, vertex(vstart + t.v3).position);
    }

    Plane p2;
    if(edge.t2 >= 0) {
        const Triangle& t(mesh.triangle(edge.t2));
        p2 = Plane(vertex(vstart + t.v1).position, vertex(vstart + t.v2).position, vertex(vstart + t.v3).position);
    }

    // sort out which of the edges, if any, are facing the light
    faces_light1 = p1 * light_position > 0.0f;
    const bool faces_light2 = p2 * light_position > 0.0f;

    // two-winged edges are silhouette edges if one triangle faces towards the light and the other way
    return ((edge.t1 >= 0 && edge.t2 >= 0 && ((faces_light1 && !faces_light2) || (!faces_light1 && faces_light2)))
        // one-winged edges are only a silhouette edge if the *first* triangle faces the light
        || ((edge.t1 < 0 || edge.t2 < 0) && faces_light1));
}

size_t Renderable::compute_silhouette_directional(const Direction& light_direction, boost::shared_array<float> varray)
{
    // Mathematics for 3D Game Programming and Computer Graphics, section 10.3
    size_t vstart = 0, ecount = 0;
    float *v = varray.get();
    for(size_t i=0; i<model().mesh_count(); ++i) {
        const Mesh& mesh(model().mesh(i));

        for(size_t j=0; j<mesh.edge_count(); ++j) {
            const Edge& edge(mesh.edge(j));

            bool faces_light1;
            if(is_silhouette_edge(mesh, edge, light_direction, vstart, faces_light1)) {
                const Vertex& v1(vertex(vstart + (faces_light1 ? edge.v2 : edge.v1)));
                const Vertex& v2(vertex(vstart + (faces_light1 ? edge.v1 : edge.v2)));

                const size_t idx = ecount * 3 * 4;

                *(v + idx + 0) = v1.position.x();
                *(v + idx + 1) = v1.position.y();
                *(v + idx + 2) = v1.position.z();
                *(v + idx + 3) = 1.0f;

                *(v + idx + 4) = v2.position.x();
                *(v + idx + 5) = v2.position.y();
                *(v + idx + 6) = v2.position.z();
                *(v + idx + 7) = 1.0f;

                // third vertex is at infinity
                *(v + idx + 8) = 0.0f;
                *(v + idx + 9) = 0.0f;
                *(v + idx + 10) = 0.0f;
                *(v + idx + 11) = 0.0f;

                ecount++;
            }
        }
        vstart += mesh.vertex_count();
    }

    return ecount * 3;
}

size_t Renderable::compute_silhouette_positional(const Position& light_position, boost::shared_array<float> varray)
{
    // Mathematics for 3D Game Programming and Computer Graphics, section 10.3
    size_t vstart = 0, ecount = 0;
    float *v = varray.get();
    for(size_t i=0; i<model().mesh_count(); ++i) {
        const Mesh& mesh(model().mesh(i));

        for(size_t j=0; j<mesh.edge_count(); ++j) {
            const Edge& edge(mesh.edge(j));

            bool faces_light1;
            if(is_silhouette_edge(mesh, edge, light_position, vstart, faces_light1)) {
                const Vertex& v1(vertex(vstart + (faces_light1 ? edge.v2 : edge.v1)));
                const Vertex& v2(vertex(vstart + (faces_light1 ? edge.v1 : edge.v2)));

                const size_t idx = ecount * 4 * 4;

                *(v + idx + 0) = v1.position.x();
                *(v + idx + 1) = v1.position.y();
                *(v + idx + 2) = v1.position.z();
                *(v + idx + 3) = 1.0f;

                *(v + idx + 4) = v2.position.x();
                *(v + idx + 5) = v2.position.y();
                *(v + idx + 6) = v2.position.z();
                *(v + idx + 7) = 1.0f;

                // last two vertices are at infinity
                *(v + idx + 8) = v2.position.x();
                *(v + idx + 9) = v2.position.y();
                *(v + idx + 10) = v2.position.z();
                *(v + idx + 11) = 0.0f;

                *(v + idx + 12) = v1.position.x();
                *(v + idx + 13) = v1.position.y();
                *(v + idx + 14) = v1.position.z();
                *(v + idx + 15) = 0.0f;

                ecount++;
            }
        }
        vstart += mesh.vertex_count();
    }

    return ecount * 4;
}

void Renderable::render() const
{
    if(!ready()) {
        return;
    }

    Matrix4 matrix;
    transform(matrix);

    boost::lock_guard<boost::recursive_mutex> guard(_geometry->mutex());

    Renderer& renderer(Engine::instance().renderer());
    renderer.push_model_matrix();
    renderer.multiply_model_matrix(matrix);

    // render the meshes
    size_t tcount = 0;
    for(size_t i=0; i<model().mesh_count(); ++i) {
        const Mesh& mesh(model().mesh(i));
        render_mesh(mesh, tcount);
        tcount += mesh.triangle_count();
    }

    renderer.pop_model_matrix();
}

void Renderable::render(const Light& light, const Camera& camera) const
{
    if(!ready()) {
        return;
    }

    Matrix4 matrix;
    transform(matrix);

    boost::lock_guard<boost::recursive_mutex> guard(_geometry->mutex());

    Renderer& renderer(Engine::instance().renderer());
    renderer.push_model_matrix();
    renderer.multiply_model_matrix(matrix);

    // render the meshes
    size_t tcount = 0;
    for(size_t i=0; i<model().mesh_count(); ++i) {
        const Mesh& mesh(model().mesh(i));
        render_mesh(mesh, tcount, light, camera);
        tcount += mesh.triangle_count();
    }

    renderer.pop_model_matrix();
}

void Renderable::render_shadow(boost::shared_ptr<Shader> shader, const Light& light, const Camera& camera, size_t vcount, bool cap) const
{
    if(!ready()) {
        return;
    }

    Matrix4 matrix;
    transform(matrix);

    boost::lock_guard<boost::recursive_mutex> guard(_geometry->mutex());

    Renderer& renderer(Engine::instance().renderer());
    renderer.push_model_matrix();
    renderer.multiply_model_matrix(matrix);

    shader->begin();

    renderer.init_shader_matrices(*shader);

// TODO: this forces us to always have a shadow material... don't do that!
    renderer.init_shader_light(*shader, *(Engine::instance().resource_manager().material("shadow")), light, camera);

    if(typeid(light) == typeid(DirectionalLight)) {
        render_shadow_directional(shader, dynamic_cast<const DirectionalLight&>(light), vcount);
    } else if(typeid(light) == typeid(PositionalLight) || typeid(light) == typeid(SpotLight)) {
        render_shadow_positional(shader, dynamic_cast<const PositionalLight&>(light), vcount, cap);
    }

    shader->end();

    renderer.pop_model_matrix();
}

void Renderable::render_unlit(const Camera& camera)
{
    Matrix4 matrix;
    transform(matrix);

    boost::lock_guard<boost::recursive_mutex> guard(_geometry->mutex());

    Renderer& renderer(Engine::instance().renderer());
    renderer.push_model_matrix();
    renderer.multiply_model_matrix(matrix);

    const EngineConfiguration& config(EngineConfiguration::instance());

    if(config.render_normals()) {
        render_normals();
    }

    /*if(config.render_bounds()) {
        renderer.render_cube(relative_bounds());
    }*/

    renderer.pop_model_matrix();

    on_render_unlit(camera);
}

void Renderable::render_shadow_directional(boost::shared_ptr<Shader> shader, const DirectionalLight& light, size_t vcount) const
{
    // get the attribute locations
    GLint vloc = shader->attrib_location("vertex");

    // render the silhouette
    glEnableVertexAttribArray(vloc);
        glBindBuffer(GL_ARRAY_BUFFER, _buffers.shadow_vertex_array());
        glVertexAttribPointer(vloc, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, vcount);
    glDisableVertexAttribArray(vloc);
}

void Renderable::render_shadow_positional(boost::shared_ptr<Shader> shader, const PositionalLight& light, size_t vcount, bool cap) const
{
    shader->uniform1i("cap", cap);

    // get the attribute locations
    GLint vloc = shader->attrib_location("vertex");

    // render the silhouette
    glEnableVertexAttribArray(vloc);
        glBindBuffer(GL_ARRAY_BUFFER, _buffers.shadow_vertex_array());
        glVertexAttribPointer(vloc, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_QUADS, 0, vcount);
    glDisableVertexAttribArray(vloc);
}

void Renderable::render_mesh(const Mesh& mesh, size_t start) const
{
    boost::shared_ptr<Shader> shader(mesh.shader());
    Engine::instance().renderer().init_shader_matrices(*shader);

    // setup the detail texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.detail_texture());
    shader->uniform1i("detail_texture", 0);

    // setup the normal map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mesh.normal_map());
    shader->uniform1i("normal_map", 1);

    // setup the specular map
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mesh.specular_map());
    shader->uniform1i("specular_map", 2);

    // setup the emission map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mesh.emission_map());
    shader->uniform1i("emission_map", 3);

    // get the attribute locations
    GLint vloc = shader->attrib_location("vertex");
    GLint nloc = shader->attrib_location("normal");
    GLint tnloc = shader->attrib_location("tangent");
    GLint tloc = shader->attrib_location("texture_coord");

    // render the mesh
    glEnableVertexAttribArray(vloc);
    glEnableVertexAttribArray(nloc);
    glEnableVertexAttribArray(tnloc);
    glEnableVertexAttribArray(tloc);
        glBindBuffer(GL_ARRAY_BUFFER, _buffers.texture_array());
        glVertexAttribPointer(tloc, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _buffers.normal_array());
        glVertexAttribPointer(nloc, 3, GL_FLOAT, GL_TRUE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _buffers.tangent_array());
        glVertexAttribPointer(tnloc, 4, GL_FLOAT, GL_TRUE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _buffers.vertex_array());
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, start * 3, mesh.triangle_count() * 3);
    glDisableVertexAttribArray(tloc);
    glDisableVertexAttribArray(tnloc);
    glDisableVertexAttribArray(nloc);
    glDisableVertexAttribArray(vloc);
}

void Renderable::render_mesh(const Mesh& mesh, size_t start, const Light& light, const Camera& camera) const
{
    boost::shared_ptr<Material> material(mesh.material());
    boost::shared_ptr<Shader> shader(mesh.shader());
    Engine::instance().renderer().init_shader_matrices(*shader);
    Engine::instance().renderer().init_shader_light(*shader, *material, light, camera);

    // setup the detail texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh.detail_texture());
    shader->uniform1i("detail_texture", 0);

    // setup the normal map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mesh.normal_map());
    shader->uniform1i("normal_map", 1);

    // setup the specular map
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mesh.specular_map());
    shader->uniform1i("specular_map", 2);

    // setup the emission map
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, mesh.emission_map());
    shader->uniform1i("emission_map", 3);

    // get the attribute locations
    GLint vloc = shader->attrib_location("vertex");
    GLint nloc = shader->attrib_location("normal");
    GLint tnloc = shader->attrib_location("tangent");
    GLint tloc = shader->attrib_location("texture_coord");

    // render the mesh
    glEnableVertexAttribArray(vloc);
    glEnableVertexAttribArray(nloc);
    glEnableVertexAttribArray(tnloc);
    glEnableVertexAttribArray(tloc);
        glBindBuffer(GL_ARRAY_BUFFER, _buffers.texture_array());
        glVertexAttribPointer(tloc, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _buffers.normal_array());
        glVertexAttribPointer(nloc, 3, GL_FLOAT, GL_TRUE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _buffers.tangent_array());
        glVertexAttribPointer(tnloc, 4, GL_FLOAT, GL_TRUE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _buffers.vertex_array());
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, start * 3, mesh.triangle_count() * 3);
    glDisableVertexAttribArray(tloc);
    glDisableVertexAttribArray(tnloc);
    glDisableVertexAttribArray(nloc);
    glDisableVertexAttribArray(vloc);
}

void Renderable::render_normals() const
{
    // render the mesh normals
    size_t tcount = 0;
    for(size_t i=0; i<model().mesh_count(); ++i) {
        const Mesh& mesh(model().mesh(i));
        render_normals(mesh, tcount);
        tcount += mesh.triangle_count();
    }
}

void Renderable::render_normals(const Mesh& mesh, size_t start) const
{
    const size_t vstart = start * 3 * 2 * 3;

    // setup the normal line array
    glBindBuffer(GL_ARRAY_BUFFER, _buffers.normal_line_array());
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count() * 2 * 3 * sizeof(float),
        _geometry->normal_line_buffer().get() + vstart, GL_DYNAMIC_DRAW);

    // setup the tangent line array
    glBindBuffer(GL_ARRAY_BUFFER, _buffers.tangent_line_array());
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count() * 2 * 3 * sizeof(float),
        _geometry->tangent_line_buffer().get() + vstart, GL_DYNAMIC_DRAW);

    // render the normals
    boost::shared_ptr<Shader> rshader(Engine::instance().resource_manager().shader("red"));
    rshader->begin();
    Engine::instance().renderer().init_shader_matrices(*rshader);

    // get the attribute locations
    GLint vloc = rshader->attrib_location("vertex");

    // render the mesh
    glEnableVertexAttribArray(vloc);
        glBindBuffer(GL_ARRAY_BUFFER, _buffers.normal_line_array());
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_LINES, 0, mesh.vertex_count() * 2);
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
        glBindBuffer(GL_ARRAY_BUFFER, _buffers.tangent_line_array());
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_LINES, 0, mesh.vertex_count() * 2);
    glDisableVertexAttribArray(vloc);

    gshader->end();
}

void Renderable::calculate_vertices(const Skeleton& skeleton)
{
    _model->calculate_vertices(skeleton, _vertices, *_geometry);

    // setup the vertex array
    glBindBuffer(GL_ARRAY_BUFFER, _buffers.vertex_array());
    glBufferData(GL_ARRAY_BUFFER, _geometry->vertex_buffer_size() * sizeof(float),
        _geometry->vertex_buffer().get(), is_static() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    // setup the normal array
    glBindBuffer(GL_ARRAY_BUFFER, _buffers.normal_array());
    glBufferData(GL_ARRAY_BUFFER, _geometry->normal_buffer_size() * sizeof(float),
        _geometry->normal_buffer().get(), is_static() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    // setup the tangent array
    glBindBuffer(GL_ARRAY_BUFFER, _buffers.tangent_array());
    glBufferData(GL_ARRAY_BUFFER, _geometry->tangent_buffer_size() * sizeof(float),
        _geometry->tangent_buffer().get(), is_static() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    // setup the texture array
    glBindBuffer(GL_ARRAY_BUFFER, _buffers.texture_array());
    glBufferData(GL_ARRAY_BUFFER, _geometry->texture_buffer_size() * sizeof(float),
        _geometry->texture_buffer().get(), is_static() ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

}
