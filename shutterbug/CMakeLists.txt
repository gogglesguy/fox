set(SOURCES ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp
${CMAKE_CURRENT_BINARY_DIR}/icons.h
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
target_link_libraries(shutterbug FX::FOX)

# Icon generation
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${CMAKE_CURRENT_BINARY_DIR}/icons.h
  COMMAND ${RESWRAP_CMD} -h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.h ${ICONS}
  COMMAND ${RESWRAP_CMD} -i ${CMAKE_CURRENT_BINARY_DIR}/icons.h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${RESWRAP_DEPENDS} ${ICONS}
  VERBATIM
  )

install(TARGETS shutterbug RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Applications)
