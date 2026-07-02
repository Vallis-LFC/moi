#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "../utils/decompress.hpp"
#include "ls_tree.hpp"

using namespace std;

int handle_ls_tree(int argc, char* argv[]){
    if(argc<=3){
        cerr<<"not enough arguments";
        return EXIT_FAILURE;
    }

    const string flag = argv[2];
    const set<string> supported_flags = {"--name-only"};

    if(supported_flags.find(flag) == supported_flags.end()){
        cerr<<"Invalid flag for ls-tree cmd, expected one of ";
        for(const auto &f: supported_flags) cerr<<f<<" ";
        cerr<<"\n";
        return EXIT_FAILURE;
    }

    if (flag=="--name-only"){
        const string value = argv[3];
        if(value.size()<40){
            cerr<<"invalid sha1 hash length";
            return EXIT_FAILURE;
        }
        const string dir_name = value.substr(0,2);
        const string blob_sha = value.substr(2);

        string path = ".git/objects/"+dir_name+"/"+blob_sha;
        string obj_str = decompress_git_object(path);

        if(obj_str.empty()){
            cerr<<"failed to read or decompress obj file\n";
            return EXIT_FAILURE;
        }

        if(obj_str.substr(0,4)!="tree"){
            cerr<<"invalid tree obj\n";
            return EXIT_FAILURE;
        }

        size_t pos = obj_str.find('\0');
        if(pos == string::npos){
            cerr<<"Malformed tree obj";
            return EXIT_FAILURE;
        }

        pos+=1;
        set<string> filenames;

        while(pos<obj_str.size()){
            size_t mode_end = obj_str.find(' ', pos);
            if(mode_end == string::npos)break;
            pos = mode_end+1;

            size_t name_end = obj_str.find('\0', pos);
            if(name_end==string::npos) break;

            string file_name = obj_str.substr(pos, name_end-pos);
            filenames.insert(file_name);
            if(name_end+1+20>obj_str.size()){
                cerr<<"corrupted tree entry payload(truncated SHA)";
                return EXIT_FAILURE;
            }

            pos = name_end+20+1;
        }
        for(auto name: filenames) cout<< name<< "\n";
        
    }
    return EXIT_SUCCESS;
}