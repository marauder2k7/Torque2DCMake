project(${TORQUE_APP_NAME})

if(UNIX)
    if(NOT CXX_FLAG32)
        set(CXX_FLAG32 "")
    endif()
    #set(CXX_FLAG32 "-m32") #uncomment for build x32 on OSx64

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAG32} -Wundef -msse -pipe -Wfatal-errors -Wno-return-type-c-linkage -Wno-unused-local-typedef ${TORQUE_ADDITIONAL_LINKER_FLAGS}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CXX_FLAG32} -Wundef -msse -pipe -Wfatal-errors -Wno-return-type-c-linkage -Wno-unused-local-typedef ${TORQUE_ADDITIONAL_LINKER_FLAGS}")
    else()
    # default compiler flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAG32} -Wundef -msse -pipe -Wfatal-errors -no-pie ${TORQUE_ADDITIONAL_LINKER_FLAGS} -Wl,-rpath,'$$ORIGIN'")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CXX_FLAG32} -Wundef -msse -pipe -Wfatal-errors ${TORQUE_ADDITIONAL_LINKER_FLAGS} -Wl,-rpath,'$$ORIGIN'")

   endif()    

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# TODO: fmod support

###############################################################################
# modules
###############################################################################

option(TORQUE_BOX2D "Box2D Physics" ON)

mark_as_advanced(TORQUE_BOX2D)

option(TORQUE_SFX_VORBIS "Vorbis Sound" ON)
mark_as_advanced(TORQUE_SFX_VORBIS)
option(TORQUE_SFX_OPENAL "OpenAL Sound" ON)
#windows uses openal-soft
if(WIN32)
	
	add_subdirectory( ${libDir}/Box2D ${CMAKE_CURRENT_BINARY_DIR}/Box2D)
endif()

mark_as_advanced(TORQUE_SFX_OPENAL)

