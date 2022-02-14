#include "VTncEditorClass.h"
#include "VTncEditorFileDialog.hpp"

VTncEditorFileDialog FileDialog;

VTncEditor::VTncEditor(const Arguments& arguments): Magnum::Platform::Application{arguments,
    Configuration{}.setTitle("Magnum ImGui Example").setWindowFlags(Configuration::WindowFlag::Resizable)}
{
    _imgui = Magnum::ImGuiIntegration::Context(Magnum::Vector2{windowSize()}/dpiScaling(),windowSize(), framebufferSize());
    Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
    Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
        setMinimalLoopPeriod(16);
    #endif
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
}
char* currentfilepath = "outPath not defined yet :)";

void VTncEditor::drawEvent() {
    
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
    _imgui.newFrame();
    /* Enable currentfilepath input */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();


    ImGui::Begin(
			"Editor",
			/*p_open=*/nullptr,
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus
		);
        
        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("File")){
                if(ImGui::MenuItem("Load")){
                    FileDialog.VTncEditorOpen(&currentfilepath, VTncEditorFileDialog::Mode::Load);    
                    std::cout << currentfilepath << std::endl;
                }
                if(ImGui::MenuItem("Save As")){
                    FileDialog.VTncEditorOpen(&currentfilepath, VTncEditorFileDialog::Mode::Save);    
                    std::cout << currentfilepath << std::endl;
                }
            ImGui::EndMenu();
            }
        ImGui::EndMainMenuBar();
        }
    ImGui::End();

    {
        
        ImGui::Text("Hello, world!");
        ImGui::ColorPicker4("a", &_floatValue);
        ImGui::SliderFloat("Float", &_floatValue, 0.0f, 1.0f);
        if(ImGui::ColorEdit3("Clear Color", _clearColor.data()))
            Magnum::GL::Renderer::setClearColor(_clearColor);
        if(ImGui::Button("Test Window")) _showDemoWindow ^= true;
        if(ImGui::Button("Another Window")) _showAnotherWindow ^= true;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0/Magnum::Double(ImGui::GetIO().Framerate), Magnum::Double(ImGui::GetIO().Framerate));
    }

    if(_showDemoWindow) {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow();
    }

    /* Update application cursor */
    _imgui.updateApplicationCursor(*this);

    /* Set appropriate states. If you only draw ImGui, it is sufficient to
       just enable blending and scissor test in the constructor. */
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);

    _imgui.drawFrame();

    /* Reset state. Only needed if you want to draw something else with
       different state after. */
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);

    swapBuffers();
    redraw();
}

void VTncEditor::viewportEvent(ViewportEvent& event) {
    Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    _imgui.relayout(Magnum::Vector2{event.windowSize()}/event.dpiScaling(),
        event.windowSize(), event.framebufferSize());
}

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
#else


#endif

MAGNUM_APPLICATION_MAIN(VTncEditor);