#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include "httplib.h"
#include <cstdlib>

namespace fs = std::filesystem;

const std::string SHARED_DIR = "./shared_files";
const std::string STATIC_DIR = "./static";

void start_cloudflare_tunnel() {
    if (!std::filesystem::exists("cloudflared.exe")) {
        std::cout << "Downloading Cloudflare Tunnel...\n";
        std::system("curl -L -o cloudflared.exe https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-windows-amd64.exe");
    }

    std::cout << "Launching Cloudflare tunnel...\n";
    std::system("start cmd /k cloudflared.exe tunnel --url http://localhost:8080");
}

int main() {
    if (!fs::exists(SHARED_DIR)){
        fs::create_directory(SHARED_DIR);
    }
    if (!fs::exists(STATIC_DIR)){
        fs::create_directory(STATIC_DIR);
    }

    httplib::Server svr;

    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        std::ifstream file(STATIC_DIR + "/index.html");
        
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            res.set_content(buffer.str(), "text/html");
        } else {
            res.status = 404;
            res.set_content("Error: Could not find static/index.html. Make sure the folder and file exist.", "text/plain");
        }
    });

    svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res){
        if (req.form.has_file("file_upload")){
            const auto& file = req.form.get_file("file_upload");
            if (!file.filename.empty()){
                fs::path filePath = fs::path(SHARED_DIR) / file.filename;
            
                std::ofstream ofs(filePath, std::ios::binary);
                if (ofs.is_open()){
                    ofs.write(file.content.c_str(), file.content.size());
                    ofs.close();
                } else {
                    std::cerr << "Failed to open file for writing: " << filePath << "\n";
                }
            }
        }
        res.set_redirect("/");
    });

    svr.Get("/list", [](const httplib::Request &req, httplib::Response &res){
        std::ifstream file(STATIC_DIR + "/list.html");
        std::string html;
        
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            html = buffer.str();
        } else {
            res.status = 404;
            res.set_content("Error: Could not find static/list.html", "text/plain");
            return;
        }

        std::string rows;
        bool hasFiles = false;

        for (const auto& entry : fs::directory_iterator(SHARED_DIR)) {
            hasFiles = true;
            std::string filename = entry.path().filename().string();
            rows += "<li class='file-row'><a href='/download/" + filename + "'>" + filename
                + "</a><span class='download-icon'>&#8595;</span></li>";
        }

        if (hasFiles) {
            html += "<ul class='file-list'>" + rows + "</ul>";
        } else {
            html += R"(<div class="empty-state">
            <h2>No files yet</h2>
            <p>Upload a file from your phone to see it here.</p>
            </div>)";
        }

        html += R"(</div>
        <a href="/" class="btn btn-secondary">Back to Home</a>
        </body>
        </html>)";

        res.set_content(html, "text/html"); 
    });

    svr.set_mount_point("/download", SHARED_DIR.c_str());

    std::cout << "======================================\n";
    std::cout << "Starting server on port 8080...\n";
    std::cout << "Look at the new window for your public URL!\n";
    std::cout << "======================================\n";

    start_cloudflare_tunnel();
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}

