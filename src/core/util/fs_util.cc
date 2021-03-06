#include "src/pch.h"
#include <fstream>
#include <iostream>
#include "fs_util.h"

namespace energonsoftware {

boost::filesystem::path home_dir()
{
    char* dir = NULL;

#if defined WIN32
    dir = "C:\\";

    TCHAR home_dir[MAX_PATH];
    ZeroMemory(home_dir, MAX_PATH * sizeof(TCHAR));

    HANDLE token = 0;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        DWORD size = MAX_PATH;
        if(GetUserProfileDirectory(token, home_dir, &size)) {
            //dir = home_dir;
        }
        CloseHandle(token);
    }
#else
    dir = std::getenv("HOME");
#endif

    return dir;
}

bool is_hidden(const boost::filesystem::path& path)
{
#if BOOST_VERSION >= 104600
    return '.' == boost::trim_left_copy(path.filename().string())[0];
#else
    return '.' == boost::trim_left_copy(path.filename())[0];
#endif
}

void list_directory(const boost::filesystem::path& path, std::vector<boost::filesystem::path>& files, bool recurse, bool hidden)
{
    if(!boost::filesystem::exists(path)) {
        return;
    }

    // can't use a recursive_directory_iterator here
    // because we need to not recurse hidden directories
    boost::filesystem::directory_iterator end;
    for(boost::filesystem::directory_iterator it(path); it != end; ++it) {
        if(boost::filesystem::is_regular_file(it->status())) {
            if(!hidden && is_hidden(it->path())) {
                continue;
            }
            files.push_back(it->path());
        } else if(recurse && boost::filesystem::is_directory(it->status())) {
            if(!hidden && is_hidden(it->path())) {
                continue;
            }
            list_directory(it->path(), files, recurse, hidden);
        }
    }
}

bool file_to_string(const boost::filesystem::path& path, std::string& str)
{
    std::ifstream file(path.string().c_str());
    if(!file.is_open()) {
        std::cerr << "file: " << path << " does NOT exist!" << std::endl;
        return false;
    }

    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return true;
}

bool file_to_strings(const boost::filesystem::path& path, std::vector<std::string>& s)
{
    std::ifstream is(path.string().c_str());
    if(!is.is_open()) {
        std::cerr << "file: " << path.string() << " does NOT exist!" << std::endl;
        return false;
    }

    while(!is.eof()) {
        std::string line;
        std::getline(is, line);
        s.push_back(line);
    }
    return true;
}

}
