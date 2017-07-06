set(SOURCES ControlPanel.cpp
ControlPanel.h
icons.cpp
icons.h
)


set(ICONS colors_gif.gif
controlpanel_gif.gif
filebinding_gif.gif
settings_gif.gif
)

add_executable(ControlPanel ${SOURCES})
target_link_libraries(ControlPanel FOX)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )