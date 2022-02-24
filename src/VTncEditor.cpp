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
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar;



void VTncEditor::drawEvent() {
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
    _imgui.newFrame();
    if(ImGui::GetIO().WantTextInput && !isTextInputActive()) startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive()) stopTextInput();
    
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiID id = ImGui::GetID("Main");
    if(!this->firstframerendered){
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
        this->firstframerendered = true;
    }
    ImGui::DockBuilderGetNode(id)->SetLocalFlags(ImGuiDockNodeFlags_NoResize);
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
        if (FileDialog.LoadedFile.isFile)
        {
            for (size_t y = 0; y < FileDialog.LoadedFile.layersResolution[this->currentLayer].y; y++){
                for (size_t x = 0; x < FileDialog.LoadedFile.layersResolution[this->currentLayer].x; x++)
                {
                    if(x > 0) ImGui::SameLine();
                    ImGui::PushID(x + (y * 1000));
                    ImGui::Button(" ", ImVec2(20.0f,20.0f));
                    ImGui::PopID();
                }
                ImGui::NewLine();
            }
        }
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0/Magnum::Double(ImGui::GetIO().Framerate), Magnum::Double(ImGui::GetIO().Framerate));
    ImGui::End();

ImGui::Begin("Frames", nullptr, flags1);
    for (size_t i = 0; i < FileDialog.LoadedFile.layersQuantity; i++)
    {
        ImGui::SameLine();
        ImGui::PushID(i);
        if(ImGui::Button(("Layer: " + std::to_string(i)).c_str(), ImVec2(80.0f,20.0f))) currentLayer = i;
        ImGui::PopID();
    }
ImGui::End();
    

    ImGui::Begin("Color picker", nullptr, flags1);
        ImGui::ColorPicker4("a", &_floatValue);
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