###############################################################################
# options
###############################################################################
if(NOT MSVC AND NOT APPLE) # handle single-configuration generator
    set(TORQUE_BUILD_TYPE "Debug" CACHE STRING "Select one of Debug, Release and RelWithDebInfo")
    set_property(CACHE TORQUE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")

    set(TORQUE_ADDITIONAL_LINKER_FLAGS "" CACHE STRING "Additional linker flags")
    mark_as_advanced(TORQUE_ADDITIONAL_LINKER_FLAGS)
endif()

if(WIN32)
    # warning C4800: 'XXX' : forcing value to bool 'true' or 'false' (performance warning)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4800")
    # warning C4018: '<' : signed/unsigned mismatch
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4018")
    # warning C4244: 'initializing' : conversion from 'XXX' to 'XXX', possible loss of data
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4244")
	
endif()

# build types
if(NOT MSVC AND NOT APPLE) # handle single-configuration generator
	set(CMAKE_BUILD_TYPE ${TORQUE_BUILD_TYPE})
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(TORQUE_DEBUG TRUE)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(TORQUE_RELEASE TRUE)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(TORQUE_RELEASE TRUE)
    else()
		message(FATAL_ERROR "Please select Debug, Release or RelWithDebInfo for TORQUE_BUILD_TYPE")
	endif()
endif()

###############################################################################
# Always enabled paths first
###############################################################################
addPath("${srcDir}/") # must come first :)
addPathRec("${srcDir}/algorithm")
addPath("${srcDir}/assets")
addPath("${srcDir}/audio")
addPath("${srcDir}/bitmapFont")
addPath("${srcDir}/collection")
addPath("${srcDir}/component")
addPath("${srcDir}/component/behaviors")
addPath("${srcDir}/console")
addPath("${srcDir}/debug")
addPath("${srcDir}/debug/remote")
addPath("${srcDir}/delegates")
addPath("${srcDir}/game")
addPath("${srcDir}/graphics")
addPath("${srcDir}/gui")
addPath("${srcDir}/gui/buttons")
addPath("${srcDir}/gui/containers")
addPath("${srcDir}/gui/editor")
addPath("${srcDir}/gui/language")
addPath("${srcDir}/input")
addPath("${srcDir}/io")
addPath("${srcDir}/io/resource")
addPath("${srcDir}/io/zip")
addPath("${srcDir}/math")
addPath("${srcDir}/memory")
addPath("${srcDir}/messaging")
addPath("${srcDir}/module")
addPath("${srcDir}/network")
addPath("${srcDir}/persistence")
addPath("${srcDir}/persistence/rapidjson/include/rapidjson")
addPath("${srcDir}/persistence/rapidjson/include/rapidjson/internal")
addPath("${srcDir}/persistence/taml")
addPath("${srcDir}/persistence/taml/binary")
addPath("${srcDir}/persistence/taml/json")
addPath("${srcDir}/persistence/taml/xml")
addPath("${srcDir}/persistence/tinyXML")
addPath("${srcDir}/platform")

addPath("${srcDir}/platform/menus")
addPath("${srcDir}/platform/nativeDialogs")
addPath("${srcDir}/platform/threads")
addPath("${srcDir}/sim")
addPath("${srcDir}/spine")
addPath("${srcDir}/string")
addPath("${srcDir}/testing")
addPath("${srcDir}/testing/tests")
addPath("${srcDir}/2d")
addPath("${srcDir}/2d/assets")
addPath("${srcDir}/2d/controllers")
addPath("${srcDir}/2d/controllers/core")
addPath("${srcDir}/2d/core")
addPath("${srcDir}/2d/experimental")
addPath("${srcDir}/2d/experimental/composites")
addPath("${srcDir}/2d/gui")
addPath("${srcDir}/2d/scene")
addPath("${srcDir}/2d/sceneobject")

if(UNIX AND NOT APPLE)
    option(AL_ALEXT_PROTOTYPES "Use Extended OpenAL options" ON)
endif()
if(APPLE)
    option(AL_ALEXT_PROTOTYPES "Use Extended OpenAL options" OFF)
    addFramework("OpenAL")
endif()

# Vorbis
addInclude(${libDir}/libvorbis/include)
addLib(libvorbis)
addLib(libogg)

if(WIN32)
    addPath("${srcDir}/platformWin32")
    addPath("${srcDir}/platformWin32/nativeDialogs")
    addPath("${srcDir}/platformWin32/menus")
    addPath("${srcDir}/platformWin32/threads")
    # add windows rc file for the icon
    addFile("${projectOutDir}/Torque 2D.rc")
endif()

if(APPLE)
    addPath("${srcDir}/platformOSX")
endif()

if(UNIX AND NOT APPLE)
    addPath("${srcDir}/platformX86UNIX")
endif()

###############################################################################
###############################################################################
finishExecutable()
###############################################################################
###############################################################################

# Set Visual Studio startup project
if((${CMAKE_VERSION} VERSION_EQUAL 3.6.0) OR (${CMAKE_VERSION} VERSION_GREATER 3.6.0) AND MSVC)
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TORQUE_APP_NAME})
endif()

if(NOT EXISTS "${projectOutDir}/${PROJECT_NAME}.torsion")
    CONFIGURE_FILE("${cmakeDir}/template.torsion.in" "${projectOutDir}/${PROJECT_NAME}.torsion")
endif()

if(NOT EXISTS "${projectOutDir}/Torque 2D.ico")
      CONFIGURE_FILE("${cmakeDir}/Torque 2D.ico" "${projectOutDir}/Torque 2D.ico" COPYONLY)
endif()
if(WIN32)
    if(NOT EXISTS "${projectSrcDir}/Torque 2D.rc")
        CONFIGURE_FILE("${cmakeDir}/Torque 2D.rc.in" "${projectOutDir}/Torque 2D.rc")
    endif()
    if(NOT EXISTS "${projectOutDir}/cleandso.bat")
        CONFIGURE_FILE("${cmakeDir}/cleandso.bat.in" "${projectOutDir}/cleandso.bat")
    endif()
