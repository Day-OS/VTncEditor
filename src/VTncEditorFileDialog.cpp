#include "VTncEditorFileDialog.hpp"
#include "fstream"
#include "ios"
#include "sys/stat.h"

void VTncEditorFileDialog::VTncEditorOpen(char** file, Mode mode) 
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

        //selects which NFD dialog to show (loading or saving)
        std::fstream filefstream;
        if (mode == Mode::Load){ NFD_OpenDialog(&outPath, filterItem, 1, NULL); filefstream = std::fstream(outPath, std::ios::in | std::ios::binary);}
        else{NFD_SaveDialog(&outPath, filterItem, 1, NULL, "VTNCFILE.vtnc");  filefstream = std::fstream(outPath, std::ios::out | std::ios::binary);}
        
        if(!filefstream) std::cout << "(VTNCEditor)[ERROR] File could not be loaded!";
        else{
            VTNCRW VTNCCLASS;
            if (mode == Mode::Load)
            {    
                struct stat res;
                stat(outPath, &res);
                char buffer[res.st_size];
                filefstream.read(buffer, res.st_size);
                std::vector<unsigned char> filevector(buffer, buffer + res.st_size);
                *file = outPath;
                this->LoadedFile = VTNCCLASS.read(filevector);
            }
            else{
                std::vector<unsigned char> buffertowrite = VTNCCLASS.write(this->LoadedFile);
                filefstream.write(reinterpret_cast<const char*>(buffertowrite.data()), buffertowrite.size());
                filefstream.close();    
            }
        }
    #endif
}

VTncEditorFileDialog::~VTncEditorFileDialog() 
{
    
}

VTncEditorFileDialog::VTncEditorFileDialog() 
{
    
}

void VTncEditorFileDialog::WASMCallback_load_file(uint8_t *buffer, size_t size) 
{
    VTNCRW VTNCCLASS;
    std::vector<unsigned char> filevector(buffer, buffer + size);
    this->LoadedFile = VTNCCLASS.read(filevector);
    std::cout << "Buffer: " << &buffer << " | Size: " << size << std::endl;
    std::cout << "IS VTNC???: " << std::hex << ('0' + int(this->LoadedFile.framesQuantity)) << std::endl;
    std::cout << "KKKKKKKKKK :)";
}
