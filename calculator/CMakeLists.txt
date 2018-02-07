set(SOURCES Calculator.cpp
Calculator.h
help.cpp
HelpWindow.cpp
HelpWindow.h
icons.cpp
icons.h
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
target_link_libraries(calculator FOX)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

install(TARGETS calculator RUNTIME DESTINATION bin)
