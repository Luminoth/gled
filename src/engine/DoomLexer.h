#if !defined __DOOMLEXER_H__
#define __DOOMLEXER_H__

#include "src/core/util/Lexer.h"

namespace energonsoftware {

class DoomLexer : public Lexer
{
public:
    enum Token
    {
        VERSION,
        MD5VERSION,
        COMMANDLINE,
        NUM_JOINTS,
        NUM_MESHES,
        JOINTS,
        MESH,
        SHADER,
        NUM_VERTS,
        VERT,
        NUM_TRIS,
        TRI,
        NUM_WEIGHTS,
        WEIGHT,
        NUM_FRAMES,
        FRAME_RATE,
        NUM_ANIMATED_COMPONENTS,
        HIERARCHY,
        BOUNDS,
        BASE_FRAME,
        FRAME,
        MAP,
        GLOBAL_AMBIENT_COLOR,
        MODELS,
        RENDERABLES,
        LIGHTS,
        AMBIENT,
        DIFFUSE,
        SPECULAR,
        EMISSIVE,
        SHININESS,
        DETAIL,
        NOMIPMAP,
        NOCOMPRESS,
        NOREPEAT,
        BORDER,
        NORMALMAP,
        SPECULARMAP,
        EMISSIONMAP,
        BRUSHDEF3,
        PATCHDEF2,
        PATCHDEF3,
        MAP_PROC_FILE,
        MODEL,
        PORTALS,
        NODES,
        SHADOW_MODEL,
        MESHES,
        MATERIAL,
        HAS_NORMALS,
        HAS_EDGES,
        VERTICES,
        TRIANGLES,
        VERTEX_SHADER,
        GEOMETRY_SHADER,
        FRAGMENT_SHADER,
        FRAGMENT_DATA,
    };

public:
    DoomLexer();
    explicit DoomLexer(const std::string& data);
    virtual ~DoomLexer() throw();

private:
    DISALLOW_COPY_AND_ASSIGN(DoomLexer);
};

}

#endif
