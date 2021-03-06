#pragma once
#include <Magnum/Math/Color.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include "vtncrw.h"
#include "imgui_internal.h"
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
        VTNCRW VTNCRWLib;
        uint8_t currentLayer = 0;
        int currentFrame = 0;
        int zoom = 100;
        int cachedFramesSize = 1;
        uint8_t selectedColor_I = 255;
        ImVec4 selectedColor;
        bool isPlaying = false;
        bool PreviewWithOutline = true;
        Magnum::Float _floatValue = 0x72909aff;
        Magnum::Color4 _clearColor = 0x72909aff_rgbaf;
        enum Theme{
            Color_Unselected = 0x00,
            Color_Selected = 0xFF
        };
        
};
