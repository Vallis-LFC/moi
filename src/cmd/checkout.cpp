#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "../utils/utils.hpp"
#include "../utils/decompress.hpp"
#include "checkout.hpp"

using namespace std;
namespace fs = std::filesystem;

static bool restore_tree(const string &tree_sha, const fs::path &git_root, const fs::path &target_dir) {
    string dir_name = tree_sha.substr(0, 2);
    string blob_sha = tree_sha.substr(2);
    fs::path obj_path = git_root / ".git" / "objects" / dir_name / blob_sha;

    if (!fs::exists(obj_path)) {
        obj_path = git_root / "objects" / dir_name / blob_sha;
    }

    string tree_data = decompress_git_object(obj_path.string());
    if (tree_data.empty()) return false;

    size_t null_pos = tree_data.find('\0');
    if (null_pos == string::npos) return false;

    string content = tree_data.substr(null_pos + 1);
    size_t pos = 0;

    while (pos < content.size()) {
        size_t space_pos = content.find(' ', pos);
        if (space_pos == string::npos) break;
        string mode = content.substr(pos, space_pos - pos);

        size_t name_null = content.find('\0', space_pos + 1);
        if (name_null == string::npos) break;
        string name = content.substr(space_pos + 1, name_null - (space_pos + 1));

        if (name_null + 1 + 20 > content.size()) break;
        string binary_sha = content.substr(name_null + 1, 20);
        pos = name_null + 1 + 20;

        char hex_sha[41];
        for (int i = 0; i < 20; ++i) {
            sprintf(hex_sha + (i * 2), "%02x", (unsigned char)binary_sha[i]);
        }
        hex_sha[40] = '\0';
        string entry_sha(hex_sha);

        fs::path entry_path = target_dir / name;

        if (mode == "40000" || mode == "040000") {
            fs::create_directories(entry_path);
            if (!restore_tree(entry_sha, git_root, entry_path)) return false;
        } else {
            string b_dir = entry_sha.substr(0, 2);
            string b_file = entry_sha.substr(2);
            fs::path b_path = git_root / ".git" / "objects" / b_dir / b_file;

            if (!fs::exists(b_path)) {
                b_path = git_root / "objects" / b_dir / b_file;
            }

            string blob_data = decompress_git_object(b_path.string());
            size_t blob_null = blob_data.find('\0');
            string file_content = (blob_null != string::npos) ? blob_data.substr(blob_null + 1) : "";

            fs::create_directories(entry_path.parent_path());
            ofstream out_file(entry_path, ios::binary);
            out_file.write(file_content.data(), file_content.size());
        }
    }
    return true;
}

int handle_checkout(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: moi checkout <commit-sha>\n";
        return EXIT_FAILURE;
    }

    fs::path git_root;
    try {
        git_root = find_git_root(".");
    } catch (const exception &e) {
        cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    string commit_sha = argv[2];
    string dir_name = commit_sha.substr(0, 2);
    string blob_sha = commit_sha.substr(2);
    fs::path path = git_root / ".git" / "objects" / dir_name / blob_sha;

    if (!fs::exists(path)) {
        path = git_root / "objects" / dir_name / blob_sha;
    }

    string commit_data = decompress_git_object(path.string());
    if (commit_data.empty()) {
        cerr << "Error: Commit " << commit_sha << " not found.\n";
        return EXIT_FAILURE;
    }

    size_t null_pos = commit_data.find('\0');
    string body = commit_data.substr(null_pos + 1);

    if (body.rfind("tree ", 0) != 0) {
        cerr << "Error: Invalid commit object format.\n";
        return EXIT_FAILURE;
    }

    string tree_sha = body.substr(5, 40);

    if (!restore_tree(tree_sha, git_root, git_root)) {
        cerr << "Error: Failed to checkout tree " << tree_sha << "\n";
        return EXIT_FAILURE;
    }

    fs::path head_path = git_root / ".git" / "HEAD";
    if (!fs::exists(head_path.parent_path())) {
        head_path = git_root / "HEAD";
    }

    ofstream head_file(head_path);
    head_file << commit_sha << "\n";

    cout << "HEAD is now at " << commit_sha << "\n";
    return EXIT_SUCCESS;
}