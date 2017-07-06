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

add_library(CHART-1.7 ${SOURCES})
add_library(CHART ALIAS CHART-1.7)
target_link_libraries(CHART-1.7 FOX)


set(ICONS marble.bmp)

add_executable(charttest charttest.cpp icons.cpp icons.h)
target_link_libraries(charttest CHART)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )
