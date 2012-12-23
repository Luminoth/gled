#if !defined __RENDERCOMMANDQUEUE_H__
#define __RENDERCOMMANDQUEUE_H__

namespace energonsoftware {

class RenderCommand
{
public:
    enum RenderCommandType
    {
        RC_POLYGON_MODE,
        RC_GEN_BUFFERS,
        RC_DELETE_BUFFERS,
        RC_GEN_TEXTURES,
        RC_DELETE_TEXTURES,
        RC_GEN_TEXTURE,
        RC_DELETE_TEXTURE,
    };

    static boost::shared_ptr<RenderCommand> new_render_command(RenderCommandType type);

public:
    virtual ~RenderCommand() throw() {}

public:
    RenderCommandType type() const { return _type; }

protected:
    friend class Renderer;
    friend class RenderCommandQueue;
    virtual void handle() = 0;

private:
    RenderCommandType _type;

protected:
    explicit RenderCommand(RenderCommandType type) : _type(type) {}

private:
    RenderCommand();
};

class PolygonModeRenderCommand : public RenderCommand
{
public:
    static void destroy(PolygonModeRenderCommand* const command, MemoryAllocator* const allocator);

public:
    PolygonModeRenderCommand() : RenderCommand(RC_POLYGON_MODE), face(GL_FRONT_AND_BACK), mode(GL_FILL) {}
    virtual ~PolygonModeRenderCommand() throw() {}

private:
    virtual void handle();

public:
    GLenum face, mode;
};

class GenBuffersRenderCommand : public RenderCommand
{
public:
    static void destroy(GenBuffersRenderCommand* const command, MemoryAllocator* const allocator);

public:
    GenBuffersRenderCommand() : RenderCommand(RC_GEN_BUFFERS), n(0) {}
    virtual ~GenBuffersRenderCommand() throw() {}

private:
    virtual void handle();

public:
    GLsizei n;
    boost::function<void(boost::shared_array<GLuint>)> callback;
};

class DeleteBuffersRenderCommand : public RenderCommand
{
public:
    static void destroy(DeleteBuffersRenderCommand* const command, MemoryAllocator* const allocator);

public:
    DeleteBuffersRenderCommand() : RenderCommand(RC_DELETE_BUFFERS), n(0), buffers(NULL) {}
    virtual ~DeleteBuffersRenderCommand() throw() {}

private:
    virtual void handle();

public:
    GLsizei n;
    const GLuint* buffers;
};

class GenTexturesRenderCommand : public RenderCommand
{
public:
    static void destroy(GenTexturesRenderCommand* const command, MemoryAllocator* const allocator);

public:
    GenTexturesRenderCommand() : RenderCommand(RC_GEN_TEXTURES), n(0) {}
    virtual ~GenTexturesRenderCommand() throw() {}

private:
    virtual void handle();

public:
    GLsizei n;
    boost::function<void(boost::shared_array<GLuint>)> callback;
};

class DeleteTexturesRenderCommand : public RenderCommand
{
public:
    static void destroy(DeleteTexturesRenderCommand* const command, MemoryAllocator* const allocator);

public:
    DeleteTexturesRenderCommand() : RenderCommand(RC_DELETE_TEXTURES), n(0), textures(NULL) {}
    virtual ~DeleteTexturesRenderCommand() throw() {}

private:
    virtual void handle();

public:
    GLsizei n;
    const GLuint* textures;
};

class GenTextureRenderCommand : public RenderCommand
{
public:
    static void destroy(GenTextureRenderCommand* const command, MemoryAllocator* const allocator);

public:
    GenTextureRenderCommand() : RenderCommand(RC_GEN_TEXTURE) {}
    virtual ~GenTextureRenderCommand() throw() {}

private:
    virtual void handle();

public:
    boost::function<void(GLuint)> callback;
};

class DeleteTextureRenderCommand : public RenderCommand
{
public:
    static void destroy(DeleteTextureRenderCommand* const command, MemoryAllocator* const allocator);

public:
    DeleteTextureRenderCommand() : RenderCommand(RC_DELETE_TEXTURE), texture(0) {}
    virtual ~DeleteTextureRenderCommand() throw() {}

private:
    virtual void handle();

public:
    GLuint texture;
};

class RenderCommandQueue
{
private:
    static Logger& logger;

public:
    virtual ~RenderCommandQueue() throw();

public:
    size_t size() const { return _queue.size(); }
    bool empty() const { return _queue.empty(); }

    void push(boost::shared_ptr<RenderCommand> command);
    boost::shared_ptr<RenderCommand> pop();

private:
    boost::recursive_mutex _mutex;
    std::list<boost::shared_ptr<RenderCommand> > _queue;

private:
    friend class Renderer;
    RenderCommandQueue();
    DISALLOW_COPY_AND_ASSIGN(RenderCommandQueue);
};

}

#endif
