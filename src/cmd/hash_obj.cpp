#include "hash_obj.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>
#include "../external/sha1.hpp"
#include "../utils/compress.hpp"
#include "../utils/write_blob.hpp"
//test
using namespace std;

int handle_hash_object(int argc, char* argv[]){
    if(argc !=4){
        cerr << "Invalid arguments. Usage: " << argv[0] << " hash-object -w <file_path>\n";
        return EXIT_FAILURE;
    }

    const string flag = argv[2];
    const set<string> supported_flags = {"-w"};

    if(supported_flags.find(flag) == supported_flags.end()){
        cerr<<"invalid flag for hash-object, expected: ";
        for(const auto &f: supported_flags)
            cerr<<f<<" ";
        cerr<<"\n";
        return EXIT_FAILURE;
        
    }

    if (flag == "-w"){
        string file_path = argv[3];
        if(!filesystem::exists(file_path)){
            cerr<<"file does not exist";
            return EXIT_FAILURE;
        }

        string digest = write_blob(file_path);
        if (digest.empty()){
            cerr<<"failed to write blob for file: "<<file_path<<endl;
            return EXIT_FAILURE;
        }
        cout<< digest<< endl;

    }
    return EXIT_SUCCESS;
}