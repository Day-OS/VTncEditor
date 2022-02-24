#pragma once
#include "VTncEditorClass.h"
#pragma GCC diagnostic push

#ifdef CORRADE_TARGET_EMSCRIPTEN
    #pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
    #include "emscripten.h"
#else
    #include "nfd.hpp"
#endif

class VTncEditorFileDialog
{
private:
public:
    enum Mode {Load, Save};
    VTNCFile LoadedFile;
    VTncEditorFileDialog();
    void WASMCallback_load_file(uint8_t *buffer, size_t size);
    void VTncEditorOpen(char** file, Mode mode);
    ~VTncEditorFileDialog();
};