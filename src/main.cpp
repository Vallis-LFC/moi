#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "cmd/init.hpp"
#include "cmd/cat_file.hpp"
#include "cmd/hash_obj.hpp"
#include "cmd/commit_tree.hpp"
#include "cmd/ls_tree.hpp"
#include "cmd/write_tree.hpp"
#include "cmd/add.hpp"
#include "cmd/commit.hpp"
#include "cmd/log.hpp"
#include "cmd/checkout.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here!\n";

    if (argc < 2) {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }
    
    std::string command = argv[1];
    
    if (command == "init") {
        return handle_init();
    } 
    else if (command == "cat-file"){
        return handle_cat_file(argc, argv);
    }
    else if (command == "hash-object"){
        return handle_hash_object(argc,argv);
    }
    else if(command == "ls-tree"){
        return handle_ls_tree(argc,argv);
    }
    else if(command == "write-tree"){
        return handle_write_tree(argc,argv);
    }
    else if(command == "commit-tree"){
        return handle_commit_tree(argc,argv);
    }
    else if(command == "add"){
        std::vector<std::string> cmd_args;
        for(int i =2; i<argc;++i){
            cmd_args.push_back(argv[i]);
        }
        return handle_add(cmd_args);
    }
    else if (command == "commit"){
        std::vector<std::string> cmd_args;
        for(int i =2; i<argc;++i){
            cmd_args.push_back(argv[i]);
        }
        return handle_commit(cmd_args);
    }
     else if (command == "log") {
        return handle_log(argc, argv);
    } else if (command == "checkout") {
        return handle_checkout(argc, argv);
    }
    else {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }

    
    return EXIT_SUCCESS;
}
