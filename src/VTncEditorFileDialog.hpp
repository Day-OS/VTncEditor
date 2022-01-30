#pragma once
#include "VTncEditorClass.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"

#ifdef CORRADE_TARGET_EMSCRIPTEN
    #include "emscripten.h"
#else
    #include "nfd.hpp"
#endif

class VTncEditorFileDialog
{
private:
    /* data */
public:
    VTNCFile LoadedFile;
    VTncEditorFileDialog(/* args */);
    void WASMCallback_load_file(uint8_t *buffer, size_t size, char** currentfilepath);
    void VTncEditorOpen(char** file);
    ~VTncEditorFileDialog();
};