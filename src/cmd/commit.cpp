#include "commit.hpp"
#include "../utils/write_tree.hpp"
#include "../utils/utils.hpp"
#include "../utils/write_commit.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

int handle_commit(const std::vector<std::string>& args){
    std::string msg = "";
    for(size_t i =0; i<args.size(); ++i){
        if(args[i]=="-m" && i+1< args.size()){
            msg = args[i+1];
            break;
        }
    }

    if (msg.empty()){
        std:cerr<<"-m require non empty string \n";
        return 1;
    }

    try{
        fs::path git_root = find_git_root(".");

        std::string tree_sha = write_tree(".");
        if(tree_sha.empty()) {
            std::cerr<< "error: failed to gen tree obj\n";
            return 1;
        }

        std::string parent_sha = "";
        std::ifstream head_file(".git/HEAD");
        if(head_file.is_open()){
            std::string ref_keyword, ref_path;
            if(head_file>>ref_keyword){
                std::ifstream ref_file(".git/"+ref_path);
                if(ref_file.is_open()){
                    ref_file>> parent_sha;
                    ref_file.close();
                }
            }
            head_file.close();
        }

        std::string commit_raw_content = create_commit_content(tree_sha, parent_sha, msg);
        std::string commit_sha = create_commit(commit_raw_content);
        if(commit_sha.empty()){
            return 1;
        }

        std::ofstream update_ref(git_root / ".git/refs/heads/main");
        if(update_ref.is_open()){
            update_ref<<commit_sha;
            update_ref.close();
        }

        std::cout<<"[main "<<commit_sha.substr(0,7)<<"] "<< msg<<"\n";

    }
    catch(const std::exception &e){
        std::cerr<<e.what()<<"\n";
        return 1;
    }
    return 0;
    
}