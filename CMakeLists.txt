cmake_minimum_required(VERSION 3.17...3.24)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

project(BEMI
        VERSION 1.0 
        DESCRIPTION "Implementatiion of BEM method for EMI model." 
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_OPENMP "Enables OpenMP" ON)
option(BUILD_SHARED_LIBS "Build shared (ON) or static (OFF) libraries" OFF)

find_package(Eigen3 REQUIRED CONFIG) # Eigen3::Eigen
find_package(Gmsh REQUIRED) # ${GMSH_LIB}, ${GMSH_INC}
find_package(FFTW3 REQUIRED) #${FFTW3_INCLUDE_DIRS}, ${FFTW3_LIBRARY_DIRS}, ${FFTW3_LIBRARIES}

if(ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        link_libraries(OpenMP::OpenMP_CXX)
    endif()
endif()

include_directories(BEFORE include)

add_subdirectory(src)

add_executable(bemi src/main.cpp)
target_link_libraries(bemi PUBLIC MainHeader parameters)

