#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <filesystem>
#include "../external/sha1.hpp"
#include "compress.hpp"
#include "utils.hpp"
#include <stdexcept>

using namespace std;

filesystem::path find_git_root(const filesystem::path &start_path){
    filesystem::path current = filesystem::absolute(start_path); //using absolute
    while(!filesystem::exists(current/".git")){
        if(filesystem::exists(current/"objects") && filesystem::exists(current/"refs")){
            return current;
        }

        if(current == current.root_path()){
            throw runtime_error("fatal: not inside git repo");
        }
        current = current.parent_path();
    }
    return current;
}

vector<unsigned char> hex_to_raw(const string &hex){
    if(hex.length()%2!=0){
        throw runtime_error("hex str must be event length");
    }

    vector<unsigned char>raw;
    raw.reserve(hex.length()/2);
    char byte_buffer[3] = {0,0,0};

    for(size_t i = 0;i< hex.length(); i+=2){
        byte_buffer[0] = hex[i];
        byte_buffer[1] = hex[i+1];

        raw.push_back(static_cast<unsigned char>(strtol(byte_buffer, nullptr, 16)));
    }

    return raw;
}

string get_git_author(){
    const char* env_usrname = getenv("GIT_AUTHOR_NAME");
    const char* env_email = getenv("GIT_AUTHOR_EMAIL");

    if(env_usrname && env_email){
        return string(env_usrname) + " <" + string(env_email) + ">";
    }
}

string get_current_timestamp(){
    time_t now = time(nullptr);
    struct tm* gmt  = gmtime(&now);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%s %z", gmt);
    return string(buffer);
}

string create_commit_content(const string &tree_sha, const string &parent_sha, const string &commit_msg){
    string author = get_git_author();
    string time_stamp = get_current_timestamp();
    stringstream commit;

    commit<<"tree"<< tree_sha <<"\n";
    if(!parent_sha.empty()){
        commit<<"parent "<<parent_sha<<"\n";

    }

    commit<<"author "<<author<<" "<<time_stamp<<"\n";
    commit<<"committer "<<author<<" "<<time_stamp<<"\n";
    commit<<"\n"<<commit_msg<<"\n";

    return commit.str();
}