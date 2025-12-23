include(GNUInstallDirs)

add_executable(reswrap reswrap.cpp)

install(TARGETS reswrap RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
