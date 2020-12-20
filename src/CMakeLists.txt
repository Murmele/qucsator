project(qucsator_app CXX C)
cmake_minimum_required(VERSION 3.16)

include(${PROJECT_SOURCE_DIR}/../../cmake/readMakeList.txt)

# qucsator_SOURCE_DIR is set when the project qucsator exists.
# If this project is build by it self, this variable is not set
if (NOT qucsator_SOURCE_DIR)
        set(qucsator_SOURCE_DIR ${PROJECT_SOURCE_DIR}/..)
	set(qucsator_BINARY_DIR ${qucsator_app_BINARY_DIR})
endif()

#TODO: do that only if they are not set previously
# Define to the address where bug reports for this package should be sent.
set(PACKAGE_BUGREPORT "qucs-bugs@lists.sourceforge.net")

# Define to the full name of this package.
set(PACKAGE_NAME "qucsator")

# Define to the full name and version of this package.
set(PACKAGE_STRING "${PACKAGE_NAME} ${PROJECT_VERSION}")

# Define to the one symbol short name of this package.
set(PACKAGE_TARNAME ${PACKAGE_NAME})

# Define to the home page for this package.
set(PACKAGE_URL "http://sourceforge.net/projects/qucs/")

##
## Checks for libraries.
##
## AC_CHECK_LIB(m, sin) need to check for sin?
#if(NOT WIN32)
#  find_library(MATH_LIB NAMES m)
#  if(NOT MATH_LIB)
#    message(SEND_ERROR "Math lib not found: ${MATH_LIB}")
#  else()
#    message(STATUS "Math lib found at: ${MATH_LIB}")
#  endif()
#endif()

##
## Checks for header files. AC_HEADER_STDC !! obsolete, need to check? Define
## STDC_HEADERS if the system has ANSI C header files. Specifically, this macro
## checks for `stdlib.h', `stdarg.h', `string.h', and `float.h'; Lifted the cmake
## checks from gd-libdg, Lua https://bitbucket.org/libgd/gd-libgd
## https://github.com/LuaDist/libgd/tree/master/cmake/modules
#include(CheckIncludeFiles)
#set(CMAKE_REQUIRED_INCLUDES "/usr/include" "/usr/local/include")
#set(CMAKE_MODULE_PATH "${qucs-core_SOURCE_DIR}/cmake/modules")
#include(AC_HEADER_STDC)

##
## Further header checks
##
#include(CheckIncludeFile)

## list of headers to be checked
#set(INCLUDES ieeefp.h memory.h stddef.h stdlib.h string.h unistd.h)

##
## Check if header can be included. * Define HAVE_[basename]_H to 1 if you have
## the header.
##
#foreach(header ${INCLUDES})
#  get_filename_component(base ${header} NAME_WE)
#  string(TOUPPER ${base} base)
#  check_include_file(${header} HAVE_${base}_H)
#  # MESSAGE(STATUS "${header}  --> ${HAVE_${base}_H}")
#endforeach()

## Checks for typedefs, structures, and compiler characteristics. AC_C_CONST
## !!obsolete AC_C_CONST "This macro is obsolescent, as current C compilers
## support `const'. New programs need not use this macro."

#
# Check for type sizes.
#
include(CheckTypeSize)
check_type_size("short" SIZEOF_SHORT)
check_type_size("int" SIZEOF_INT)
check_type_size("long" SIZEOF_LONG)
check_type_size("double" SIZEOF_DOUBLE)
check_type_size("long double" SIZEOF_LONG_DOUBLE)
# MESSAGE(STATUS "short  ${SIZEOF_SHORT}" ) MESSAGE(STATUS "int   ${SIZEOF_INT}"
# ) MESSAGE(STATUS "long  ${SIZEOF_LONG}" ) MESSAGE(STATUS "double
# ${SIZEOF_DOUBLE}" ) MESSAGE(STATUS "long double ${SIZEOF_LONG_DOUBLE}" )

