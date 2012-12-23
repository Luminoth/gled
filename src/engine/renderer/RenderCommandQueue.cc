#include "src/pch.h"
#include "src/engine/Engine.h"
#include "src/engine/ResourceManager.h"
#include "Renderable.h"
#include "Renderer.h"
#include "Shader.h"
#include "RenderCommandQueue.h"

namespace energonsoftware {

boost::shared_ptr<RenderCommand> RenderCommand::new_render_command(RenderCommandType type)
{
    // commands go on the frame allocator
    MemoryAllocator& allocator(Engine::instance().frame_allocator());
    switch(type)
    {
    case RC_POLYGON_MODE:
        return boost::shared_ptr<RenderCommand>(new(allocator) PolygonModeRenderCommand(),
            boost::bind(&PolygonModeRenderCommand::destroy, _1, &allocator));
    case RC_GEN_BUFFERS:
        return boost::shared_ptr<RenderCommand>(new(allocator) GenBuffersRenderCommand(),
            boost::bind(&GenBuffersRenderCommand::destroy, _1, &allocator));
    case RC_DELETE_BUFFERS:
        return boost::shared_ptr<RenderCommand>(new(allocator) DeleteBuffersRenderCommand(),
            boost::bind(&DeleteBuffersRenderCommand::destroy, _1, &allocator));
    case RC_GEN_TEXTURES:
        return boost::shared_ptr<RenderCommand>(new(allocator) GenTexturesRenderCommand(),
            boost::bind(&GenTexturesRenderCommand::destroy, _1, &allocator));
    case RC_DELETE_TEXTURES:
        return boost::shared_ptr<RenderCommand>(new(allocator) DeleteTexturesRenderCommand(),
            boost::bind(&DeleteTexturesRenderCommand::destroy, _1, &allocator));
    case RC_GEN_TEXTURE:
        return boost::shared_ptr<RenderCommand>(new(allocator) GenTextureRenderCommand(),
            boost::bind(&GenTextureRenderCommand::destroy, _1, &allocator));
    case RC_DELETE_TEXTURE:
        return boost::shared_ptr<RenderCommand>(new(allocator) DeleteTextureRenderCommand(),
            boost::bind(&DeleteTextureRenderCommand::destroy, _1, &allocator));
    }
    return boost::shared_ptr<RenderCommand>();
}

void PolygonModeRenderCommand::destroy(PolygonModeRenderCommand* const command, MemoryAllocator* const allocator)
{
    command->~PolygonModeRenderCommand();
    operator delete(command, *allocator);
}

void PolygonModeRenderCommand::handle()
{
    glPolygonMode(face, mode);
}

void GenBuffersRenderCommand::destroy(GenBuffersRenderCommand* const command, MemoryAllocator* const allocator)
{
    command->~GenBuffersRenderCommand();
    operator delete(command, *allocator);
}

void GenBuffersRenderCommand::handle()
{
    MemoryAllocator& allocator(Engine::instance().frame_allocator());
    boost::shared_array<GLuint> buffers(new(allocator) GLuint[n], boost::bind(&MemoryAllocator::release, &allocator, _1));
    glGenBuffers(n, buffers.get());
    callback(buffers);
}

void DeleteBuffersRenderCommand::destroy(DeleteBuffersRenderCommand* const command, MemoryAllocator* const allocator)
{
    command->~DeleteBuffersRenderCommand();
    operator delete(command, *allocator);
}

void DeleteBuffersRenderCommand::handle()
{
    glDeleteBuffers(n, buffers);
}

void GenTexturesRenderCommand::destroy(GenTexturesRenderCommand* const command, MemoryAllocator* const allocator)
{
    command->~GenTexturesRenderCommand();
    operator delete(command, *allocator);
}

void GenTexturesRenderCommand::handle()
{
    MemoryAllocator& allocator(Engine::instance().frame_allocator());
    boost::shared_array<GLuint> textures(new(allocator) GLuint[n], boost::bind(&MemoryAllocator::release, &allocator, _1));
    glGenTextures(n, textures.get());
    callback(textures);
}

void DeleteTexturesRenderCommand::destroy(DeleteTexturesRenderCommand* const command, MemoryAllocator* const allocator)
{
    command->~DeleteTexturesRenderCommand();
    operator delete(command, *allocator);
}

void DeleteTexturesRenderCommand::handle()
{
    glDeleteTextures(n, textures);
}

void GenTextureRenderCommand::destroy(GenTextureRenderCommand* const command, MemoryAllocator* const allocator)
{
    command->~GenTextureRenderCommand();
    operator delete(command, *allocator);
}

void GenTextureRenderCommand::handle()
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    callback(texture);
}

void DeleteTextureRenderCommand::destroy(DeleteTextureRenderCommand* const command, MemoryAllocator* const allocator)
{
    command->~DeleteTextureRenderCommand();
    operator delete(command, *allocator);
}

void DeleteTextureRenderCommand::handle()
{
    glDeleteTextures(1, &texture);
}

Logger& RenderCommandQueue::logger(Logger::instance("energonsoftware.engine.renderer.RenderCommandQueue"));

RenderCommandQueue::RenderCommandQueue()
{
}

RenderCommandQueue::~RenderCommandQueue() throw()
{
}

void RenderCommandQueue::push(boost::shared_ptr<RenderCommand> command)
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);
    _queue.push_back(command);
}

boost::shared_ptr<RenderCommand> RenderCommandQueue::pop()
{
    boost::lock_guard<boost::recursive_mutex> guard(_mutex);
    boost::shared_ptr<RenderCommand> ret(_queue.front());
    _queue.pop_front();
    return ret;
}

}
