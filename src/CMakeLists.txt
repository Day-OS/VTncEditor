find_package(Magnum REQUIRED GL)

if(CORRADE_TARGET_EMSCRIPTEN)
    message("Built as WASMKKKKK")
    find_package(Magnum REQUIRED EmscriptenApplication)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s EXPORTED_RUNTIME_METHODS=ccall")
else()
    find_package(Magnum REQUIRED Sdl2Application)
endif()

find_package(MagnumIntegration REQUIRED ImGui)


set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

file(GLOB SOURCES
    VTncEditor.cpp

    VTncEditorFileDialog.hpp
    VTncEditorFileDialog.cpp
)

add_executable(VTncEditor ${SOURCES})


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
    target_link_options(VTncEditor PRIVATE "-s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\"]'")
    target_link_options(VTncEditor PRIVATE "-sEXPORTED_FUNCTIONS=_main,_load_file")
    target_link_options(VTncEditor PRIVATE "-s LLD_REPORT_UNDEFINED")
else()
    message("Built as SDL")
    target_link_libraries(VTncEditor PRIVATE nfd)
endif ()


# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VTncEditor)
