set(SOURCES Calculator.cpp
Calculator.h
help.cpp
HelpWindow.cpp
HelpWindow.h
${CMAKE_CURRENT_BINARY_DIR}/icons.cpp
${CMAKE_CURRENT_BINARY_DIR}/icons.h
main.cpp
Preferences.cpp
Preferences.h
)

set(ICONS bigcalc.gif
colors.gif
information.gif
constmem.bmp
question.gif
tinycalc.gif)

add_executable(calculator ${SOURCES})
target_link_libraries(calculator FX::FOX)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${CMAKE_CURRENT_BINARY_DIR}/icons.h
  COMMAND reswrap -h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.h ${ICONS}
  COMMAND reswrap -i ${CMAKE_CURRENT_BINARY_DIR}/icons.h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap ${ICONS}
  VERBATIM
  )

# add_custom_command(
#   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/help.cpp ${CMAKE_CURRENT_BINARY_DIR}/help.h
#   COMMAND reswrap -h -a -o ${CMAKE_CURRENT_BINARY_DIR}/help.h help.txt
#   COMMAND reswrap -i ${CMAKE_CURRENT_BINARY_DIR}/help.h -a -o ${CMAKE_CURRENT_BINARY_DIR}/help.cpp help.txt
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#   DEPENDS reswrap help.txt
#   VERBATIM
#   )

install(TARGETS calculator RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Applications)