endif()

###############################################################################
# Common Libraries
###############################################################################
addLib(lpng)
addLib(ljpeg)
addLib(zlib)
addLib(Box2D)

if(WIN32)
	set(TORQUE_EXTERNAL_LIBS "COMCTL32.LIB;COMDLG32.LIB;USER32.LIB;ADVAPI32.LIB;GDI32.LIB;RPCRT4.LIB;WINMM.LIB;WS2_32.LIB;vfw32.lib;Imm32.lib;shell32.lib;shlwapi.lib;ole32.lib" CACHE STRING "external libs to link against")
	mark_as_advanced(TORQUE_EXTERNAL_LIBS)
	addLib("${TORQUE_EXTERNAL_LIBS}")
endif()


###############################################################################
# Always enabled Definitions
###############################################################################
addDef(TORQUE_DEBUG Debug)
addDef(TORQUE_DEBUG_GUARD)
addDef(_CRT_SECURE_NO_WARNINGS)
addDef(_CRT_SECURE_NO_DEPRECATE)
addDef(UNICODE)
addDef(_UNICODE) # for VS
addDef(TORQUE_UNICODE)
addDef(DEBUG=1)

##TORQUE_DEBUG;TORQUE_DEBUG_GUARD;_CRT_SECURE_NO_DEPRECATE;UNICODE;

if(UNIX AND NOT APPLE)
	addDef(LINUX)
endif()

###############################################################################
# Include Paths
###############################################################################
addInclude("${srcDir}/")
addInclude("${libDir}/")
addInclude("${libDir}/lpng")
addInclude("${libDir}/ljpeg")
addInclude("${libDir}/zlib")
addInclude("${libDir}/libogg/include")
addInclude("${srcDir}/persistence/rapidjson")
addInclude("${srcDir}/persistence/libjson")
addInclude("${srcDir}/testing/googleTest")
addInclude("${srcDir}/testing/googleTest/include")
addInclude("${srcDir}/spine")
addInclude("${libDir}/Box2D")
addInclude("${libDir}/openal/win32/al")
if(UNIX AND NOT APPLE)
	addInclude("/usr/include/freetype2/freetype")
	addInclude("/usr/include/freetype2")
endif()

if(MSVC)
    # Match projectGenerator naming for executables
    set(OUTPUT_CONFIG DEBUG MINSIZEREL RELWITHDEBINFO)
    set(OUTPUT_SUFFIX DEBUG MINSIZE    OPTIMIZEDDEBUG)
    foreach(INDEX RANGE 2)
        list(GET OUTPUT_CONFIG ${INDEX} CONF)
        list(GET OUTPUT_SUFFIX ${INDEX} SUFFIX)
        set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_${CONF} ${PROJECT_NAME}_${SUFFIX})
    endforeach()
endif()

###############################################################################
# Project-specific configuration:
###############################################################################

include(${TORQUE_APP_DIR}/${PROJECT_NAME}.cmake OPTIONAL)

###############################################################################
# Installation
###############################################################################

if(TORQUE_TEMPLATE)
    message("Prepare Template(${TORQUE_TEMPLATE}) install...")
    file(GLOB_RECURSE INSTALL_FILES_AND_DIRS "${CMAKE_SOURCE_DIR}/Templates/${TORQUE_TEMPLATE}/*")

    foreach(ITEM ${INSTALL_FILES_AND_DIRS})
        get_filename_component( dir ${ITEM} DIRECTORY )
        STRING(REGEX REPLACE "${CMAKE_SOURCE_DIR}/Templates/${TORQUE_TEMPLATE}/" "${TORQUE_APP_DIR}/" INSTALL_DIR ${dir})
        install( FILES ${ITEM} DESTINATION ${INSTALL_DIR} )
    endforeach()
endif()