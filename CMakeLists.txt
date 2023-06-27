cmake_minimum_required(VERSION 3.24)

project(omaVideo)

if (NOT DEFINED PLATFORM)
    message(FATAL_ERROR "Define PLATFORM. Must be one of: linux, efi")
endif()

set(SOURCES
    src/core/core.c
    src/format/reader.c
    src/format/renderer.c
    src/platform/${PLATFORM}.c
)

if ("${PLATFORM}" STREQUAL "efi")
    set(${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/vendor/gnu-efi/lib/data.c)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

if ("${PLATFORM}" STREQUAL "linux")
    message(STATUS "Building for Linux.")
    find_package(X11 REQUIRED)
    target_include_directories(${PROJECT_NAME} PUBLIC ${X11_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
endif()

if ("${PLATFORM}" STREQUAL "efi")
    message(STATUS "Building for EFI. Make sure you have Clang.")
    set(CMAKE_C_COMPILER clang)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -D__EFI__)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/gnu-efi/inc ${CMAKE_CURRENT_SOURCE_DIR}/vendor/gnu-efi/inc/x86_64 ${CMAKE_CURRENT_SOURCE_DIR}/vendor/gnu-efi/inc/protocol)
    
    target_compile_options(${PROJECT_NAME} PUBLIC -target x86_64-unknown-windows -ffreestanding -fshort-wchar -mno-red-zone -O2)
    target_link_options(${PROJECT_NAME} PUBLIC -target x86_64-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link)
    set(CMAKE_EXECUTABLE_SUFFIX .efi)

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND dd if=/dev/zero of=fat.img bs=1k count=524288
        COMMAND mkfs.vfat fat.img 524288
        COMMAND mmd -i fat.img ::/EFI
        COMMAND mmd -i fat.img ::/EFI/BOOT
        COMMAND mcopy -i fat.img ${PROJECT_NAME}.efi ::/EFI/BOOT/BOOTX64.EFI
        COMMAND mcopy -i fat.img ${PROJECT_NAME}.pdb ::/EFI/BOOT/BOOTX64.PDB
        COMMAND mcopy -i fat.img ${CMAKE_CURRENT_SOURCE_DIR}/test.bin ::/VIDEO.BIN
        BYPRODUCTS "fat.img"
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test.bin
        COMMENT "Making fat.img")
endif()
