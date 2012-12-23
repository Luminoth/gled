#if !defined __MATERIAL_H__
#define __MATERIAL_H__

#include "src/core/math/Vector.h"

namespace energonsoftware {

class Lexer;

class Material
{
private:
    static Logger& logger;

public:
    static void destroy(Material* const material, MemoryAllocator* const allocator);

public:
    virtual ~Material() throw();

public:
    const std::string& name() const { return _name; }

    const Color& ambient_color() const { return _ambient; }
    void ambient_color(const Color& color) { _ambient = color; }

    const Color& diffuse_color() const { return _diffuse; }
    void diffuse_color(const Color& color) { _diffuse = color; }

    const Color& specular_color() const { return _specular; }
    void specular_color(const Color& color) { _specular = color; }

    const Color& emissive_color() const { return _emissive; }
    void emissive_color(const Color& color) { _emissive = color; }

    float shininess() const { return _shininess; }
    void shininess(float shininess) { _shininess = shininess; }

    const std::string& shader() const { return _shader; }
    void shader(const std::string& shader) { _shader = shader; }

    bool has_detail_texture() const { return !_detail_texture.empty(); }
    const std::string& detail_texture() const { return _detail_texture; }
    void detail_texture(const std::string& detail_texture) { _detail_texture = detail_texture; }

    bool no_mipmap() const { return _nomipmap; }
    void no_mipmap(bool nomipmap) { _nomipmap = nomipmap; }

    bool no_compress() const { return _nocompress; }
    void no_compress(bool nocompress) { _nocompress = nocompress; }

    bool no_repeat() const { return _norepeat; }
    void no_repeat(bool norepeat) { _norepeat = norepeat; }

    bool border() const { return _border; }
    void border(bool border) { _border = border; }

    bool has_normal_map() const { return !_normal_map.empty(); }
    const std::string& normal_map() const { return _normal_map; }
    void normal_map(const std::string& normal_map) { _normal_map = normal_map; }

    bool has_specular_map() const { return !_specular_map.empty(); }
    const std::string& specular_map() const { return _specular_map; }
    void specular_map(const std::string& specular_map) { _specular_map = specular_map; }

    bool has_emission_map() const { return !_emission_map.empty(); }
    const std::string& emission_map() const { return _emission_map; }
    void emission_map(const std::string& emission_map) { _emission_map = emission_map; }

private:
    friend class ResourceManager;
    explicit Material(const std::string& name);
    bool load(const boost::filesystem::path& path);

private:
    bool scan_ambient(Lexer& lexer);
    bool scan_diffuse(Lexer& lexer);
    bool scan_specular(Lexer& lexer);
    bool scan_emissive(Lexer& lexer);
    bool scan_shininess(Lexer& lexer);
    bool scan_shader(Lexer& lexer);
    bool scan_detail_texture(Lexer& lexer);
    bool scan_normal_map(Lexer& lexer);
    bool scan_specular_map(Lexer& lexer);
    bool scan_emission_map(Lexer& lexer);

private:
    std::string _name;

    Color _ambient, _diffuse, _specular, _emissive;
    float _shininess;

    std::string _shader;

    std::string _detail_texture, _normal_map, _specular_map, _emission_map;
    bool _nomipmap, _nocompress, _norepeat, _border;

private:
    Material();
};

}

#endif
