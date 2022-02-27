#include "VTncEditorClass.h"
#include "VTncEditorFileDialog.hpp"

VTncEditorFileDialog FileDialog;

VTncEditor::VTncEditor(const Arguments& arguments): Magnum::Platform::Application{arguments,
    Configuration{}.setTitle("VTncEditor").setWindowFlags(Configuration::WindowFlag::Resizable)}
{
    _imgui = Magnum::ImGuiIntegration::Context(Magnum::Vector2{windowSize()}/dpiScaling(),windowSize(), framebufferSize());
    Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
    Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID) 
        setMinimalLoopPeriod(16); 
    #endif
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
    
}
char* currentfilepath = nullptr;

ImGuiWindowFlags flagsmenu = ImGuiWindowFlags_NoNavFocus |                                                      // so turn off everything that would make it act like a window
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus | 
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

ImGuiWindowFlags flags1 = ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus | 
			ImGuiWindowFlags_NoMove;



void VTncEditor::drawEvent() {
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
    _imgui.newFrame();
    if(ImGui::GetIO().WantTextInput && !isTextInputActive()) startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive()) stopTextInput();
    
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID id = ImGui::GetID("Main");
    static bool firsttime = true;
    if(firsttime){
        ImGui::DockBuilderRemoveNode(id);
        ImGui::DockBuilderAddNode(id);
        ImGui::DockBuilderSetNodeSize(id, viewport->Size);
        ImGuiID dock1 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.7f, nullptr, &id);
        ImGuiID dock2 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.3f, nullptr, &id);
        ImGuiID dock3 = ImGui::DockBuilderSplitNode(dock1, ImGuiDir_Down, 0.3f, nullptr, &dock1);
        ImGui::DockBuilderDockWindow("Preview", dock1);
        ImGui::DockBuilderDockWindow("Color picker", dock2);
        ImGui::DockBuilderDockWindow("Frames", dock3);
        ImGui::DockBuilderFinish(id);
        firsttime = false;
    }
    ImGui::DockBuilderGetNode(id)->SetLocalFlags(ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::DockBuilderSetNodeSize(id, viewport->WorkSize);
    ImGui::DockBuilderSetNodePos(id,viewport->WorkPos);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));    

    
    ImGui::Begin("Menu",nullptr, flagsmenu);
        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("File")){
                if(ImGui::MenuItem("Load")){
                    FileDialog.VTncEditorOpen(&currentfilepath, VTncEditorFileDialog::Mode::Load);
                }
                if(ImGui::MenuItem("Save As")){
                    FileDialog.VTncEditorOpen(&currentfilepath, VTncEditorFileDialog::Mode::Save);
                }
            ImGui::EndMenu();
            }
        ImGui::EndMainMenuBar();
        }       
    ImGui::End();
    
    ImGui::PopStyleVar();
    
    ImGui::Begin("Preview", nullptr, flags1);
        ImGuiIO& io = ImGui::GetIO();

        if (FileDialog.LoadedFile.isFile)
        {
            for (size_t y = 0; y < FileDialog.LoadedFile.layersResolution[this->currentLayer].y; y++){
                for (size_t x = 0; x < FileDialog.LoadedFile.layersResolution[this->currentLayer].x; x++)
                {
                    u8c currentColorIndex =  FileDialog.LoadedFile.Layers[this->currentLayer].framesArray[currentFrame].Pixels[x + (y*FileDialog.LoadedFile.layersResolution[this->currentLayer].x)];
                    RGBA color = FileDialog.LoadedFile.Colors[currentColorIndex];
                    if(x > 0) ImGui::SameLine();
                    float pixelButtonSize = 40.0f;
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, pixelButtonSize/2.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
                    ImGui::PushStyleColor(ImGuiCol_Button,(ImVec4)ImColor(int(color.R), int(color.G), int(color.B), int(color.A)));
                    ImGui::PushStyleColor(ImGuiCol_Border,(ImVec4)ImColor(0xFF,0xFF,0xFF,0xFF));
                    ImGui::PushID(x + (y * FileDialog.LoadedFile.layersResolution[this->currentLayer].y));
                    if (ImGui::Button(color.A ? " " :"A", ImVec2(pixelButtonSize,pixelButtonSize)))
                    {
                       FileDialog.LoadedFile.Layers[this->currentLayer].framesArray[currentFrame].Pixels[x + (y*FileDialog.LoadedFile.layersResolution[this->currentLayer].x)]  = selectedColor_I;
                    }
                    
                    ImGui::PopStyleColor(2);
                    ImGui::PopStyleVar(2);
                    ImGui::PopID();
                }
            }
        }
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0/Magnum::Double(ImGui::GetIO().Framerate), Magnum::Double(ImGui::GetIO().Framerate));
    ImGui::End();

    ImGui::Begin("Frames", nullptr, flags1);
        static Magnum::Double lastframe = 0;
        ImGui::SliderInt("Frame", &currentFrame, 0, FileDialog.LoadedFile.framesQuantity - 1);
        ImGui::SameLine();
        int _framesquantity = FileDialog.LoadedFile.framesQuantity;
        ImGui::InputInt("Frames quantity", &_framesquantity);
        FileDialog.LoadedFile.framesQuantity = char(_framesquantity);
        ImGui::Checkbox("Play", &isPlaying);
        lastframe += 1000.0/Magnum::Double(ImGui::GetIO().Framerate);
        if (isPlaying && lastframe > (FileDialog.LoadedFile.Layers[currentLayer].framesArray[currentFrame].msDuration))
        {
            lastframe = 0;
            currentFrame > (FileDialog.LoadedFile.framesQuantity -1)? currentFrame = 0 : currentFrame++;
        }
        
        if (ImGui::Button("+", ImVec2(40.0f,40.0f)))
            {
                FileDialog.LoadedFile.colorsQuantity++;
                RGBA newcolor;
                newcolor.R = 0xFF; newcolor.G = 0xFF; newcolor.B = 0xFF; newcolor.A = 0xFF; 
                FileDialog.LoadedFile.Colors[FileDialog.LoadedFile.colorsQuantity] = newcolor;
            }

        for (size_t i = 0; i < FileDialog.LoadedFile.layersQuantity; i++)
        {
            ImGui::SameLine();
            ImGui::PushID(i);
            if(ImGui::Button(("Layer: " + std::to_string(i)).c_str(), ImVec2(80.0f,20.0f))) currentLayer = i;
            ImGui::PopID();
        }
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse pos: <INVALID>");
        ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);

        int count = IM_ARRAYSIZE(io.MouseDown);
        ImGui::Text("Mouse down:");         for (int i = 0; i < count; i++) if (ImGui::IsMouseDown(i))      { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
        ImGui::Text("Mouse clicked:");      for (int i = 0; i < count; i++) if (ImGui::IsMouseClicked(i))   { ImGui::SameLine(); ImGui::Text("b%d (%d)", i, ImGui::GetMouseClickedCount(i)); }
        ImGui::Text("Mouse released:");     for (int i = 0; i < count; i++) if (ImGui::IsMouseReleased(i))  { ImGui::SameLine(); ImGui::Text("b%d", i); }
        ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);
        ImGui::Text("Pen Pressure: %.1f", io.PenPressure); // Note: currently unused

    ImGui::End();

    ImGui::Begin("Color picker", nullptr, flags1);
        static float pixelButtonSize = 40.0f;
        std::string text = "Selected color: ";
        ImGui::ColorPicker4((text + std::to_string(selectedColor_I)).c_str(), (float *) &selectedColor, ImGuiColorEditFlags_AlphaBar);
        if (selectedColor_I != 255)
        {   
            FileDialog.LoadedFile.Colors[selectedColor_I].R = int(selectedColor.x * 255);
            FileDialog.LoadedFile.Colors[selectedColor_I].G = int(selectedColor.y * 255); 
            FileDialog.LoadedFile.Colors[selectedColor_I].B = int(selectedColor.z * 255);
            FileDialog.LoadedFile.Colors[selectedColor_I].A = int(selectedColor.w * 255);    
        }
        for (size_t i = 0; i < FileDialog.LoadedFile.colorsQuantity; i++)
        {
            RGBA currentColor = FileDialog.LoadedFile.Colors[i];
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, pixelButtonSize/2.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
            ImGui::PushStyleColor(ImGuiCol_Button,(ImVec4)ImColor(int(currentColor.R), int(currentColor.G), int(currentColor.B), int(currentColor.A)));
            ImGui::PushStyleColor(ImGuiCol_Border,(ImVec4)ImColor(i == selectedColor_I? Color_Selected : Color_Unselected,i == selectedColor_I? Color_Selected : Color_Unselected,i == selectedColor_I? Color_Selected : Color_Unselected, 255));
            ImGui::PushID(i);
            if (ImGui::Button(std::to_string(i).c_str(), ImVec2(pixelButtonSize,pixelButtonSize)))
            {
                selectedColor_I = i;
                selectedColor = ImColor(FileDialog.LoadedFile.Colors[selectedColor_I].R,FileDialog.LoadedFile.Colors[selectedColor_I].G,FileDialog.LoadedFile.Colors[selectedColor_I].B,FileDialog.LoadedFile.Colors[selectedColor_I].A);
            }
            
            ImGui::SameLine();
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar(2);
            ImGui::PopID();
        }
        ImGui::PushStyleColor(ImGuiCol_Border,(ImVec4)ImColor(Color_Selected,Color_Selected,Color_Selected,Color_Selected));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, pixelButtonSize/2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        if (ImGui::Button("+", ImVec2(pixelButtonSize,pixelButtonSize)))
        {
            FileDialog.LoadedFile.colorsQuantity++;
            RGBA newcolor;
            newcolor.R = 0xFF; newcolor.G = 0xFF; newcolor.B = 0xFF; newcolor.A = 0xFF; 
            FileDialog.LoadedFile.Colors[FileDialog.LoadedFile.colorsQuantity] = newcolor;
        }
        
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    ImGui::End();
    
    if(false && _showDemoWindow) {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow();
    }

    _imgui.updateApplicationCursor(*this);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);

    _imgui.drawFrame();
    swapBuffers();
    redraw();
}

