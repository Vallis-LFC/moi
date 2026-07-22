#include "../utils/compress.hpp"
#include "../external/sha1.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <vector>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

string write_blob(const string&file_path){
    ifstream input_file(file_path, ios::binary);
    if(!input_file){
        cerr<<"failed to open file"<< file_path << endl;
        return "";
    }

    string file_content{istreambuf_iterator<char>(input_file), istreambuf_iterator<char>()};
    input_file.close();

    string final_content = "blob "+ to_string(file_content.size());
    final_content.push_back('\0');
    final_content.append(file_content);

    SHA1 checksum;
    checksum.update(final_content);
    string digest = checksum.final();

    string sub_dir_name = digest.substr(0,2);
    string object_file_name = digest.substr(2);

    fs::path object_dir = fs::current_path() / ".git" / "objects" / sub_dir_name;
    fs::create_directories(object_dir);
    
    if (!fs::exists(object_dir)) {
        cerr << "CRITICAL: Windows refused to create directory: " << object_dir.string() << endl;
        exit(EXIT_FAILURE);
    }

    fs::path full_path = object_dir / object_file_name;

    // filesystem::create_directories(".git/objects/"+sub_dir_name);
    // string full_path = ".git/objects/" +sub_dir_name+"/"+object_file_name;

    string compressed_data;
    if(!compressData(final_content, compressed_data)){
        cerr<<"compression failed"<<endl;
        exit(EXIT_FAILURE);
    }

    ofstream output_file(full_path, ios::binary);
    if (!output_file){
        cerr<<"failed to create object file to path"<< full_path.string() << endl;
        exit(EXIT_FAILURE);
    }
    output_file.write(compressed_data.data(), compressed_data.size());
    output_file.close();
    return digest; //return in hex format
}