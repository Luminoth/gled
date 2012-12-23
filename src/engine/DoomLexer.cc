#include "src/pch.h"
#include "DoomLexer.h"

namespace energonsoftware {

DoomLexer::DoomLexer()
    : Lexer()
{
    set_keyword("Version", VERSION);
    set_keyword("MD5Version", MD5VERSION);
    set_keyword("commandline", COMMANDLINE);
    set_keyword("numJoints", NUM_JOINTS);
    set_keyword("numMeshes", NUM_MESHES);
    set_keyword("joints", JOINTS);
    set_keyword("mesh", MESH);
    set_keyword("shader", SHADER);
    set_keyword("numverts", NUM_VERTS);
    set_keyword("vert", VERT);
    set_keyword("numtris", NUM_TRIS);
    set_keyword("tri", TRI);
    set_keyword("numweights", NUM_WEIGHTS);
    set_keyword("weight", WEIGHT);
    set_keyword("numFrames", NUM_FRAMES);
    set_keyword("frameRate", FRAME_RATE);
    set_keyword("numAnimatedComponents", NUM_ANIMATED_COMPONENTS);
    set_keyword("hierarchy", HIERARCHY);
    set_keyword("bounds", BOUNDS);
    set_keyword("baseframe", BASE_FRAME);
    set_keyword("frame", FRAME);
    set_keyword("map", MAP);
    set_keyword("global_ambient_color", GLOBAL_AMBIENT_COLOR);
    set_keyword("models", MODELS);
    set_keyword("renderables", RENDERABLES);
    set_keyword("lights", LIGHTS);
    set_keyword("ambient", AMBIENT);
    set_keyword("diffuse", DIFFUSE);
    set_keyword("specular", SPECULAR);
    set_keyword("emissive", EMISSIVE);
    set_keyword("shininess", SHININESS);
    set_keyword("detail", DETAIL);
    set_keyword("nomipmap", NOMIPMAP);
    set_keyword("nocompress", NOCOMPRESS);
    set_keyword("norepeat", NOREPEAT);
    set_keyword("border", BORDER);
    set_keyword("normalmap", NORMALMAP);
    set_keyword("specularmap", SPECULARMAP);
    set_keyword("emissionmap", EMISSIONMAP);
    set_keyword("brushDef3", BRUSHDEF3);
    set_keyword("patchDef2", PATCHDEF2);
    set_keyword("patchDef3", PATCHDEF3);
    set_keyword("mapProcFile003", MAP_PROC_FILE);
    set_keyword("model", MODEL);
    set_keyword("interAreaPortals", PORTALS);
    set_keyword("nodes", NODES);
    set_keyword("shadowModel", SHADOW_MODEL);
    set_keyword("meshes", MESHES);
    set_keyword("material", MATERIAL);
    set_keyword("has_normals", HAS_NORMALS);
    set_keyword("has_edges", HAS_EDGES);
    set_keyword("vertices", VERTICES);
    set_keyword("triangles", TRIANGLES);
    set_keyword("vertex_shader", VERTEX_SHADER);
    set_keyword("geometry_shader", GEOMETRY_SHADER);
    set_keyword("fragment_shader", FRAGMENT_SHADER);
    set_keyword("fragment_data", FRAGMENT_DATA);
}

DoomLexer::DoomLexer(const std::string& data)
    : Lexer(data)
{
}

DoomLexer::~DoomLexer() throw()
{
}

}
