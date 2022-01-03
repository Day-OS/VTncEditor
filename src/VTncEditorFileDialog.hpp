#include "iostream"

#ifndef CORRADE_TARGET_EMSCRIPTEN
    #include "nfd.hpp"
#endif

class VTncEditorFileDialog
{
private:
    /* data */
public:
    VTncEditorFileDialog(/* args */);
    ~VTncEditorFileDialog();
};

VTncEditorFileDialog::VTncEditorFileDialog(/* args */)
{
    #ifndef CORRADE_TARGET_EMSCRIPTEN
    NFD_Init();
    nfdchar_t *outPath = "?";
    nfdfilteritem_t filterItem[2] = { { "Source code", "c,cpp,cc" }, { "Headers", "h,hpp" } };
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);
    #endif

    std::cout << "LOADEEEEEEEEEEEEEEEEEEEDD!!!";
}

VTncEditorFileDialog::~VTncEditorFileDialog()
{
}
