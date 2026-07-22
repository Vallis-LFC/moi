#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <filesystem>
#include "write_tree.hpp"
#include "../utils/decompress.hpp"
#include "../utils/write_tree.hpp"
#include "../utils/utils.hpp"
#include "../utils/write_blob.hpp"

using namespace std;
using recursive_directory_iterator = filesystem::recursive_directory_iterator;

int handle_write_tree(int argc, char* argv[]){
    if(argc<2){
        cerr<<"usage: moi write tree"<<endl;
        return EXIT_FAILURE;
    }

    try{
        filesystem::path repo_root = find_git_root(filesystem::current_path());
        string tree_hash = write_tree(repo_root);
        cout<<tree_hash<<endl;

    }
    catch (const exception &e){
        cerr<<e.what()<<endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}