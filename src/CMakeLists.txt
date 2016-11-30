find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${PROJECT_SOURCE_DIR}/include ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
file(GLOB FILES "*.h" "*.cpp")
#message("SDL include dir: " ${SDL2_INCLUDE_DIR})
add_executable(dyn-emu ${FILES})
llvm_map_components_to_libnames(llvm_libs support core irreader target executionengine mc mcjit x86 x86asmparser x86asmprinter x86codegen x86desc x86disassembler x86info x86utils)
target_link_libraries(dyn-emu ${llvm_libs})
set_property(TARGET dyn-emu PROPERTY CXX_STANDARD 11)
set_property(TARGET dyn-emu PROPERTY CXX_STANDARD_REQUIRED ON)

