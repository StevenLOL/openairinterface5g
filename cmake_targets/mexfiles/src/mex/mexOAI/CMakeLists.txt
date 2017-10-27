# compile mex
set(CPP_FILE mexOAI)

# set up matlab libraries
#set(CPP_FILE_NAME ${CPP_FILE}.c)
include_directories(${MATLAB_INCLUDE_DIR})
#INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR} "/home/sebastian/Work/openairinterface5g/openair1/PHY/LTE_TRANSPORT/")
#GET_DIRECTORY_PROPERTY(output INCLUDE_DIRECTORIES)
#MESSAGE(WARNING ${output})

add_library(${CPP_FILE} SHARED
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexOAI.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschDecodingInit.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexSetTausSeed.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexGetTbs.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexTaus.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschEncoding.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschChannelLevelPrec_TM56.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexLog2Approx.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschChannelCompensation_TM56.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschDualStreamCorrelation.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschDetectionMrc.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschIALLR.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschDecoding.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschChannelLevel.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschChannelCompensation.c
    ${CMAKE_SOURCE_DIR}/src/mex/mexOAI/mexDlschSISOLLR.c
    ${CMAKE_SOURCE_DIR}/src/Matlabdef.def)

target_link_libraries(${CPP_FILE} PHY)

#include_directories("${OPENAIR_BIN_DIR}")
#include_directories("${OPENAIR1_DIR}")
#target_link_libraries(${CPP_FILE} ${MATLAB_LIBRARIES})
#target_include_directories(${CPP_FILE} PUBLIC "${OPENAIR1_DIR}")
  
# 32-bit or 64-bit mex
if(WIN32)
  if (CMAKE_CL_64)
      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexw64)
  else(CMAKE_CL_64)
      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexw32)
  endif(CMAKE_CL_64)
else(WIN32)
  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexa64 PREFIX "")
  else(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexglx PREFIX "")
  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
endif(WIN32)



# install to /bin by default
#install(TARGETS ${CPP_FILE} DESTINATION ./bin)
