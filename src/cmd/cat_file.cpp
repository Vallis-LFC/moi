#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "cat_file.hpp"
#include "../utils/decompress.hpp"

using namespace std;

int handle_cat_file(int argc, char*argv[]){
    if(argc <=3){
        cerr << "Invalid arg, required `-p <blob_sha>`\n";
        return EXIT_FAILURE;
    }

    const string flag = argv[2];
    const set<string> supported_flags = {"-p"};

    if (supported_flags.find(flag) == supported_flags.end()){
        cerr << "Invalid flag for cat_file, expected one of ";
        for (const auto &f : supported_flags)
            cerr<< f << " ";
        cerr << "\n";
        return EXIT_FAILURE;
    }

    if (flag == "-p"){
        const string value = argv[3];
        const string dir_name = value.substr(0,2);
        const string blob_sha = value.substr(2);

        string path = ".git/objects/" + dir_name + "/" + blob_sha;
        string object_str =  decompress_git_object(path);
        if(object_str.empty()){
            cerr << "Failed to open object file \n";
            return EXIT_FAILURE;
        }

        cout << object_str.substr(object_str.find('\0')+1) << flush;

        return EXIT_SUCCESS;
    }
}