#
# Check for double type. * valid types are: double, float and long double. *
# defines: nr_double_t,  The global type of double representation. * defines:
# NR_DOUBLE_SIZE,  The size of the double representation. * Use -DENABLE-
# DOUBLE="[float,double, long double]"
if(ENABLE-DOUBLE)
  # User defined
  set(DoubleType ${ENABLE-DOUBLE})

  # valid types
  set(ValidTypes "float" "double" "long double")

  list(FIND ValidTypes ${DoubleType} HasType)
  if(HasType EQUAL -1)
          message(FATAL_ERROR "Valid types are: ${ValidTypes}")
  endif()

  # The global type of double representation.
  set(nr_double_t DoubleType)
  check_type_size(${DoubleType} DoubleSize)

  # The size of the double representation.
  set(NR_DOUBLE_SIZE ${DoubleSize})
else()
  # Default double
  set(DoubleType "double")
  # The global type of double representation.
  set(nr_double_t ${DoubleType})
  check_type_size(${DoubleType} DoubleSize)

  # The size of the double representation.
  set(NR_DOUBLE_SIZE ${DoubleSize})
endif()
message(STATUS "using double type: ${DoubleType}; size: ${DoubleSize}")

# defines used in qucs_typedefs.h
set(QUCS_INT32_TYPE ${nr_int32_t})
set(QUCS_INT16_TYPE ${nr_int16_t})
set(QUCS_DOUBLE_TYPE ${DoubleType})
set(QUCS_DOUBLE_SIZE ${DoubleSize})
##
# Configure the header qucs_typedefs.h, interpolate above definitions.
#
configure_file("${qucsator_SOURCE_DIR}/qucs_typedefs.h.cmake"
        "${qucsator_BINARY_DIR}/qucs_typedefs.h")

#
# Check for library functions * not all functons seem to be used after defined.
# TODO check for HAVE_{func}
#
include(CheckFunctionExists)
set(REQUIRED_FUNCTIONS
    floor
    pow
    exp
    sqrt
    log10
    log
    cos
    sin
    acos
    asin # for real.cpp
    tan
    atan
    sinh
    cosh
    tanh
    fabs
    modf
    atan2
    jn
    yn
    erf
    erfc # for fspecial.cpp
    round
    trunc
    acosh
    asinh # for real.cpp
    strdup
    strerror
    strchr) # for compat.h, matvec.cpp, scan_*.cpp

foreach(func ${REQUIRED_FUNCTIONS})
  string(TOUPPER ${func} FNAME)
  check_function_exists(${func} HAVE_${FNAME})
  # message(STATUS "${func}  --> ${HAVE_${FNAME}}")
endforeach()

##
## Checks for complex classes and functions, as in the Autotools scripts.
##
## AC_CXX_NAMESPACES !!custom m4 AC_CXX_HAVE_COMPLEX !!custom m4
## AC_CXX_HAVE_TR1_COMPLEX !!custom m4 AC_CHECK_CXX_COMPLEX_FUNCS([cos cosh exp
## log log10 sin sinh sqrt tan tanh]) !!custom m4
## AC_CHECK_CXX_COMPLEX_FUNCS([acos acosh asin asinh atan atanh])
## AC_CHECK_CXX_COMPLEX_FUNCS([log2 norm]) AC_CHECK_CXX_COMPLEX_POW
## AC_CHECK_CXX_COMPLEX_ATAN2         !failed, need libstdc?
## AC_CHECK_CXX_COMPLEX_FMOD          !failed: AC_CHECK_CXX_COMPLEX_POLAR
## AC_CHECK_CXX_COMPLEX_POLAR_COMPLEX !failed

##
## Namespace
##
## Check whether the compiler implements namespaces
##
#try_compile(
#  HAVE_NAMESPACES ${CMAKE_BINARY_DIR}
#  ${qucs-core_SOURCE_DIR}/cmake/namespaces.cpp OUTPUT_VARIABLE TRY_OUT)
#if(NOT HAVE_NAMESPACES)
#  message(
#    SEND_ERROR
#      "${PROJECT_NAME} requires an c++ compiler with namespace HAVE_NAMESPACES failed"
#  ) # ${TRY_OUT}")
#endif()

##
## Check whether the compiler has complex<T>
##
#try_compile(HAVE_COMPLEX ${CMAKE_BINARY_DIR}
#            ${qucs-core_SOURCE_DIR}/cmake/complex.cpp OUTPUT_VARIABLE TRY_OUT)
#if(NOT HAVE_COMPLEX)
#  message(SEND_ERROR "HAVE_COMPLEX failed") # ${TRY_OUT}")
#endif()

