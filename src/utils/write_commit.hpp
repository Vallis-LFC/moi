#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include "../utils/compress.hpp"
#include "../external/sha1.hpp"

using namespace std;

string create_commit(const string &commit_content){
    string header = "commit " + to_string(commit_content.size()) + '\0';
    string final_content = header + commit_content;

    SHA1 checksum;
    checksum.update(final_content);
    string digest = checksum.final();

    string subdir_name = digest.substr(0,2);    //slice to create subdir to distribute file across 256 diff permanent folders (up to ff)
    string commit_object_filename = digest.substr(2);

    filesystem::create_directories(".git/objects/"+subdir_name);
    string commit_object_path = ".git/objects/" + subdir_name +"/"+commit_object_filename;

    string compressed_data;
    if(!compressData(final_content, compressed_data)){
        cerr<<"compression failed"<<endl;
        return ""; //returns to main file for exit failure if nessasary (check for empty)
    }
    ofstream output_file(commit_object_path, ios::binary);  //ios::binary tells it not to touch characters as they might open in text mode by default
    if (!output_file){
        cerr<<"failed to write commit obj to db path: " <<commit_object_path<< endl;
        return "";
    }
    output_file.write(compressed_data.data(), compressed_data.size());
    output_file.close();

    return digest;

}