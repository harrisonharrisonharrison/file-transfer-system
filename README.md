# C++ PC-to-Phone File Transfer
<img width="569" height="461" alt="image" src="https://github.com/user-attachments/assets/f105457a-1342-4a4f-aaf9-e47e101ba044" />
<img width="481" height="499" alt="image" src="https://github.com/user-attachments/assets/8deacf93-84d5-4944-bbe9-3d1d36f37db0" />

C++ web server that lets you easily transfer files between your Windows PC and phone. 

## Features
* Uses native browser (Safari/Chrome).
* Automatically downloads and uses `cloudflared` to bypass strict router settings.
* Generates a scannable QR code directly in command prompt.
* Upload files from Phone → PC, or download files from PC → Phone.

## Project Structure
Make sure your directory looks like this before compiling:
```text
/your-folder
 ├── main.cpp
 ├── httplib.h          # Get this from yhirose/cpp-httplib
 └── /static
      └── index.html    # The frontend web UI
```

## How to Compile and Run
Ensure you have installed CMake, then open your terminal in the project directory and run:
```text
cmake -B build
cmake --build build
./build/Debug/server.exe
```
Uploaded files will be in the `/shared_files` directory
