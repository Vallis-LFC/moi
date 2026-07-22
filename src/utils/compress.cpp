#include <zlib.h>
#include <string>

bool compressData(const std::string &input, std::string &output){
    z_stream zs{};
    if (deflateInit(&zs, Z_BEST_COMPRESSION)!= Z_OK) return false;

    zs.next_in = (Bytef *)input.data();
    zs.avail_in = input.size();

    char outbuffer[32768];
    int deflateRes;
    do{
        zs.next_out = reinterpret_cast<Bytef *>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        deflateRes = deflate(&zs, Z_FINISH);

        if(deflateRes <0){
            deflateEnd(&zs);
            return false;
        }

        output.append(outbuffer, sizeof(outbuffer) - zs.avail_out);

    } while (deflateRes != Z_STREAM_END);

    deflateEnd(&zs);
    return true;
}