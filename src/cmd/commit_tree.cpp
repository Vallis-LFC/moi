#include <iostream>
#include <fstream>
#include <filesystem>
#include <set>
#include <string>
#include "../utils/decompress.hpp"
#include "commit_tree.hpp"
#include "../utils/write_blob.hpp"
#include "../utils/write_tree.hpp"
#include "../utils/write_commit.hpp"
#include "../utils/utils.hpp"

using namespace std;
// moi commit-tree <tree_sha> (-p <parent_sha>)[optional] -m <message>

int handle_commit_tree(int argc, char* argv[]){
    if(argc < 5){
        cerr << "Invalid arguments. Usage: " << argv[0] << " commit-tree <tree_sha> (-p <parent_sha>)[optional] -m <message>\n";
        return EXIT_FAILURE;
    }

    const string tree_sha = argv[2];
    string parent_sha = "";
    string commit_msg = "";

    for(int i=3; i<argc; i++){
        string flag = argv[i];
        if(flag == "-p"){
            if(i+1 >= argc){
                cerr << "Missing value for -p flag\n";
                return EXIT_FAILURE;
            }
            parent_sha = argv[++i];
        } else if(flag == "-m"){
            if(i+1 >= argc){
                cerr << "Missing value for -m flag\n";
                return EXIT_FAILURE;
            }
            commit_msg = argv[++i];
        } else {
            cerr << "Invalid flag: " << flag << "\n";
            return EXIT_FAILURE;
        }
    }

    if(commit_msg.empty()){
        cerr << "Commit message is required. Use -m <message>\n";
        return EXIT_FAILURE;
    }

    string commit_content = create_commit_content(tree_sha, parent_sha, commit_msg);
    if(commit_content.empty()){
        cerr << "Failed to create commit object\n";
        return EXIT_FAILURE;
    }

    string digest = create_commit(commit_content);
    if(digest.empty()){
        cerr << "Failed to write commit object\n";
        return EXIT_FAILURE;
    }

    cout << digest << endl;
    return EXIT_SUCCESS;
}