##
## Check std::vector::erase iterator type [const_iterator | iterator]
##
#message(STATUS "Checking HAVE_ERASE_CONSTANT_ITERATOR")
#try_compile(
#  HAVE_ERASE_CONSTANT_ITERATOR ${CMAKE_BINARY_DIR}
#  ${qucs-core_SOURCE_DIR}/cmake/erase_iterator_type.cpp OUTPUT_VARIABLE TRY_OUT)
#if(HAVE_ERASE_CONSTANT_ITERATOR)
#  message(STATUS "Using std::vector:erase iterator type : const_iterator")
#else()
#  message(STATUS "Using std::vector:erase iterator type : iterator")
#endif()

##
## Check for list of complex functions.
##
#set(COMPLEX_FUNCS_GRP1
#    acos
#    acosh
#    asin
#    asinh
#    atan
#    atanh
#    cos
#    cosh
#    exp
#    log
#    log10
#    sin
#    sinh
#    sqrt
#    tan
#    tanh
#    log2
#    norm)
#set(COMPLEX_FUNCS_GRP2 pow atan2 fmod polar polar_complex)

##
## test complex function group 1 code inlined to easily replace '${func}'
##
#foreach(func ${COMPLEX_FUNCS_GRP1})
#  set(code
#      " #include <complex>
#      using namespace std\;
#    #ifdef log2
#    #undef log2
#    #endif

#    int main() {
#      complex<double> a\;
#      ${func}(a)\;
#      return 0\;
#    }")

#  file(WRITE ${qucsator_SOURCE_DIR}/cmake/test_${func}.cpp ${code})

#  string(TOUPPER ${func} FNAME)

#  message(STATUS "Checking HAVE_CXX_COMPLEX_${FNAME}")

#  try_compile(
#    HAVE_CXX_COMPLEX_${FNAME} ${CMAKE_BINARY_DIR}
#    ${qucsator_SOURCE_DIR_SOURCE_DIR}/cmake/test_${func}.cpp OUTPUT_VARIABLE TRY_OUT)
#  if(NOT HAVE_CXX_COMPLEX_${FNAME})
#    message(STATUS "HAVE_CXX_COMPLEX_${FNAME} failed") # ${TRY_OUT}")
#  endif()

#  file(REMOVE ${qucsator_SOURCE_DIR}/cmake/test_${func}.cpp ${code})
#endforeach()

##
## test complex function group 2 use prepared source file.
##
#foreach(func ${COMPLEX_FUNCS_GRP2})
#  string(TOUPPER ${func} FNAME)

#  message(STATUS "Checking HAVE_CXX_COMPLEX_${FNAME}")

#  try_compile(
#    HAVE_CXX_COMPLEX_${FNAME} ${CMAKE_BINARY_DIR}
#    ${qucs-core_SOURCE_DIR}/cmake/complex_${func}.cpp
#    COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
#    OUTPUT_VARIABLE TRY_OUT)
#  if(NOT HAVE_CXX_COMPLEX_${FNAME})
#    message(STATUS "HAVE_CXX_COMPLEX_${FNAME} failed") # ${TRY_OUT}")
#  endif()
#endforeach()

set(USE_VERILOG false)

#
# Configure the header config.h, interpolate above definitions.
#
configure_file("${qucsator_SOURCE_DIR}/config.h.cmake"
        "${qucsator_BINARY_DIR}/config.h")

#
# List of lexer/parsers type names
#
set(ParserTypes
        csv
	citi
	dataset
	mdl
	netlist
	touchstone
	zvr)

set(generated_SRC)
foreach(type ${ParserTypes})
  # Create custom Bison
  set(bisonIn "${CMAKE_CURRENT_SOURCE_DIR}/parse_${type}.ypp")
  set(bisonOut "parse_${type}.hpp"
          "parse_${type}.cpp")
  add_custom_command(
          OUTPUT ${bisonOut}
	COMMAND
	${BISON_EXECUTABLE}
	  --defines=parse_${type}.hpp
	  --output=parse_${type}.cpp
	  ${bisonIn}
	  DEPENDS ${bisonIn})
    # Create custom Flex
    set(flexIn "${CMAKE_CURRENT_SOURCE_DIR}/scan_${type}.lpp")
    set(flexOut "scan_${type}.cpp")
    add_custom_command(
	      OUTPUT ${flexOut}
	    #COMMAND ${FLEX_EXECUTABLE} --outfile=${flexOut} ${flexIn} # not working on windows 10?
	    COMMAND ${FLEX_EXECUTABLE} -o"${flexOut}" ${flexIn}
	    DEPENDS ${flexIn})

    list(APPEND generated_SRC ${bisonOut})
    list(APPEND generated_SRC ${flexOut})
