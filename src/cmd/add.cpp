#include "add.hpp"
#include "../utils/write_blob.hpp"
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

bool should_ignore(const fs::path &path){
    for (const auto& part : path) {
            if (part == ".git") {
                return true;
            }
        }

    std::string filename = path.filename().string();
    if (filename == "moi.exe" || filename == "desktop.ini"|| filename == "your_program.sh") {
        return true;
    }
    
    return false;

}

int handle_add(const std::vector<std::string>& args){

    if (args.empty()) {
        std::cerr << "No arguments provided.\n";
        return 1;
    }

    if(args[0] == ".") {
        try{
            for(const auto& entry : fs::recursive_directory_iterator(".")){
                if(fs::is_regular_file(entry.path())){
                    //std::cout << "Checking file: " << entry.path().string() << " (Ignored: " << (should_ignore(entry.path()) ? "YES" : "NO") << ")\n"; //for debug
                    if(!should_ignore(entry.path())){
                        std::string sha = write_blob(entry.path().string());
                        std::cout<<"staged file: "<<entry.path().string() <<" -> "<<sha<<"\n";
                    }
                }
            }
        }
        catch (const fs::filesystem_error &e){
            std::cerr<<"Error staging files: "<<e.what()<<"\n";
            return 1;
        }
    }
    else{
        if(fs::exists(args[0]) && fs::is_regular_file(args[0])){
            std::string sha = write_blob(args[0]);
            std::cout<<"Staged file: "<<args[0]<<" -> "<<sha<<"\n";

        }
        else{
            std::cerr<< "Path does not exist: "<< args[0]<<"\n";
            return 1;
        }
    }
    return 0;
}