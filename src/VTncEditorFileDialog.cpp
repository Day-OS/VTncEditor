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
            char buffer[res.st_size];
            filefstream.read(buffer, res.st_size);
            std::vector<unsigned char> filevector(buffer, buffer + res.st_size);
            *file = outPath;
            VTNCRW VTNCCLASS;
            this->LoadedFile = VTNCCLASS.read(filevector);
            std::fstream ff("C:/Users/danie/Documents/GitHub/VTncEditor/modules/VTNCRW-LIB/filestructure/cellbit.vtnc", std::ios::out | std::ios::binary);
            ff.write(reinterpret_cast<const char*>(VTNCCLASS.write(this->LoadedFile).data()), 4);
            ff.close();
        }
    #endif
}

VTncEditorFileDialog::~VTncEditorFileDialog() 
{
    
}

VTncEditorFileDialog::VTncEditorFileDialog() 
{
    
}

void VTncEditorFileDialog::WASMCallback_load_file(uint8_t *buffer, size_t size, char** currentfilepath) 
{
    //I'M DUPLICATED IN VTncEditorFileDialog.cpp just for now, remember to delete me later!!!
    VTNCRW VTNCCLASS;
    std::vector<unsigned char> filevector(buffer, buffer + size);
    this->LoadedFile = VTNCCLASS.read(filevector);
    std::cout << "Buffer: " << &buffer << " | Size: " << size << std::endl;
    std::cout << "IS VTNC???: " << std::hex << ('0' + int(this->LoadedFile.framesQuantity)) << std::endl;
    *currentfilepath = "File loaded (THIS IS NOT AN ERROR: Couldn't put filename because the dev is stupid)";
    std::cout << "KKKKKKKKKK :)";
}