endforeach()


#
# Source code libqucs
#
#    equation.cpp # <= depends on gperfapphash.cpp
#list(LENGTH ${generated_SRC} NUMBER_ELEMENTS)
#message("NUMBER_ELEMENTS: ${NUMBER_ELEMENTS}")
#if (${NUMBER_ELEMENTS} LESS_EQUAL "0")
#	message(FATAL_ERROR "No files generated")
#endif(
readVariable(MakeList SRCS LIBQUCS_SRC)
list(APPEND LIBQUCS_SRC ${generated_SRC})

#
# Template classes
#
readVariable(MakeList TEMPLATE_HDRS TEMPLATES)

#
# Include headers to be installed
#
readVariable(MakeList HDRS PUBLIC_HEADERS)
list(APPEND PUBLIC_HEADERS
        ${qucsator_BINARY_DIR}/config.h
	${qucsator_BINARY_DIR}/qucs_typedefs.h)

#
# Replace 'evaluate::[whatever]' by NULL
#
# * evaluate.h (class evaluate): New class implementing the actual evaluation
#   function (applications) for the equations in Qucs.
#
if (UNIX)
 add_custom_command(
   OUTPUT gperfappgen.h
   COMMAND
     ${SED_TOOL} -e 's/evaluate::[a-zA-Z0-9_]*/NULL/g' <
     ${CMAKE_CURRENT_SOURCE_DIR}/applications.h >
     ${CMAKE_CURRENT_BINARY_DIR}/gperfappgen.h
   DEPENDS ${applications.h})
else()
add_custom_command(
  OUTPUT gperfappgen.h
  COMMAND
    ${SED_TOOL} -e "s/evaluate::[a-zA-Z0-9_]*/NULL/g" <
    ${CMAKE_CURRENT_SOURCE_DIR}/applications.h >
    ${CMAKE_CURRENT_BINARY_DIR}/gperfappgen.h
  DEPENDS ${applications.h})
endif()

#
# Compile gperfappgen * used to generate gperf input file (used in qucsator)
#
set(gperf_SRC gperfappgen.cpp gperfappgen.h)
add_executable(gperfappgen ${gperf_SRC})
target_include_directories(gperfappgen PRIVATE ${qucsator_BINARY_DIR}
                            ${qucsator_app_BINARY_DIR}
                            ${qucsator_app_SOURCE_DIR}
			    ${qucsator_app_SOURCE_DIR}/math
			    )

#
# Run gperfappgen, pipe to gperf input to gperfapphash.gph
#
add_custom_command(
  OUTPUT gperfapphash.gph
  COMMAND gperfappgen > ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.gph
  DEPENDS gperfappgen
  COMMENT "gperfappgen gets executed"
  VERBATIM)

#
# Run gperf, create hash table. * -I, Include the necessary system include files
# at the beginning of the code. * -m, Perform multiple iterations to minimize
# generated table. * Replace '{""}' by '{"",0}; (why?)
#
if (UNIX)
 add_custom_command(
   OUTPUT gperfapphash.cpp
   COMMAND ${GPERF_TOOL} -I -m 8 ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.gph >
           temp.gperf
   COMMAND ${SED_TOOL} -e 's/{""},/{"",0},/g' < temp.gperf > ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.cpp
   DEPENDS gperfapphash.gph)
else()
add_custom_command(
  OUTPUT gperfapphash.cpp
  COMMAND ${GPERF_TOOL} -I -m 8 ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.gph >
          temp.gperf
  COMMAND ${SED_TOOL} -e "s/{\"\"},/{\"\",0},/g" < temp.gperf > ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.cpp
  DEPENDS gperfapphash.gph)
endif()

 #target <- source (includea) equation.cpp: gperfapphash.cpp

 #noinst_PROGRAMS = gperfappgen gperfappgen_SOURCES = gperfappgen.cpp

# for cleaning (autogenerated) set(gperf_FILES gperfapphash.cpp gperfapphash.gph
# gperfappgen.h)

