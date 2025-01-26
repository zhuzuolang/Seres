file(GLOB SRC_FILES "*.cpp")
add_library(Frontend STATIC ${SRC_FILES})
target_include_directories(Frontend PRIVATE ${PROJECT_SOURCE_DIR}/include/)
