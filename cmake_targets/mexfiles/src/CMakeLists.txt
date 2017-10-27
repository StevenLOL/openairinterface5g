cmake_minimum_required(VERSION 2.8)
#project(MEX)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/src/cmake) # add FindMatlab module

add_definitions(/DMATLAB_MEX_FILE) #define matlab macros
add_definitions(/DMX_COMPAT_32)

find_package(Matlab REQUIRED)

IF(MATLAB_FOUND)
    message(STATUS "MATLAB Found, MATLAB MEX will be compiled.")
    #add_subdirectory(mex)
ELSE(MATLAB_FOUND)
    MESSAGE("MATLAB not found...nothing will be built.")
ENDIF(MATLAB_FOUND)

# compile mex
#set(CPP_FILE mexGetTbs)
#set(CPP_FILE_NAME ${CMAKE_SOURCE_DIR}/src/mex/mexGetTbs/${CPP_FILE}.c)

# set up matlab libraries
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})
#add_library(mexGetTbs SHARED ${CMAKE_SOURCE_DIR}/src/mex/mexGetTbs/mexGetTbs.c ${CMAKE_SOURCE_DIR}/src/Matlabdef.def)
#target_link_libraries(mexGetTbs PHY ${MATLAB_LIBRARIES})

set(list mexOAI mexAdd mexGetTbs mexTaus mexSetTausSeed mexDlschDecodingInit mexDlschEncoding mexDlschChannelLevelPrec_TM56 mexLog2Approx mexDlschChannelCompensation_TM56 mexDlschDualStreamCorrelation mexDlschDetectionMrc mexDlschIALLR mexDlschDecoding)

#foreach (file ${list})
#add_library(${file} SHARED ${CMAKE_SOURCE_DIR}/src/mex/${file}/${file}.c ${CMAKE_SOURCE_DIR}/src/Matlabdef.def)
#target_link_libraries(${file} PHY ${MATLAB_LIBRARIES})
#
## 32-bit or 64-bit mex
#if(WIN32)
#  if (CMAKE_CL_64)
#      SET_TARGET_PROPERTIES(${file} PROPERTIES SUFFIX .mexw64)
#  else(CMAKE_CL_64)
#      SET_TARGET_PROPERTIES(${file} PROPERTIES SUFFIX .mexw32)
#  endif(CMAKE_CL_64)
#else(WIN32)
#  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
#      SET_TARGET_PROPERTIES(${file} PROPERTIES SUFFIX .mexa64 PREFIX "")
#  else(CMAKE_SIZEOF_VOID_P MATCHES "8")
#      SET_TARGET_PROPERTIES(${file} PROPERTIES SUFFIX .mexglx PREFIX "")
#  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
#endif(WIN32)
#
#endforeach()

include(${CMAKE_SOURCE_DIR}/src/mex/mexOAI/CMakeLists.txt)

# 32-bit or 64-bit mex
#if(WIN32)
#  if (CMAKE_CL_64)
 #     SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexw64)
 # else(CMAKE_CL_64)
 #     SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexw32)
 # endif(CMAKE_CL_64)
#else(WIN32)
#  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
#      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexa64 PREFIX "")
#  else(CMAKE_SIZEOF_VOID_P MATCHES "8")
#      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexglx PREFIX "")
#  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
#endif(WIN32)

# install to /bin by default
#install(TARGETS ${CPP_FILE} DESTINATION ../bin)
