# Identify source files
set(SRC_FILES main.cpp)

# Set up exec
set(TARGET_NAME helmsolve_tests)
add_executable(${TARGET_NAME} ${SRC_FILES})

# Set compile flags, link options for this build type
target_compile_options(${TARGET_NAME} PRIVATE ${BUILD_TYPE_COMPILE_FLAGS})
target_link_options(${TARGET_NAME} PRIVATE ${BUILD_TYPE_LINK_FLAGS})

# Link exec to Nektar
target_link_libraries(${TARGET_NAME} PUBLIC Nektar++::nektar++)

# Set install location
install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
