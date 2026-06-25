#include "decompress.hpp"
#include <fstream>
#include <vector>
#include <zlib.h>

std::string decompress_git_object(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return "";

    std::vector<char> compressed_data((std::istreambuf_iterator<char>(file)),
                                       std::istreambuf_iterator<char>());
    file.close();

    z_stream strm = {};
    strm.next_in = reinterpret_cast<Bytef*>(compressed_data.data());
    strm.avail_in = compressed_data.size();

    if (inflateInit(&strm) != Z_OK) return "";

    std::string decompressed_out;
    char buffer[4096];
    int ret;

    do {
        strm.next_out = reinterpret_cast<Bytef*>(buffer);
        strm.avail_out = sizeof(buffer);

        ret = inflate(&strm, Z_NO_FLUSH);
        
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || 
            ret == Z_MEM_ERROR || ret == Z_NEED_DICT) {
            inflateEnd(&strm);
            return "";
        }

        size_t unpacked_bytes = sizeof(buffer) - strm.avail_out;
        decompressed_out.append(buffer, unpacked_bytes);

    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    return decompressed_out;
}