#include "ShadowViewer.hh"
#include <iostream>

#ifdef _WIN32
#  include <windows.h>
#  include <stdlib.h>
#  include <errhandlingapi.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    // This make crashes very visible - without them, starting the
    // application from cmd.exe or powershell can surprisingly hide
    // any signs of a an application crash!
    SetErrorMode(0);
#endif
    char const* mesh = "neutral.off";
     if (argc > 2) {
        std::cerr << "Usage: ShadowViewer mesh.off" << std::endl;
        exit(1);
     } else if (argc==2) {
        mesh = argv[1];
     }

    ShadowViewer window("Shadow Viewer", 640, 480);

    std::string mesh_filename = std::string(MESH_PATH) + "/" + mesh;
    window.loadMesh(mesh_filename);
    return window.run();
}
