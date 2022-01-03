find_package(Magnum REQUIRED GL)

if(CORRADE_TARGET_EMSCRIPTEN)
    message("Built as WASMKKKKK")
    find_package(Magnum REQUIRED EmscriptenApplication)
else()
    find_package(Magnum REQUIRED Sdl2Application)
endif()

find_package(MagnumIntegration REQUIRED ImGui)


set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

add_executable(VTncEditor VTncEditor.cpp)

target_include_directories(VTncEditor PUBLIC modules/VTNCRW-LIB ) 
target_link_libraries(
    VTncEditor PRIVATE
    Magnum::Application
    Magnum::GL
    Magnum::Magnum 
    MagnumIntegration::ImGui
    VTNCRW
    )

if (CORRADE_TARGET_EMSCRIPTEN)
    message("Built as WASM")   
else()
    target_link_libraries(VTncEditor PRIVATE nfd)
endif ()


# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VTncEditor)