void VTncEditor::viewportEvent(ViewportEvent& event) {Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()}); _imgui.relayout(Magnum::Vector2{event.windowSize()}/event.dpiScaling(), event.windowSize(), event.framebufferSize());}
void VTncEditor::keyPressEvent(KeyEvent& event) {if(_imgui.handleKeyPressEvent(event)) return;}
void VTncEditor::keyReleaseEvent(KeyEvent& event) {if(_imgui.handleKeyReleaseEvent(event)) return;}
void VTncEditor::mousePressEvent(MouseEvent& event) {if(_imgui.handleMousePressEvent(event)) return;}
void VTncEditor::mouseReleaseEvent(MouseEvent& event) {if(_imgui.handleMouseReleaseEvent(event)) return;}
void VTncEditor::mouseMoveEvent(MouseMoveEvent& event) {if(_imgui.handleMouseMoveEvent(event)) return;}
void VTncEditor::mouseScrollEvent(MouseScrollEvent& event) {if(_imgui.handleMouseScrollEvent(event)) {/* Prevent scrolling the page */ event.setAccepted(); return;}}
void VTncEditor::textInputEvent(TextInputEvent& event) {if(_imgui.handleTextInputEvent(event)) return;}

#ifdef CORRADE_TARGET_EMSCRIPTEN
extern "C" EMSCRIPTEN_KEEPALIVE int load_file(uint8_t *buffer, size_t size) {
    FileDialog.WASMCallback_load_file(buffer, size, &currentfilepath);
    return 1;
};
#endif

MAGNUM_APPLICATION_MAIN(VTncEditor);