## Qucs library dependencies
add_subdirectory(components)
add_subdirectory(components/digital)
add_subdirectory(components/devices)
add_subdirectory(components/microstrip)
if (USE_VERILOG)
    message("Verilog used")
    add_subdirectory(interface)
    add_subdirectory(components/verilog)
endif()
add_subdirectory(math)

## Qucsconv application
#add_subdirectory(converter)

## Linux?
#set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")

#
# Build libqucs as SHARED, dynamic library
#
# After: - http://stackoverflow.com/questions/11429055/cmake-how-create-a-
# single-shared-library-from-all-static-libraries-of-subprojec
#
add_library(
  libqucsator STATIC
  ${LIBQUCS_SRC}
  $<TARGET_OBJECTS:coreComponents>
  $<TARGET_OBJECTS:coreMicrostrip>
  $<TARGET_OBJECTS:coreDevices>
  $<TARGET_OBJECTS:coreDigital>
  gperfapphash.cpp
  $<TARGET_OBJECTS:coreMath>
  )

#target_link_libraries(libqucsator PUBLIC
#	coreMath
#	coreComponents
##	coreInterface
##	coreVerilog
#coreMicrostrip
#coreDevices
#coreDigital
#${CMAKE_DL_LIBS} # for dlerror(), dlopen() ...
#)

target_include_directories(libqucsator
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/math
	${CMAKE_CURRENT_SOURCE_DIR}/components
	${CMAKE_CURRENT_SOURCE_DIR}/components/microstrip
	${CMAKE_CURRENT_SOURCE_DIR}/components/devices
	${CMAKE_CURRENT_SOURCE_DIR}/components/digital
	${qucsator_app_BINARY_DIR}  # generated sources by bison and flex are located here
	PRIVATE
	${qucsator_BINARY_DIR}	    # qucs_typedefs.h
	)

get_target_property(MY_SOURCES libqucsator SOURCES)
#message(FATAL_ERROR "${MY_SOURCES}")

# By default, symbols provided by the sublibs that are not used by mainlib (which is all of them in this case)
# are not used. This changes that.
#if (WIN32)
#    set_target_properties(libqucsator PROPERTIES
#	LINK_FLAGS "/WHOLEARCHIVE"
#    )
#elseif (APPLE)
#    set_target_properties(libqucsator PROPERTIES
#	LINK_FLAGS "-Wl,-all_load"
#    )
#else ()
#    set_target_properties(libqucsator PROPERTIES
#	LINK_FLAGS "-Wl,--whole-archive"
#    )
#endif ()
#set_target_properties(libqucsator PROPERTIES
#    LINK_FLAGS "-Wl,--whole-archive"
#)
# rename the library to let it be libqucsator (not liblibqucsator).
# The target name must be unique and qucsator is already used for the application
set_target_properties(libqucsator PROPERTIES OUTPUT_NAME qucsator)

#
# Create target to handle gperfapp dependency
#
add_custom_target(equation DEPENDS ${qucsator_app_BINARY_DIR}/gperfapphash.cpp
	${PROJECT_SOURCE_DIR}/equation.cpp)
add_dependencies(libqucsator equation)


# JUST FOR TESTING_ START
add_executable(test123 main_temp.cpp)
target_include_directories(test123
	PRIVATE
	${qucsator_SOURCE_DIR}
	${qucsator_BINARY_DIR}
	)
target_link_libraries(test123 libqucsator)
# JUST FOR TESTING_ ENDE

##
## Create qucsator application
##
#add_executable(qucsator ucs.cpp)
#target_include_directories(qucsator
#	PRIVATE
#	${qucsator_SOURCE_DIR}
#	${qucsator_BINARY_DIR}
#	)
### DL libs needed for opening a shared library
#target_link_libraries(qucsator libqucsator ${CMAKE_DL_LIBS})

##
## Handle install
##
#install(TARGETS qucsator DESTINATION bin)

## set Windows runtime location for libqucsator See:
## http://www.cmake.org/pipermail/cmake/2010-June/037461.html
#install(
#  TARGETS libqucsator
#  RUNTIME DESTINATION bin COMPONENT runtime
#  ARCHIVE DESTINATION lib COMPONENT devel
#  LIBRARY DESTINATION lib COMPONENT library)

#install(FILES ${PUBLIC_HEADERS} DESTINATION include/qucsator)
