## WINDOWS: Could NOT find BISON (missing: BISON_EXECUTABLE)
install BISON from: http://gnuwin32.sourceforge.net/packages/bison.htm

## WINDOWS: Could NOT find FLEX (missing: FLEX_EXECUTABLE)
install FLEX from: http://gnuwin32.sourceforge.net/packages/flex.htm

## In file included from C:\Users\mmarmsoler\Documents\Personal\GIT\qucs\qucsator\src\ucs.cpp:39: C:\Users\mmarmsoler\Documents\Personal\GIT\qucs\qucsator\src\components/component.h:35:10: fatal error: compat.h: No such file or directory #include "compat.h"
problem commit: adcda63f7cfd11f398b5cfed5110764a04e7f6f8

seems to be a problem in qucsator, because when commenting out qucsator target in src/CMakeLists.txt the compiler error does not occur anymore
#add_executable(qucsator ucs.cpp ${PUBLIC_HEADERS} ${TEMPLATES})
#target_include_directories(qucsator PRIVATE ${qucsator_SOURCE_DIR})

## C:/Users/mmarmsoler/Documents/Personal/GIT/qucs/qucsator/src/logging.h:32:1: error: '__BEGIN_DECLS' does not name a type __BEGIN_DECLS
occured during build of coreMath: Scanning dependencies of target coreMath
defined in the config file config.h in the qucsator folder
include config.h, how it should no about it?
#include "config.h"

## C:/Users/mmarmsoler/Documents/Personal/GIT/qucs/qucsator/src/compat.h:33:18: error: 'index' was not declared in this scope # define strchr  index
index is a deprecated posix function: https://stackoverflow.com/questions/4091864/c-differences-between-strchr-and-index
strchr is a standard c function.
solution commit: 87719aa0b7a2c185bdb531cae508836edfc66769

## windows: flex.exe: unknown flag '-'.
solution: 15c6aba89bc03e5c94f37bdc00cec813f68565b2

## windows: m4: cannot open `Files': No such file or directory
m4: cannot open `(x86)\GnuWin32/share/bison': No such file or directory
m4: cannot open `C:\Program': No such file or directory
m4: cannot open `Files': No such file or directory
m4: cannot open `(x86)\GnuWin32/share/bison/m4sugar/m4sugar.m4': No such file or directory
Problem during execution of bison.exe
https://stackoverflow.com/questions/16442556/m4-no-such-file-or-directory-bison

## C:\Users\mmarmsoler\Documents\Personal\GIT\qucs\qucsator\src\eqnsys.cpp:55: Fehler: redefinition of 'qucs::eqnsys<nr_type_t>::eqnsys()' eqnsys<nr_type_t>::eqnsys () {
solution: 9046e62088c1060d354916fafdd988c3c99e29a2

## windows: By not providing "FindSED.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "SED", but
  CMake did not find one.
  
 1) http://gnuwin32.sourceforge.net/packages/sed.htm
 2) find_program(SED_TOOL NAMES sed)

## windows: sed.exe: -e expression #1, char 1: unknown command: `''
replace '' by ""

## windows: add_custom_command(
  OUTPUT gperfapphash.gph
  COMMAND gperfappgen > ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.gph
  DEPENDS gperfappgen
  COMMENT "Gperfappgen gets executed"
  VERBATIM
Solution: It gets not executed, because gperfapphash is not needed anywhere. 
add_custom_command(
  OUTPUT gperfapphash.cpp
  COMMAND ${GPERF_TOOL} -I -m 8 ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.gph >
          temp.gperf
  COMMAND ${SED_TOOL} -e "s/{''},/{'',0},/g" < temp.gperf >
          ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.cpp
  DEPENDS gperfapphash.gph)
Seeing here, gperfapphash.gph is only needed if gperfapphash.cpp is needed.
So gperfapphash.cpp must be added as source to the libqucsator library. Then it is needed and so also the Command gets executed.
Solution commit: d4e6b0d0d8bf5eda6ebdf689407b5af09910c87f

## /usr/bin/ld: CMakeFiles/libqucsator.dir/gperfappgen.cpp.o:(.data.rel.local+0x0): multiple definition of `qucs::eqn::applications'; CMakeFiles/libqucsator.dir/evaluate.cpp.o:(.data.rel+0x0): first defined here
/usr/bin/ld: CMakeFiles/libqucsator.dir/gperfappgen.cpp.o: in function `qucs::eqn::checker::tag2key(int)':


