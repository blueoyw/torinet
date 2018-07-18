# 요구 CMake 최소 버전
CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 )

# 프로젝트 이름 및 버전
SET( PROJECT_NAME "server" )
PROJECT ( ${PROJECT_NAME} )
SET ( PROJECT_VERSION_MAJOR 0 )
SET ( PROJECT_VERSION_MINOR 1 )

SET ( PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}" )
SET ( PROJECT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )

#print environment
MESSAGE ( "====================================" )
MESSAGE ( "Start ${PROJECT_NAME}" )
MESSAGE ( "TORINET_DIR=${TORINET_DIR}" )
MESSAGE ( "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}" )
MESSAGE ( "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}" )
MESSAGE ( "PROJECT_INCLUDE_DIR=${PROJECT_INCLUDE_DIR}" )
MESSAGE ( "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}" )
MESSAGE ( "PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}" )

# CMake C++11
set( CMAKE_CXX_STANDARD 11 )

#SET ( CMAKE_C_COMPILER "gcc" )
SET ( CMAKE_C_COMPILER "g++" )

# 빌드 대상 바이너리 파일명 및 소스 파일 목록
SET ( OUTPUT_ELF
    "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.out"
    )

#SET ( TORINET_DIR "${PROJECT_SOURCE_DIR}/lib" )
SET ( TORINET_FILES
    "${TORINET_DIR}/TcpSession.cpp"
    "${TORINET_DIR}/IoServicePool.cpp"
    "${TORINET_DIR}/TcpServer.cpp"
    )

SET ( SRC_FILES
    "${PROJECT_SOURCE_DIR}/Character.cpp"
    "${PROJECT_SOURCE_DIR}/Room.cpp"
    "${PROJECT_SOURCE_DIR}/WorldServer.cpp"
    "${PROJECT_SOURCE_DIR}/main.cpp"
    )

# 공통 헤더 파일 Include 디렉토리 (-I)
#INCLUDE_DIRECTORIES ( include driver/include )
FIND_PACKAGE( Boost 1.67 COMPONENTS system filesystem thread REQUIRED )
INCLUDE_DIRECTORIES ( "${PROJECT_BINARY_DIR}" "${PROJECT_INCLUDE_DIR}" $(Boost_INCLUDE_DIRS) )

# 공통 컴파일 옵션, 링크 옵션
ADD_COMPILE_OPTIONS ( -g -Wall -fno-strict-aliasing -std=c++0x -O2 )
#SET ( CMAKE_EXE_LINKER_FLAGS "-static -Wl,--gc-sections" )

# 공통 링크 라이브러리 (-l)
LINK_LIBRARIES( log4cxx boost_system boost_filesystem boost_program_options boost_thread torinet pthread )

# 공통 링크 라이브러리 디렉토리 (-L)
#LINK_DIRECTORIES ( /usr/lib )
LINK_DIRECTORIES ( "${TORINET_DIR}" )

# "Debug" 형상 한정 컴파일 옵션, 링크 옵션
SET ( CMAKE_C_FLAGS_DEBUG "-DDEBUG -DC_FLAGS" )
SET ( CMAKE_EXE_LINKER_FLAGS_DEBUG "-DDEBUG -DLINKER_FLAGS" )

# "Release" 형상 한정 컴파일 옵션, 링크 옵션
SET ( CMAKE_C_FLAGS_RELEASE "-DRELEASE -DC_FLAGS" )
SET ( CMAKE_EXE_LINKER_FLAGS_RELEASE "-DRELEASE -DLINKER_FLAGS" )

# add library to make
ADD_LIBRARY( torinet STATIC ${TORINET_FILES} )
# add application to make
ADD_EXECUTABLE( ${OUTPUT_ELF} ${SRC_FILES} )
# add dependency for application.
TARGET_LINK_LIBRARIES( ${OUTPUT_ELF} PUBLIC torinet )
