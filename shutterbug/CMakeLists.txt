set(SOURCES icons.cpp
icons.h
ShutterBug.cpp
ShutterBug.h
Snapper.cpp
Snapper.h
)

set(ICONS line_0.gif
line_1.gif
line_2.gif
line_3.gif
line_4.gif
line_5.gif
line_6.gif
line_7.gif
line_8.gif
shutterbug.gif
tinyshutterbug.gif)

add_executable(shutterbug ${SOURCES})
target_link_libraries(shutterbug FOX)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

install(TARGETS shutterbug RUNTIME DESTINATION bin)
