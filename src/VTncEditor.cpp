#include "VTncEditorClass.h"
#include "VTncEditorFileDialog.hpp"

VTncEditorFileDialog FileDialog;

VTncEditor::VTncEditor(const Arguments& arguments): Magnum::Platform::Application{arguments,
    Configuration{}.setTitle("Magnum ImGui Example").setWindowFlags(Configuration::WindowFlag::Resizable)}
{
    //nfdresult_t result =  NFD::OpenDialog(outPath, filterItem, 2);
    _imgui = Magnum::ImGuiIntegration::Context(Magnum::Vector2{windowSize()}/dpiScaling(),windowSize(), framebufferSize());
    Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
    Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    #if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
        setMinimalLoopPeriod(16);
    #endif
}

void VTncEditor::drawEvent() {
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);
    _imgui.newFrame();
    /* Enable text input */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

        
    ImGui::Begin("DEBUGGERSONS");
    ImGui::Text("test loading folder stuff: ");
    if (ImGui::Button("wassuuuuuuuuuuup man"))
    {
        FileDialog.VTncEditorOpen();
    };
    ImGui::Text("outPath not defined yet :)");
    ImGui::End();

    /* 1. Show a simple window.
       Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appear in
       a window called "Debug" automatically */
    {
        
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("Float", &_floatValue, 0.0f, 1.0f);
        if(ImGui::ColorEdit3("Clear Color", _clearColor.data()))
            Magnum::GL::Renderer::setClearColor(_clearColor);
        if(ImGui::Button("Test Window")) _showDemoWindow ^= true;
        if(ImGui::Button("Another Window")) _showAnotherWindow ^= true;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0/Magnum::Double(ImGui::GetIO().Framerate), Magnum::Double(ImGui::GetIO().Framerate));
    }

    /* 2. Show another simple window, now using an explicit Begin/End pair */
    if(_showAnotherWindow) {
        ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Another Window", &_showAnotherWindow);
        ImGui::Text("Hello");
        ImGui::End();
    }

    /* 3. Show the ImGui demo window. Most of the sample code is in
       ImGui::ShowDemoWindow() */
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
    std::cout << "Buffer: " << &buffer << " | Size: " << size << std::endl;


    return 1;
};

#endif

MAGNUM_APPLICATION_MAIN(VTncEditor);