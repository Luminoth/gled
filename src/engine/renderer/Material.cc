#include "src/pch.h"
#include "src/core/common.h"
#include "src/engine/DoomLexer.h"
#include "Material.h"

namespace energonsoftware {

Logger& Material::logger(Logger::instance("gled.engine.renderer.Material"));

void Material::destroy(Material* const material, MemoryAllocator* const allocator)
{
    material->~Material();
    operator delete(material, 16, *allocator);
}

Material::Material(const std::string& name)
    : _name(name), _ambient(0.2f, 0.2f, 0.2f, 1.0f), _diffuse(0.8f, 0.8f, 0.8f, 1.0f),
        _specular(0.0f, 0.0f, 0.0f, 1.0f), _emissive(0.0f, 0.0f, 0.0f, 1.0f),
        _shininess(128.0f), _nomipmap(false), _nocompress(false), _norepeat(false), _border(false)
{
}

Material::~Material() throw()
{
}

bool Material::load(const boost::filesystem::path& path)
{
    LOG_INFO("Loading material from '" << path << "'\n");

    DoomLexer lexer;
    if(!lexer.load(path)) {
        return false;
    }

    while(!lexer.check_token(DoomLexer::END)) {
        if(lexer.check_token(DoomLexer::AMBIENT)) {
            if(!scan_ambient(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::DIFFUSE)) {
            if(!scan_diffuse(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::SPECULAR)) {
            if(!scan_specular(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::EMISSIVE)) {
            if(!scan_emissive(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::SHININESS)) {
            if(!scan_shininess(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::SHADER)) {
            if(!scan_shader(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::DETAIL)) {
            if(!scan_detail_texture(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::NORMALMAP)) {
            if(!scan_normal_map(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::SPECULARMAP)) {
            if(!scan_specular_map(lexer)) {
                return false;
            }
        } else if(lexer.check_token(DoomLexer::EMISSIONMAP)) {
            if(!scan_emission_map(lexer)) {
                return false;
            }
        }
        lexer.skip_whitespace();
    }

    return true;
}

bool Material::scan_ambient(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::AMBIENT)) {
        return false;
    }

    float r;
    if(!lexer.float_literal(r)) {
        return false;
    }

    float g;
    if(!lexer.float_literal(g)) {
        return false;
    }

    float b;
    if(!lexer.float_literal(b)) {
        return false;
    }

    float a;
    if(!lexer.float_literal(a)) {
        return false;
    }

    ambient_color(Color(r, g, b, a));
    return true;
}

bool Material::scan_diffuse(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::DIFFUSE)) {
        return false;
    }

    float r;
    if(!lexer.float_literal(r)) {
        return false;
    }

    float g;
    if(!lexer.float_literal(g)) {
        return false;
    }

    float b;
    if(!lexer.float_literal(b)) {
        return false;
    }

    float a;
    if(!lexer.float_literal(a)) {
        return false;
    }

    diffuse_color(Color(r, g, b, a));
    return true;
}

bool Material::scan_specular(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::SPECULAR)) {
        return false;
    }

    float r;
    if(!lexer.float_literal(r)) {
        return false;
    }

    float g;
    if(!lexer.float_literal(g)) {
        return false;
    }

    float b;
    if(!lexer.float_literal(b)) {
        return false;
    }

    float a;
    if(!lexer.float_literal(a)) {
        return false;
    }

    specular_color(Color(r, g, b, a));
    return true;
}

bool Material::scan_emissive(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::EMISSIVE)) {
        return false;
    }

    float r;
    if(!lexer.float_literal(r)) {
        return false;
    }

    float g;
    if(!lexer.float_literal(g)) {
        return false;
    }

    float b;
    if(!lexer.float_literal(b)) {
        return false;
    }

    float a;
    if(!lexer.float_literal(a)) {
        return false;
    }

    emissive_color(Color(r, g, b, a));
    return true;
}

bool Material::scan_shininess(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::SHININESS)) {
        return false;
    }

    float s;
    if(!lexer.float_literal(s)) {
        return false;
    }

    shininess(s);
    return true;
}

bool Material::scan_shader(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::SHADER)) {
        return false;
    }

    std::string s;
    if(!lexer.string_literal(s)) {
        return false;
    }

    shader(s);
    return true;
}

bool Material::scan_detail_texture(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::DETAIL)) {
        return false;
    }

    std::string detail;
    if(!lexer.string_literal(detail)) {
        return false;
    }

    detail_texture(detail);

    if(lexer.check_token(DoomLexer::OPEN_BRACE)) {
        lexer.match(DoomLexer::OPEN_BRACE);
        while(!lexer.check_token(DoomLexer::CLOSE_BRACE)) {
            if(lexer.check_token(DoomLexer::NOMIPMAP)) {
                lexer.match(DoomLexer::NOMIPMAP);
                no_mipmap(true);
            } else if(lexer.check_token(DoomLexer::NOCOMPRESS)) {
                lexer.match(DoomLexer::NOCOMPRESS);
                no_compress(true);
            } else if(lexer.check_token(DoomLexer::NOREPEAT)) {
                lexer.match(DoomLexer::NOREPEAT);
                no_repeat(true);
            } else if(lexer.check_token(DoomLexer::BORDER)) {
                lexer.match(DoomLexer::BORDER);
                border(true);
            }
        }
        lexer.match(DoomLexer::CLOSE_BRACE);
    }

    return true;
}

bool Material::scan_normal_map(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::NORMALMAP)) {
        return false;
    }

    std::string normal;
    if(!lexer.string_literal(normal)) {
        return false;
    }

    normal_map(normal);
    return true;
}

bool Material::scan_specular_map(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::SPECULARMAP)) {
        return false;
    }

    std::string specular;
    if(!lexer.string_literal(specular)) {
        return false;
    }

    specular_map(specular);
    return true;
}

bool Material::scan_emission_map(Lexer& lexer)
{
    if(!lexer.match(DoomLexer::EMISSIONMAP)) {
        return false;
    }

    std::string emission;
    if(!lexer.string_literal(emission)) {
        return false;
    }

    emission_map(emission);
    return true;
}

}
