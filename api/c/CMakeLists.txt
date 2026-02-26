add_library(openware_api SHARED
    api.c
)

target_include_directories(openware_api
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(openware_api PRIVATE OW_API_EXPORTS)

if(MSVC)
    target_compile_options(openware_api PRIVATE /W4)
else()
    target_compile_options(openware_api PRIVATE -Wall -Wextra -Wpedantic)
endif()

