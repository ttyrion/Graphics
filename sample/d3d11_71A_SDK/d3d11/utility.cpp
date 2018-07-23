#include "stdafx.h"
#include <fstream>
#include "utility.h"

std::string LoadShader(const std::string& cso) {
    std::ifstream ifs;
    std::string shader;
    ifs.open(cso, std::ios::binary | std::ios::in);
    if (ifs.is_open()) {
        ifs.seekg(0, std::ios_base::end);
        int size = (int)ifs.tellg();
        ifs.seekg(0, std::ios_base::beg);

        shader.resize(size);
        ifs.read(&shader[0], size);
        ifs.close();
    }

    return shader;
}