set(SOURCES ControlPanel.cpp
ControlPanel.h
${CMAKE_CURRENT_BINARY_DIR}/icons.cpp
${CMAKE_CURRENT_BINARY_DIR}/icons.h
)


set(ICONS colors_gif.gif
controlpanel_gif.gif
filebinding_gif.gif
settings_gif.gif
)

add_executable(ControlPanel ${SOURCES})
target_link_libraries(ControlPanel FX::FOX)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${CMAKE_CURRENT_BINARY_DIR}/icons.h
  COMMAND reswrap -h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.h ${ICONS}
  COMMAND reswrap -i ${CMAKE_CURRENT_BINARY_DIR}/icons.h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap ${ICONS}
  VERBATIM
  )

install(TARGETS ControlPanel RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Applications)
