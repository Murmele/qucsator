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

#Check dependencies: bison, flex, sed, gperf ...
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake)
if (ADMSXML) # set in dependencies.cmake
	set(USE_VERILOG true)
else()
	set(USE_VERILOG false)
endif()

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

# Do some checks
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/checks.cmake)

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
	add_subdirectory(interface)
	add_subdirectory(components/verilog)
endif()
add_subdirectory(math)

# Qucsconv application
add_subdirectory(converter)

## Linux?
#set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")

#
# Build libqucs as SHARED, dynamic library
#
# After: - http://stackoverflow.com/questions/11429055/cmake-how-create-a-
# single-shared-library-from-all-static-libraries-of-subprojec
#
add_library(
  libqucsator SHARED
  ${LIBQUCS_SRC}
  gperfapphash.cpp

  )
target_link_libraries(libqucsator PUBLIC
	coreMath
	coreComponents
	coreMicrostrip
	coreDevices
	coreDigital
	${CMAKE_DL_LIBS} # for dlerror(), dlopen() ...
)
if (USE_VERILOG)
	target_link_libraries(libqucsator PUBLIC coreInterface coreVerilog)
endif()

target_include_directories(libqucsator
	PUBLIC ${PROJECT_SOURCE_DIR}
	PRIVATE
	${qucsator_BINARY_DIR}	    # qucs_typedefs.h
	${qucsator_app_BINARY_DIR}  # generated sources by bison and flex are located here
	)

# rename the library to let it be libqucsator (not liblibqucsator).
# The target name must be unique and qucsator is already used for the application
set_target_properties(libqucsator PROPERTIES OUTPUT_NAME qucsator)

#
# Create target to handle gperfapp dependency
#
add_custom_target(equation DEPENDS ${qucsator_app_BINARY_DIR}/gperfapphash.cpp
	${PROJECT_SOURCE_DIR}/equation.cpp)
add_dependencies(libqucsator equation)

#
# Create qucsator application
#
add_executable(qucsator ucs.cpp)
target_include_directories(qucsator
	PRIVATE
	${qucsator_SOURCE_DIR}
	${qucsator_BINARY_DIR}
	)
## DL libs needed for opening a shared library
target_link_libraries(qucsator libqucsator ${CMAKE_DL_LIBS})

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
