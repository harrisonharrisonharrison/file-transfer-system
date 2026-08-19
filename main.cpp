#include <iostream>
#include <fstream>
#include <filesystem>
#include "httplib.h"

namespace fs = std::filesystem;

const std::string SHARED_DIR = "./shared_files";

const std::string HTML_CONTENT = R"(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PC File Transfer</title>
    <style>
        body { font-family: -apple-system, sans-serif; padding: 20px; max-width: 600px; margin: auto; }
        .box { border: 1px solid #ccc; padding: 15px; margin-bottom: 20px; border-radius: 8px; }
        button, input { margin-top: 10px; font-size: 16px; }
        button { padding: 8px 16px; background: #007aff; color: white; border: none; border-radius: 6px; }
    </style>
</head>
<body>
    <h2>Phone <-> PC Transfer</h2>

    <div class="box">
        <h3>Upload to PC</h3>
        <form action="/upload" method="post" enctype="multipart/form-data">
            <input type="file" name="file_upload" required>
            <br>
            <button type="submit">Upload File</button>
        </form>
    </div>

    <div class="box">
        <h3>Download from PC</h3>
        <a href="/list" style="color: #007aff; text-decoration: none; font-weight: bold;">View Available Files ➔</a>
    </div>
</body>
</html>
)";

int main() {
    if (!fs::exists(SHARED_DIR)){
        fs::create_directory(SHARED_DIR);
    }

    httplib::Server svr;

    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content(HTML_CONTENT, "text/html");
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

    svr.Get("/list", [](const httplib::Request& req, httplib::Response& res){
        std::string html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'></head><body style='font-family: sans-serif; padding: 20px;'>";
        html += "<h2>Files on PC</h2><ul>";

        for (const auto& entry : fs::directory_iterator(SHARED_DIR)) {
            std::string filename = entry.path().filename().string();
            html += "<li style='margin-bottom: 10px;'><a href='/download/" + filename + "'>" + filename + "</a></li>";
        }

        html += "</ul><br><a href='/'>Back to Home</a></body></html>";
        res.set_content(html, "text/html");
    });

    svr.set_mount_point("/download", SHARED_DIR.c_str());
}

