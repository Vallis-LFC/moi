#include "../utils/compress.hpp"
#include "../external/sha1.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <vector>
#include <filesystem>

using namespace std;

string write_blob(const string&file_path){
    ifstream input_file(file_path, ios::binary);
    if(!input_file){
        cerr<<"failed to open file"<< file_path << endl;
        return "";
    }

    string file_content{istreambuf_iterator<char>(input_file), istreambuf_iterator<char>()};
    input_file.close();

    string final_content = "blob"+ to_string(file_content.size())+'\0'+file_content;

    SHA1 checksum;
    checksum.update(final_content);
    string digest = checksum.final();

    string sub_dir_name = digest.substr(0,2);
    string object_file_name = digest.substr(2);

    filesystem::create_directories(".git/objects/"+sub_dir_name);
    string full_path = ".git/objects/" +sub_dir_name+object_file_name;

    string compressed_data;
    if(!compressData(final_content, compressed_data)){
        cerr<<"compression failed"<<endl;
        exit(EXIT_FAILURE);
    }

    ofstream output_file(full_path, ios::binary);
    if (!output_file){
        cerr<<"failed to create object file to path"<< full_path << endl;
        exit(EXIT_FAILURE);
    }
    output_file.write(compressed_data.data(), compressed_data.size());
    output_file.close();
    return digest; //return in hex format
}