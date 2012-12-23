#include "src/pch.h"
#include <iostream>
#include "src/core/common.h"
#include "src/engine/DoomLexer.h"
#include "src/engine/Engine.h"
#include "src/engine/State.h"
#include "MD5Model.h"
#include "MD5Animation.h"

namespace energonsoftware {

MD5Animation::AnimationJoint* MD5Animation::AnimationJoint::create_array(size_t count, MemoryAllocator& allocator)
{
    AnimationJoint* joints = reinterpret_cast<AnimationJoint*>(allocator.allocate(sizeof(AnimationJoint) * count, 16));

    AnimationJoint *joint = joints, *end = joints + count;
    while(joint != end) {
        new(joint) AnimationJoint();
        joint++;
    }

    return joints;
}

void MD5Animation::AnimationJoint::destroy_array(AnimationJoint* const joints, size_t count, MemoryAllocator* const allocator)
{
    AnimationJoint* joint = joints + count;
    while(joint > joints) {
        (--joint)->~AnimationJoint();
    }
    operator delete[](joints, 16, *allocator);
}

void MD5Animation::MD5Frame::destroy(MD5Frame* const frame, MemoryAllocator* const allocator)
{
    frame->~MD5Frame();
    operator delete(frame, 16, *allocator);
}

Logger& MD5Animation::logger(Logger::instance("gled.engine.renderer.MD5Animation"));

void MD5Animation::destroy(MD5Animation* const animation, MemoryAllocator* const allocator)
{
    animation->~MD5Animation();
    operator delete(animation, 16, *allocator);
}

MD5Animation::MD5Animation(const std::string& name)
    : Animation(name), _version(0), _account(0)
{
}

MD5Animation::~MD5Animation() throw()
{
}

void MD5Animation::build_skeletons()
{
    // animations go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    LOG_INFO("Building animation skeletons...\n");
    for(size_t i=0; i<frame_count(); ++i) {
        const MD5Frame& md5frame(dynamic_cast<const MD5Frame&>(frame(i)));

        boost::shared_ptr<Skeleton> skeleton(new(16, allocator) Skeleton(), boost::bind(&Skeleton::destroy, _1, &allocator));
        for(size_t j=0; j<joint_count(); ++j) {
            const AnimationJoint& ajoint(_askeleton[j]);
            const Skeleton::Joint& bjoint(base_joint(j));

            // start with the base frame
            Position position(bjoint.position);
            Quaternion orientation(bjoint.orientation);

            // update the joint based on the frame data
            // TODO: find a better way to swizzle this
            int flag = ajoint.acflag, idx = ajoint.acstart;
            if(flag & 1) {
                position.x(md5frame.animated_components[idx]);
                idx++;
            }
            if(flag & 2) {
                position.z(-md5frame.animated_components[idx]);
                idx++;
            }
            if(flag & 4) {
                position.y(md5frame.animated_components[idx]);
                idx++;
            }
            if(flag & 8) {
                orientation.vector().x(md5frame.animated_components[idx]);
                idx++;
            }
            if(flag & 16) {
                orientation.vector().z(-md5frame.animated_components[idx]);
                idx++;
            }
            if(flag & 32) {
                orientation.vector().y(md5frame.animated_components[idx]);
            }

            orientation.compute_scalar();

            // joint depends on the parent unless it's a root
            boost::shared_ptr<Skeleton::Joint> joint(new(16, allocator) Skeleton::Joint(),
                boost::bind(&Skeleton::Joint::destroy, _1, &allocator));
            if(ajoint.parent >= 0) {
                joint->parent = ajoint.parent;

                const Skeleton::Joint& parent(skeleton->joint(ajoint.parent));
                joint->position = parent.position + (parent.orientation * position);
                joint->orientation = parent.orientation * orientation;
                joint->orientation.normalize();
            } else {
                joint->parent = -1;
                joint->position = position;
                joint->orientation = orientation;
            }
            skeleton->add_joint(joint);
        }
        add_skeleton(skeleton);
    }
}

bool MD5Animation::on_load(const boost::filesystem::path& path)
{
    boost::filesystem::path filename(animation_dir() / path / (name() + extension()));
    LOG_INFO("Loading animation from '" << filename << "'\n");

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

    // animations go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    int fcount = scan_num_frames(lexer);
    if(fcount < 0) return false;

    int jcount = scan_num_joints(lexer);
    if(jcount < 0) return false;
    _askeleton.reset(AnimationJoint::create_array(jcount, allocator),
        boost::bind(AnimationJoint::destroy_array, _1, jcount, &allocator));

    int frate = scan_frame_rate(lexer);
    if(frate <= 0) return false;
    frame_rate(frate);

    _account = scan_num_animated_components(lexer);
    if(_account < 0) return false;

    if(!scan_hierarchy(lexer, jcount)) {
        return false;
    }

    if(!scan_bounds(lexer, fcount)) {
        return false;
    }

    if(!scan_baseframe(lexer, jcount)) {
        return false;
    }

    if(!scan_frames(lexer, fcount)) {
        return false;
    }

    return true;
}

void MD5Animation::on_unload() throw()
{
    _version = 0;
    _commandline.erase();

    _askeleton.reset();
    _account = 0;
}

bool MD5Animation::scan_version(Lexer& lexer)
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

bool MD5Animation::scan_commandline(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::COMMANDLINE)) {
        return false;
    }
    return lexer.string_literal(_commandline);
}

