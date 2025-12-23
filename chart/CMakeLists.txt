set(SOURCES chart.h
chartdefs.h
chartutils.cpp
chartutils.h
FX2DChart.cpp
FX2DChart.h
FX2DPlot.cpp
FX2DPlot.h
FXAxis.cpp
FXAxis.h
FXChart.cpp
FXChart.h
FXCurve.cpp
FXCurve.h
)


# CHART library - internal static library for chart functionality
add_library(CHART STATIC ${SOURCES})
target_link_libraries(CHART PUBLIC FX::FOX)



set(ICONS marble.bmp)

add_executable(charttest charttest.cpp
               ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp
               ${CMAKE_CURRENT_BINARY_DIR}/icons.h)
target_link_libraries(charttest PUBLIC CHART)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${CMAKE_CURRENT_BINARY_DIR}/icons.h
  COMMAND reswrap -h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.h ${ICONS}
  COMMAND reswrap -i ${CMAKE_CURRENT_BINARY_DIR}/icons.h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap ${ICONS}
  VERBATIM
  )
