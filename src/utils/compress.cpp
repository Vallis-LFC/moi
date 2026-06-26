#include <zlib.h>
#include <string>

bool compressData(const std::string &input, std::string &output){
    z_stream zs{};
    if (deflateInit(&zs, Z_BEST_COMPRESSION)!= Z_OK) return false;

    zs.next_in = (B)
}