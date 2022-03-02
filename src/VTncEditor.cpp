#include "VTncEditorClass.h"
#include "VTncEditorFileDialog.hpp"
#define MOUSEDEBUG false
#define DEBUG false

VTncEditorFileDialog FileDialog;
VTNCFile * LoadedFile = &FileDialog.LoadedFile;
//IMPLEMENTING THIS FOR ICONS IN THE FUTURE
//Magnum::Platform::Sdl2Application::setWindowIcon
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

ImGuiWindowFlags flagsmenu = ImGuiWindowFlags_NoDocking |
                             ImGuiWindowFlags_MenuBar |
                             ImGuiWindowFlags_NoNavFocus |                                                      // so turn off everything that would make it act like a window
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoBringToFrontOnFocus | 
                             ImGuiWindowFlags_NoBackground |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

ImGuiWindowFlags flags1 = ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus | 
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar;



void VTncEditor::drawEvent() {
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
    _imgui.newFrame();
    if(ImGui::GetIO().WantTextInput && !isTextInputActive()) startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive()) stopTextInput();
    
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID id = ImGui::GetID("Main");
    static bool firsttime = true;
    
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); 
    ImGui::Begin("Menu",nullptr, flagsmenu);
    ImGui::PopStyleVar();
    ImGui::DockSpace(id, ImVec2(0.0f,0.0f));
    if(firsttime){
        firsttime = false;
        if(DEBUG){
            Resolution EachLayerResolution[2];
            EachLayerResolution[0].x = 2;
            EachLayerResolution[0].y = 3;
            EachLayerResolution[1].x = 4;
            EachLayerResolution[1].y = 5;
            FileDialog.LoadedFile = VTNCRWLib.create(2, EachLayerResolution, 8, 7);
        }
        ImGui::DockBuilderRemoveNode(id);
        ImGui::DockBuilderAddNode(id, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(id, viewport->Size);
        ImGuiID dock2 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.3f, nullptr, &id);
        ImGuiID dock3 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Down, 0.3f, nullptr, &id);
        ImGui::DockBuilderDockWindow("Preview", id);
        ImGui::DockBuilderDockWindow("Color picker", dock2);
        ImGui::DockBuilderDockWindow("Frames", dock3);
        ImGui::DockBuilderFinish(id);
        
    }
    



    
        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("File")){
                
                if(ImGui::MenuItem("Load")){
                    FileDialog.VTncEditorOpen(&currentfilepath, VTncEditorFileDialog::Mode::Load);
                    cachedFramesSize = LoadedFile->framesQuantity - 1;
                }
                if(LoadedFile->isFile) 
                    {if(ImGui::MenuItem("Save As")){
                        FileDialog.VTncEditorOpen(&currentfilepath, VTncEditorFileDialog::Mode::Save);
                    }
                    if(ImGui::MenuItem("Close")){
                        //It doesn't actually closes itself, but resets the preview to the new file menu
                        LoadedFile->isFile = false;
                        cachedFramesSize = 1;
                    }
                }
            ImGui::EndMenu();}
            ImGui::Spacing();
            ImGui::SliderInt("Zoom",&zoom,1,200,"%d%%");
            ImGui::Spacing();
            ImGui::Checkbox("Enable outline", &PreviewWithOutline);
            ImGui::Spacing();
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0/Magnum::Double(ImGui::GetIO().Framerate), Magnum::Double(ImGui::GetIO().Framerate));
        ImGui::EndMainMenuBar();
        }       
    ImGui::End();
    
    ImGui::Begin("Preview", nullptr, flags1);
        ImGuiIO& io = ImGui::GetIO();
        if (LoadedFile->isFile)
        {
            for (size_t y = 0; y < LoadedFile->layersResolution[currentLayer].y; y++){
                for (size_t x = 0; x < LoadedFile->layersResolution[currentLayer].x; x++)
                {
                    if(currentFrame > cachedFramesSize){ 
                        Layer* _layer = &LoadedFile->Layers[currentLayer];
                        _layer->framesArray[currentFrame] = _layer->framesArray[cachedFramesSize];        
                        cachedFramesSize = currentFrame;
                    }
                    u8c currentColorIndex =  LoadedFile->Layers[currentLayer].framesArray[currentFrame].Pixels[x + (y*LoadedFile->layersResolution[currentLayer].x)];
                    RGBA color = LoadedFile->Colors[currentColorIndex];
                    if(x > 0) ImGui::SameLine();
                    float pixelButtonSize = 0.0004f * (ImGui::GetWindowHeight() + ImGui::GetWindowWidth()/2) * zoom;
                    ImVec4 ButtonColor = (ImVec4)ImColor(int(color.R), int(color.G), int(color.B), int(color.A));
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, pixelButtonSize/2.0f);
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
                    ImGui::PushStyleColor(ImGuiCol_Button, ButtonColor);
                    ImGui::PushStyleColor(ImGuiCol_Border,(ImVec4)ImColor(0xFF,0xFF,0xFF, PreviewWithOutline? 0xFF :0));
                    ImGui::PushID(x + (y * LoadedFile->layersResolution[currentLayer].x));
                    ButtonColor.w = ButtonColor.w/2;
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ButtonColor);
                    bool ButtonClicked = ImGui::Button(color.A ? " " :"A", ImVec2(pixelButtonSize,pixelButtonSize));
                    if (ButtonClicked || ((ImGui::IsItemHovered()|| ImGui::IsItemClicked()) && ImGui::IsMouseDown(ImGuiMouseButton_Left)))
                    {   
                        LoadedFile->Layers[currentLayer].framesArray[currentFrame].Pixels[x + (y*LoadedFile->layersResolution[currentLayer].x)]  = selectedColor_I;
                    }
                    
                    ImGui::PopStyleColor(3);
                    ImGui::PopStyleVar(2);
                    ImGui::PopID();
                }
            }
        }
        else{
            static u8c layerQuantity = 1;
            int int_layerQuantity = layerQuantity;
            ImGui::InputInt("Layer quantity", &int_layerQuantity);
            layerQuantity = int_layerQuantity ? int_layerQuantity : 1;
            
            static u8c colorsQuantity = 1;
            int int_colorsQuantity = colorsQuantity;
            ImGui::InputInt("Colors quantity", &int_colorsQuantity);
            colorsQuantity = int_colorsQuantity ? int_colorsQuantity : 1;
            
            static u8c EndsAnimationAt = 1;
            int int_EndsAnimationAt = EndsAnimationAt;
            ImGui::InputInt("End Frame", &int_EndsAnimationAt);
            EndsAnimationAt = int_EndsAnimationAt ? int_EndsAnimationAt : 1;
            
            Resolution EachLayerResolution[int_layerQuantity];
            for (size_t i = 0; i < int_layerQuantity; i++)
            {
                ImGui::PushID(i);
                    ImGui::Text(("Layer " + std::to_string(i) + " Resolution:").c_str());
                    static int xy[2] = {1,1};
                    ImGui::Text("Weight"); ImGui::SameLine();
                    ImGui::InputInt2("", xy, ImGuiInputTextFlags_None); ImGui::SameLine();
                    EachLayerResolution[i].x = xy[0];
                    EachLayerResolution[i].y = xy[1];
                ImGui::PopID();
            }
            if (ImGui::Button("Create!"))
            {
                Resolution newfile [1];
                newfile[0].x = 4;
                newfile[0].y = 5;
                FileDialog.LoadedFile = VTNCRWLib.create(layerQuantity, EachLayerResolution, colorsQuantity, EndsAnimationAt);
            }
            
        }
    ImGui::End();
    if(LoadedFile->isFile)
    {
        ImGui::Begin("Frames", nullptr, flags1);
            static Magnum::Double lastframe = 0;    
            ImGui::SliderInt("Timeline", &currentFrame, 0, LoadedFile->framesQuantity - 1);
            int _framesquantity = LoadedFile->framesQuantity;
            ImGui::InputInt("Ends at", &_framesquantity);
            LoadedFile->framesQuantity = char(_framesquantity);
            int _msDuration = LoadedFile->Layers[currentLayer].framesArray[currentFrame].msDuration;
            ImGui::InputInt("Transition time", &_msDuration);
            LoadedFile->Layers[currentLayer].framesArray[currentFrame].msDuration = u16c(_msDuration);
            ImGui::Checkbox("Play", &isPlaying);
            for (size_t i = 0; i < LoadedFile->layersQuantity; i++)
            {
                ImGui::SameLine();
                ImGui::PushID(i);
                if(ImGui::Button(("Layer: " + std::to_string(i)).c_str(), ImVec2(80.0f,20.0f))) currentLayer = i;
                ImGui::PopID();
            }

            lastframe += 1000.0/Magnum::Double(ImGui::GetIO().Framerate);
            if (isPlaying && lastframe > (LoadedFile->Layers[currentLayer].framesArray[currentFrame].msDuration))
            {
                lastframe = 0;
                currentFrame > (LoadedFile->framesQuantity -1)? currentFrame = 0 : currentFrame++;
            }

            
            if (MOUSEDEBUG){            
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

            }
        ImGui::End();


        ImGui::Begin("Color picker", nullptr, flags1);
            static float pixelButtonSize = 40.0f;
            std::string text = "Selected color: ";
            ImGui::ColorPicker4((text + std::to_string(selectedColor_I)).c_str(), (float *) &selectedColor, ImGuiColorEditFlags_AlphaBar);
            if (selectedColor_I != 255)
            {   
                LoadedFile->Colors[selectedColor_I].R = int(selectedColor.x * 255);
                LoadedFile->Colors[selectedColor_I].G = int(selectedColor.y * 255); 
                LoadedFile->Colors[selectedColor_I].B = int(selectedColor.z * 255);
                LoadedFile->Colors[selectedColor_I].A = int(selectedColor.w * 255);    
            }
            for (size_t i = 0; i < LoadedFile->colorsQuantity; i++)
            {
                RGBA currentColor = LoadedFile->Colors[i];
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, pixelButtonSize/2.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
                ImGui::PushStyleColor(ImGuiCol_Button,(ImVec4)ImColor(int(currentColor.R), int(currentColor.G), int(currentColor.B), int(currentColor.A)));
                ImGui::PushStyleColor(ImGuiCol_Border,(ImVec4)ImColor(i == selectedColor_I? Color_Selected : Color_Unselected,i == selectedColor_I? Color_Selected : Color_Unselected,i == selectedColor_I? Color_Selected : Color_Unselected, 255));
                ImGui::PushID(i);
                if (ImGui::Button(std::to_string(i).c_str(), ImVec2(pixelButtonSize,pixelButtonSize)))
                {
                    selectedColor_I = i;
                    selectedColor = ImColor(LoadedFile->Colors[selectedColor_I].R,LoadedFile->Colors[selectedColor_I].G,LoadedFile->Colors[selectedColor_I].B,LoadedFile->Colors[selectedColor_I].A);
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
                LoadedFile->colorsQuantity++;
                RGBA newcolor;
                newcolor.R = 0xFF; newcolor.G = 0xFF; newcolor.B = 0xFF; newcolor.A = 0xFF; 
                LoadedFile->Colors[LoadedFile->colorsQuantity] = newcolor;
            }
            
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
        ImGui::End();
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