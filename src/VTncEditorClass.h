#pragma once
#include <Magnum/Math/Color.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include "vtncrw.h"
#include "iostream"

#ifdef CORRADE_TARGET_ANDROID
#include <Magnum/Platform/AndroidApplication.h>
#elif defined(CORRADE_TARGET_EMSCRIPTEN)
#include <Magnum/Platform/EmscriptenApplication.h>
#else
#include <Magnum/Platform/Sdl2Application.h>
#endif

using namespace Magnum::Math::Literals;

class VTncEditor: public Magnum::Platform::Application {
    public:
        explicit VTncEditor(const Arguments& arguments);

        void drawEvent() override;

        void viewportEvent(ViewportEvent& event) override;

        void keyPressEvent(KeyEvent& event) override;
        void keyReleaseEvent(KeyEvent& event) override;

        void mousePressEvent(MouseEvent& event) override;
        void mouseReleaseEvent(MouseEvent& event) override;
        void mouseMoveEvent(MouseMoveEvent& event) override;
        void mouseScrollEvent(MouseScrollEvent& event) override;
        void textInputEvent(TextInputEvent& event) override;

    private:
        Magnum::ImGuiIntegration::Context _imgui{Magnum::NoCreate};

        bool _showDemoWindow = true;
        bool _showAnotherWindow = false;
        Magnum::Color4 _clearColor = 0x72909aff_rgbaf;
        Magnum::Float _floatValue = 0.0f;
};
