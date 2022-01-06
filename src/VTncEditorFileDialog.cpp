#include "VTncEditorFileDialog.hpp"

void VTncEditorFileDialog::VTncEditorOpen() 
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
        const char* filename = "hello.txt";
        const char* text = "Hello world!";
        NFD_Init();
        nfdchar_t *outPath = "?";
        nfdfilteritem_t filterItem[2] = { { "Source code", "c,cpp,cc" }, { "Headers", "h,hpp" } };
        nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);
    #endif
}

VTncEditorFileDialog::~VTncEditorFileDialog() 
{
    
}

VTncEditorFileDialog::VTncEditorFileDialog(/* args */) 
{
    
}
