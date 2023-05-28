# first build vst3sdk and paste root path here
set(vst3sdk_path "/Users/aidan/dev/vst/vst3sdk")
message("USING VST3SDK PATH PROVIDED: ${vst3sdk_path}")

# set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE INTERNAL "" FORCE)

set(ENV{VCPKG_DISABLE_METRICS} TRUE) #disable telemetry

cmake_minimum_required(VERSION 3.15)

set(TARGET_EXE audio_graph)
project(${TARGET_EXE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 CONFIG REQUIRED)
find_package(portaudio CONFIG REQUIRED)
find_package(rtmidi CONFIG REQUIRED)
find_package(unofficial-concurrentqueue CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} include vendor src/modules ${vst3sdk_path})
link_directories(${vst3sdk_path}/build/lib/Debug)

file(GLOB MODULE_SOURCES src/modules/*.cpp)
file(GLOB MODULE_INCLUDES src/modules/*.h)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${MODULE_SOURCES})
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${MODULE_INCLUDES})

file(GLOB SOURCES 
   	src/*.cpp
	vendor/*.cpp
	src/modules/*.cpp
	vendor/EasyVst/src/window.mm
	vendor/EasyVst/src/EasyVst.cpp
	vendor/midifile/src/MidiFile.cpp
	vendor/midifile/src/MidiEventList.cpp
	vendor/midifile/src/MidiEvent.cpp
	vendor/midifile/src/MidiMessage.cpp
	vendor/midifile/src/Options.cpp
	vendor/midifile/src/Binasc.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/connectionproxy.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/eventlist.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/hostclasses.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/module.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/parameterchanges.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/pluginterfacesupport.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/plugprovider.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/processdata.cpp
	${vst3sdk_path}/public.sdk/source/vst/utility/stringconvert.cpp
	${vst3sdk_path}/public.sdk/source/vst/vstinitiids.cpp
	${vst3sdk_path}/public.sdk/source/vst/hosting/module_mac.mm
)
file(GLOB INCLUDES include/*.h vendor/*.h)
include_directories(
    vendor/EasyVst/include/
    vendor/midifile/include/
    vendor/imgui-filebrowser/
    ${vst3sdk_path}
)
add_executable(
    ${TARGET_EXE}
    ${SOURCES}
    ${INCLUDES}
	${MODULE_SOURCES}
    ${MODULE_INCLUDES}
)

target_link_libraries(
	${TARGET_EXE}
    SDL2::SDL2                                          #app libs
    portaudio_static
    RtMidi::rtmidi
    unofficial::concurrentqueue::concurrentqueue
    imgui::imgui
    SndFile::sndfile
    base                                                #vst stuff
    pluginterfaces
    sdk_common
    sdk
)

# for imgui
if (APPLE)
    target_link_libraries(${TARGET_EXE} "-framework OpenGL")
endif()

# for xcode generator 
if(CMAKE_GENERATOR STREQUAL Xcode)
    set(CMAKE_C_FLAGS "-x objective-c")
    set(CMAKE_CXX_FLAGS "-x objective-c++")
    set(CMAKE_EXE_LINKER_FLAGS "-framework Cocoa -framework AppKit -framework CoreData -framework Foundation")
    set_property (TARGET ${TARGET_EXE} APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
else() #for unix makefiles 
    set_property (TARGET ${TARGET_EXE} APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc -ObjC++")
endif()