int MD5Animation::scan_num_frames(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::NUM_FRAMES)) {
        return -1;
    }

    int value;
    if(!lexer.int_literal(value)) {
        return -1;
    }
    return value;
}

int MD5Animation::scan_num_joints(Lexer& lexer)
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

int MD5Animation::scan_frame_rate(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::FRAME_RATE)) {
        return -1;
    }

    int value;
    if(!lexer.int_literal(value)) {
        return -1;
    }
    return value;
}

int MD5Animation::scan_num_animated_components(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::NUM_ANIMATED_COMPONENTS)) {
        return -1;
    }

    int value;
    if(!lexer.int_literal(value)) {
        return -1;
    }
    return value;
}

bool MD5Animation::scan_hierarchy(Lexer& lexer, int count)
{
    if(!lexer.match(DoomLexer::HIERARCHY)) {
        return false;
    }

    if(!lexer.match(DoomLexer::OPEN_BRACE)) {
        return false;
    }

    for(int i=0; i<count; ++i) {
        // joint name
        std::string name;
        if(!lexer.string_literal(name)) {
            return false;
        }

        // joint parent
        int parent;
        if(!lexer.int_literal(parent)) {
            return false;
        }

        // animated component flag
        int flag;
        if(!lexer.int_literal(flag)) {
            return false;
        }

        // animated component start index
        int start;
        if(!lexer.int_literal(start)) {
            return false;
        }

        AnimationJoint& joint(_askeleton[i]);
        joint.name = name;
        joint.parent = parent;
        joint.acflag = flag;
        joint.acstart = start;
    }

    return lexer.match(DoomLexer::CLOSE_BRACE);
}

bool MD5Animation::scan_bounds(Lexer& lexer, int count)
{
    if(!lexer.match(DoomLexer::BOUNDS)) {
        return false;
    }

    if(!lexer.match(DoomLexer::OPEN_BRACE)) {
        return false;
    }

    for(int i=0; i<count; ++i) {
        if(!lexer.match(DoomLexer::OPEN_PAREN)) {
            return false;
        }

        // bounds min
        Point3 min;

        float value;
        if(!lexer.float_literal(value)) {
            return false;
        }
        min.x(value);

        if(!lexer.float_literal(value)) {
            return false;
        }
        min.y(value);

        if(!lexer.float_literal(value)) {
            return false;
        }
        min.z(value);

        if(!lexer.match(DoomLexer::CLOSE_PAREN)) {
            return false;
        }

        if(!lexer.match(DoomLexer::OPEN_PAREN)) {
            return false;
        }

        // bounds max
        Point3 max;

        if(!lexer.float_literal(value)) {
            return false;
        }
        max.x(value);

        if(!lexer.float_literal(value)) {
            return false;
        }
        max.y(value);

        if(!lexer.float_literal(value)) {
            return false;
        }
        max.z(value);

        if(!lexer.match(DoomLexer::CLOSE_PAREN)) {
            return false;
        }

        // animations go on the scene allocator
        MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

        // TODO: can we add the frame in more reasonable place?
        boost::shared_ptr<Frame> frame(new(16, allocator) MD5Frame(), boost::bind(&MD5Frame::destroy, _1, &allocator));
        frame->bounds = AABB(swizzle(min), swizzle(max));
        add_frame(frame);
    }

    return lexer.match(DoomLexer::CLOSE_BRACE);
}

bool MD5Animation::scan_baseframe(Lexer& lexer, int count)
{
    if(!lexer.match(DoomLexer::BASE_FRAME)) {
        return false;
    }

    if(!lexer.match(DoomLexer::OPEN_BRACE)) {
        return false;
    }

    // animations go on the scene allocator
    MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

    for(int i=0; i<count; ++i) {
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

        boost::shared_ptr<Skeleton::Joint> joint(new(16, allocator) Skeleton::Joint(),
            boost::bind(&Skeleton::Joint::destroy, _1, &allocator));
        joint->position = swizzle(position);
        joint->orientation = Quaternion(swizzle(orientation));
        add_base_joint(joint);
    }

    return lexer.match(DoomLexer::CLOSE_BRACE);
}

bool MD5Animation::scan_frames(Lexer& lexer, int count)
{
    for(int i=0; i<count; ++i) {
        if(!lexer.match(DoomLexer::FRAME)) {
            return false;
        }

        // frame index
        int index;
        if(!lexer.int_literal(index)) {
            return false;
        }

        if(!lexer.match(DoomLexer::OPEN_BRACE)) {
            return false;
        }

        // animations go on the scene allocator
        MemoryAllocator& allocator(Engine::instance().state().scene().allocator());

        // animated components
        boost::shared_array<float> animated_components(new(allocator) float[_account], boost::bind(&MemoryAllocator::release, &allocator, _1));
        for(int j=0; j<_account; ++j) {
            float value;
            if(!lexer.float_literal(value)) {
                return false;
            }
            animated_components[j] = value;
        }

        if(!lexer.match(DoomLexer::CLOSE_BRACE)) {
            return false;
        }

        MD5Frame& md5frame(dynamic_cast<MD5Frame&>(frame(i)));
        md5frame.animated_components = animated_components;
    }

    return true;
}

}
