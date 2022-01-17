#include "VTncEditorFileDialog.hpp"
#include "fstream"
#include "ios"
#include "sys/stat.h"

void VTncEditorFileDialog::VTncEditorOpen(char** file) 
{
    #ifdef CORRADE_TARGET_EMSCRIPTEN      
        EM_ASM(
        
        var file_selector = document.createElement('input');
        file_selector.setAttribute('type', 'file');
        file_selector.setAttribute('onchange','open_file(event)');
        file_selector.setAttribute('accept','.vtnc');
        file_selector.click();
        );
    #else
        NFD_Init();
        nfdchar_t *outPath;
        nfdfilteritem_t filterItem[2] = {{"Visual Template Nicely Compressed", "vtnc" }};
        /*nfdresult_t result = */NFD_OpenDialog(&outPath, filterItem, 1, NULL);
        std::fstream filefstream(outPath, std::ios::in | std::ios::binary);
        if(!filefstream) std::cout << "[ERROR] File could not be loaded!";
        else{
            struct stat res;
            stat(outPath, &res);
            std::cout << res.st_size;
            std::cout << *file;
            char buffer[res.st_size];
            filefstream.read(buffer, res.st_size);
            filefstream.close();
            *file = buffer;
        }
    #endif
}

VTncEditorFileDialog::~VTncEditorFileDialog() 
{
    
}

VTncEditorFileDialog::VTncEditorFileDialog() 
{
    
}
