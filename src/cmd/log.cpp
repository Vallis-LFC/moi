#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include "../utils/utils.hpp"
#include "../utils/decompress.hpp"
#include "log.hpp"

using namespace std;
namespace fs = std::filesystem;

static string get_head_commit_sha(const fs::path &git_root){
    fs::path head_path = git_root / ".git" / "HEAD";
    if (!fs::exists(head_path)){
        head_path = git_root / "HEAD";
        if(!fs::exists(head_path)) return "";
    }

    ifstream head_file(head_path);
    string content;
    getline(head_file, content);

    if(content.rfind("ref: ", 0) == 0){
        fs::path ref_path = git_root / ".git" / content.substr(5);
        if(!fs::exists(ref_path)){
            ref_path = git_root / content.substr(5);
        }

        string ref_str = ref_path.string();
        while(!ref_str.empty() && (ref_str.back() == '\r' || ref_str.back() == '\n')){
            ref_str.pop_back();
        }
        if(!fs::exists(ref_str)) return "";

        ifstream ref_file(ref_str);
        string sha;
        ref_file >> sha;
        return sha;
    }

    while(!content.empty() && (content.back() == '\r' || content.back() == '\n')){
        content.pop_back();
    }
    return content;
}

int handle_log(int argc, char* argv[]){
    fs::path git_root;
    try{
        git_root = find_git_root(".");

    }catch (const exception &e){
        cerr<< e.what() << "\n";
        return EXIT_FAILURE;
    }

    string current_sha = get_head_commit_sha(git_root);
    if(current_sha.empty()){
        cerr<< "could not resolve head to a valid commit \n";
        return EXIT_FAILURE;
    }

    while(!current_sha.empty()){
        string dir_name = current_sha.substr(0,2);
        string blob_sha = current_sha.substr(2);
        fs::path obj_path = git_root / ".git" / "objects" / dir_name / blob_sha;

        if(!fs::exists(obj_path)){
            obj_path = git_root/ "objects" / dir_name / blob_sha;
        }

        string commit_data = decompress_git_object(obj_path.string());
        if(commit_data.empty()){
            cerr<<"failed to read commit object "<<current_sha<<"\n";
            return EXIT_FAILURE;
        }

        size_t null_pos = commit_data.find('\0');
        if(null_pos == string::npos) break;
        string body = commit_data.substr(null_pos +1);

        istringstream stream(body);
        string line;
        string parent_sha = "";
        string author_info = "";
        string msg = "";
        bool reading_msg = false;

        while(getline(stream, line)){
            if(!line.empty() && line.back() == '\r') line.pop_back();

            if(reading_msg){
                msg += line + "\n";
            } else if(line.empty()){
                reading_msg = true;
            }else if(line.rfind("parent ", 0) == 0){
                parent_sha = line.substr(7);
            }else if(line.rfind("author ", 0) == 0){
                author_info = line.substr(7);
            }
        }

        cout<<"commit "<<current_sha<<"\n";
        if(!author_info.empty()){
            cout<<"Author: "<< author_info << "\n";
        }
        cout <<"\n  "<< msg<<"\n";
        current_sha = parent_sha;
    }
    return EXIT_SUCCESS;
}