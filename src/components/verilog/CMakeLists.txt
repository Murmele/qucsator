project(verilog)
cmake_minimum_required(VERSION 3.16)

include(${PROJECT_SOURCE_DIR}/../../../../cmake/readMakeList.txt)

#include_directories(
#  ${qucsator_SOURCE_DIR}
#  ${qucsator_CURRENT_SOURCE_DIR}
#  ${qucsator_SOURCE_DIR}/src/math
#  ${qucsator_SOURCE_DIR}/src/components # component.h
#  ${qucsator_SOURCE_DIR}/src/components/devices) # devices.h

readVariable(MakeList VERILOG_FILES VA_FILES)
readVariable(MakeList VERILOG_XML XML_BUILD)


##
# Create list of XML Build files
# Concatenate scripts into command: -e script1 [-e script2]
set(XML_CMD)
foreach(script ${XML_BUILD})
  set(XML_CMD ${XML_CMD} -e ${CMAKE_CURRENT_SOURCE_DIR}/${script})
endforeach()

## Process each Verilog-A file.
message("ADMSXML: ${ADMSXML}")
set(gen_SRC)
foreach(file ${VA_FILES})
  get_filename_component(_name ${file} NAME_WE)
  set(output
	  ${_name}.${_name}.analogfunction.h
	  ${_name}.${_name}.analogfunction.cpp
	  ${_name}.${_name}.core.cpp
	  ${_name}.core.h
	  ${_name}.cpp
	  ${_name}.defs.h)
  # custom command/rule to generate outputs with admsXml
  add_custom_command(
	  OUTPUT ${output}
	COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${XML_CMD} -o
	${_name}
	DEPENDS ${file} ${XML_BUILD})
list(APPEND gen_SRC ${_name}.cpp)
endforeach()

add_library(coreVerilog OBJECT ${gen_SRC})

## Distribute XML scripts
set(XML_DIST ${XML_BUILD} qucsMODULEgui.xml qucsMODULEguiJSONsymbol.xml)

set(MAKE_FILES cpp2lib.makefile va2cpp.makefile)

install(FILES ${XML_DIST} DESTINATION include/qucsator)
install(FILES ${MAKE_FILES} DESTINATION include/qucsator)
