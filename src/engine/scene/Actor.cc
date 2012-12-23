#include "src/pch.h"
#include "src/engine/Engine.h"
#include "src/engine/EngineConfiguration.h"
#include "src/engine/ResourceManager.h"
#include "src/engine/State.h"
#include "src/engine/renderer/Animation.h"
#include "src/engine/renderer/Renderer.h"
#include "src/engine/renderer/Shader.h"
#include "Character.h"
#include "Monster.h"
#include "Actor.h"

namespace energonsoftware {

Logger& Actor::logger(Logger::instance("gled.engine.scene.Actor"));

boost::shared_ptr<Actor> Actor::new_actor(const std::string& type, const std::string& name)
{
    // actors stored in the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    std::string scratch(boost::algorithm::to_lower_copy(type));
    if("character" == type) {
        return boost::shared_ptr<Actor>(new(16, allocator) Character(name), boost::bind(&Character::destroy, _1, &allocator));
    } else if("monster" == type) {
        return boost::shared_ptr<Actor>(new(16, allocator) Monster(name), boost::bind(&Monster::destroy, _1, &allocator));
    }
    return boost::shared_ptr<Actor>();
}

Actor::Actor(const std::string& name)
    : Renderable(name), _cframe(0), _ftime(0.0)
{
    boost::shared_ptr<GenBuffersRenderCommand> command(
        boost::dynamic_pointer_cast<GenBuffersRenderCommand, RenderCommand>(
            RenderCommand::new_render_command(RenderCommand::RC_GEN_BUFFERS)));
    command->n = SkeletonVBOCount;
    command->callback = boost::bind(&Actor::skeleton_buffer_callback, this, _1);
    Engine::instance().renderer().command_queue().push(command);
}

Actor::~Actor() throw()
{
    if(_skeleton_vbo) {
        boost::shared_ptr<DeleteBuffersRenderCommand> command(
            boost::dynamic_pointer_cast<DeleteBuffersRenderCommand, RenderCommand>(
                RenderCommand::new_render_command(RenderCommand::RC_DELETE_BUFFERS)));
        command->n = SkeletonVBOCount;
        command->buffers = _skeleton_vbo.get();
        Engine::instance().renderer().command_queue().push(command);
    }
}

void Actor::animation(boost::shared_ptr<Animation> animation)
{
    // TODO: this needs to return a bool or something

    if(!has_model()) {
        return;
    }

    if(animation->joint_count() != model().joint_count()) {
        LOG_ERROR("Animation joint_count=" << animation->joint_count()
            << ", model joint_count=" << model().joint_count() << "\n");
        return;
    }

    _animation = animation;
}

void Actor::current_frame(size_t frame)
{
    _cframe = frame % _animation->frame_count();
}

double Actor::frame_percent() const
{
    return _ftime / _animation->frame_duration();
}

void Actor::animate()
{
    _skeleton.reset();
    _animation->interpolate_skeleton(current_frame(), next_frame(), _skeleton, frame_percent());
    calculate_vertices(_skeleton);
}

size_t Actor::next_frame() const
{
    if(_cframe == _animation->frame_count() - 1) {
        return 0;
    }
    return _cframe + 1;
}

void Actor::advance_frame()
{
    _cframe++;
    if(_cframe >= _animation->frame_count()) {
        _cframe = 0;
    }
}

void Actor::render_skeleton() const
{
    Matrix4 matrix;
    transform(matrix);

    Engine::instance().renderer().push_model_matrix();
    Engine::instance().renderer().multiply_model_matrix(matrix);

    const size_t vcount = _skeleton.nonroot_joint_count() * 3 * 2;

    // build the vertex buffer on the frame allocator
    MemoryAllocator& allocator(Engine::instance().frame_allocator());
    boost::shared_array<float> v(new(allocator) float[vcount], boost::bind(&MemoryAllocator::release, &allocator, _1));
    for(size_t i=0, j=0; i<model().joint_count(); ++i) {
        const Skeleton::Joint& joint(_skeleton.joint(i));
        if(joint.parent < 0) {
            continue;
        }

        const Position& pp(_skeleton.joint(joint.parent).position);
        const Position& p(joint.position);

        const size_t idx = j * 3 * 2;
        v[idx + 0] = pp.x();
        v[idx + 1] = pp.y();
        v[idx + 2] = pp.z();
        v[idx + 3] = p.x();
        v[idx + 4] = p.y();
        v[idx + 5] = p.z();

        j++;
    }

    // setup the vertex array
    glBindBuffer(GL_ARRAY_BUFFER, _skeleton_vbo[SkeletonVertexArray]);
    glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(float), v.get(), GL_DYNAMIC_DRAW);

    boost::shared_ptr<Shader> shader(Engine::instance().resource_manager().shader("gray"));
    shader->begin();
    Engine::instance().renderer().init_shader_matrices(*shader);

    // get the attribute locations
    GLint vloc = shader->attrib_location("vertex");

    // render the skeleton
    glEnableVertexAttribArray(vloc);
        glBindBuffer(GL_ARRAY_BUFFER, _skeleton_vbo[SkeletonVertexArray]);
        glVertexAttribPointer(vloc, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_LINES, 0, _skeleton.nonroot_joint_count() * 2);
    glDisableVertexAttribArray(vloc);

    shader->end();

    Engine::instance().renderer().pop_model_matrix();
}

void Actor::skeleton_buffer_callback(boost::shared_array<GLuint> buffers)
{
    _skeleton_vbo = buffers;
}

bool Actor::on_think(double dt)
{
// TODO: this should run the current animation until it's done
// then update the position
// and switch to the idle animation
    const EngineConfiguration& config(EngineConfiguration::instance());

    if(config.rotate_actors()) {
        rotate(dt, Vector3(0.0f, 1.0f, 0.0f));
    }

    if(_animation) {
        _ftime += dt;
        while(_ftime > _animation->frame_duration()) {
            _ftime -= _animation->frame_duration();
            advance_frame();
        }
        bounds(_animation->frame(_cframe).bounds);
    } else {
        bounds(model().bounds());
    }

    return true;
}

void Actor::on_render_unlit(const Camera& camera) const
{
    Matrix4 matrix;
    transform(matrix);

    Engine::instance().renderer().push_modelview_matrix();
    Engine::instance().renderer().multiply_model_matrix(matrix);
    Engine::instance().renderer().modelview_rotation_identity();

    const AABB& bounds(relative_bounds());
    const Position& center(bounds.center());
    _nameplate.render(name(), Vector3(center.x(), bounds.maximum().y(), center.z()), camera);

    Engine::instance().renderer().pop_modelview_matrix();

    Engine::instance().renderer().push_model_matrix();
    Engine::instance().renderer().multiply_model_matrix(matrix);

    const EngineConfiguration& config(EngineConfiguration::instance());

    if(config.render_skeleton()) {
        render_skeleton();
    }

    Engine::instance().renderer().pop_model_matrix();
}

}
