#include "src/pch.h"
#include "src/core/common.h"
#include "src/engine/DoomLexer.h"
#include "src/engine/Engine.h"
#include "src/engine/State.h"
#include "Animation.h"

namespace energonsoftware {

Logger& Animation::logger(Logger::instance("gled.engine.renderer.Animation"));

void Animation::destroy(Animation* const animation, MemoryAllocator* const allocator)
{
    animation->~Animation();
    operator delete(animation, 16, *allocator);
}

Animation::Animation(const std::string& name)
    : _name(name), _frate(0), _fduration(0.0)
{
}

Animation::~Animation() throw()
{
    unload();
}

bool Animation::load(const boost::filesystem::path& path)
{
    unload();
    return on_load(path);
}

void Animation::unload() throw()
{
    _frames.clear();
    _skeletons.clear();

    _skeleton.reset();

    _frate = 0;
    _fduration = 0.0;

    on_unload();
}

void Animation::interpolate_skeleton(size_t current_frame, size_t next_frame, Skeleton& sk, double frame_percent) const
{
    // this is frame-local animation date, so put it on that allocator
    MemoryAllocator& allocator(Engine::instance().frame_allocator());

    const Skeleton &cframe(skeleton(current_frame)), &nframe(skeleton(next_frame));
    for(size_t i=0; i<this->joint_count(); ++i) {
        const Skeleton::Joint& cfjoint(cframe.joint(i)), nfjoint(nframe.joint(i));

        boost::shared_ptr<Skeleton::Joint> joint(new(16, allocator) Skeleton::Joint(),
            boost::bind(&Skeleton::Joint::destroy, _1, &allocator));
        joint->parent = cfjoint.parent >= 0 ? cfjoint.parent : -1;
        joint->position = cfjoint.position.lerp(nfjoint.position, frame_percent);
        joint->orientation = cfjoint.orientation.slerp(nfjoint.orientation, frame_percent);

        sk.add_joint(joint);
    }
}

void Animation::frame_rate(int rate)
{
    if(rate <= 0.0) {
        return;
    }

    _frate = rate;
    _fduration = 1.0 / rate;
}

bool Animation::on_load(const boost::filesystem::path& path)
{
    boost::filesystem::path filename(animation_dir() / path / (name() + extension()));
    LOG_INFO("Loading animation from '" << filename << "'\n");

    DoomLexer lexer;
    if(!lexer.load(filename)) {
        return false;
    }

    if(!scan_header(lexer)) {
        return false;
    }

    return true;
}

bool Animation::scan_header(Lexer& lexer)
{
    std::string header;
    if(!lexer.string_literal(header)) {
        return false;
    }

    if("MDLANIM1" == header) {
        return true;
    }

    return false;
}

}
