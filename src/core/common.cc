#include "src/pch.h"
#include "src/core/util/fs_util.h"
#include "common.h"

namespace energonsoftware {

boost::filesystem::path home_conf_dir()
{
    return home_dir() / ".gled";
}

Vector3 swizzle(const Vector3& v)
{
// NOTE: any changes here need to be reflected in the following methods:
// MD5Animation::build_skeletons()
// MD5Model::scan_meshes()
// D3Map::scan_proc_surface()
    return Vector3(v.x(), v.z(), -v.y());
//return Vector3(v.x(), v.y(), v.z());
}

}