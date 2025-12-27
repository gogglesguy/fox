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


# CHART library - follows BUILD_SHARED_LIBS like FOX
# When FOX is shared, CHART is shared; when FOX is static, CHART is static
add_library(CHART ${SOURCES})
target_link_libraries(CHART PUBLIC FX::FOX)

# When building CHART as shared library, define CHARTDLL_EXPORTS for dllexport
# Consumers (like charttest) will automatically use dllimport (no define needed)
if(BUILD_SHARED_LIBS)
  target_compile_definitions(CHART PRIVATE CHARTDLL_EXPORTS)
endif()



set(ICONS marble.bmp)

add_executable(charttest charttest.cpp
               ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp
               ${CMAKE_CURRENT_BINARY_DIR}/icons.h)
target_link_libraries(charttest PUBLIC CHART)

# Icon generation (RESWRAP_CMD/RESWRAP_DEPENDS set by utils/CMakeLists.txt)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${CMAKE_CURRENT_BINARY_DIR}/icons.h
  COMMAND ${RESWRAP_CMD} -h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.h ${ICONS}
  COMMAND ${RESWRAP_CMD} -i ${CMAKE_CURRENT_BINARY_DIR}/icons.h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${RESWRAP_DEPENDS} ${ICONS}
  VERBATIM
  )
