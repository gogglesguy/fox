include(CheckIncludeFileCXX)

set(SOURCES CopyDialog.cpp
CopyDialog.h
PathFinder.cpp
PathFinder.h
Preferences.cpp
Preferences.h
PropertyDialog.cpp
PropertyDialog.h
${CMAKE_CURRENT_BINARY_DIR}/icons.cpp
${CMAKE_CURRENT_BINARY_DIR}/icons.h
)

set(ICONS bigicons.bmp
blocks_gif.gif
bookclr_gif.gif
bookdel_gif.gif
bookmrk_gif.gif
booknxt_gif.gif
bookprv_gif.gif
bookset_gif.gif
closepanel.gif
config_gif.gif
copy_gif.gif
cut_gif.gif
delete_gif.gif
desktop.bmp
details.bmp
dirup_gif.gif
enter.gif
file_gif.gif
filehidden.gif
fileshown.gif
foldernew_gif.gif
foxbig.gif
foxmini.gif
goback_gif.gif
godown_gif.gif
goforw_gif.gif
gotodir.bmp
goup_gif.gif
home_gif.gif
hosts.bmp
iconpath.gif
link_gif.gif
location.gif
maphost.bmp
miscellaneous_gif.gif
move_gif.gif
newfolder.bmp
paste.gif
pattern_gif.gif
property_gif.gif
quit_gif.gif
rename_gif.gif
rotateleft.gif
rotateright.gif
run_bmp.bmp
search.gif
setdir.gif
smallicons.bmp
sorting.bmp
unmaphost.bmp
warningicon_gif.gif
work_gif.gif)


check_include_file_cxx(sys/wait.h HAVE_SYS_WAIT_H)
if(HAVE_SYS_WAIT_H)
  add_definitions(-DHAVE_SYS_WAIT_H)
endif()

add_executable(PathFinder ${SOURCES})
target_link_libraries(PathFinder FX::FOX)

# Icon generation
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${CMAKE_CURRENT_BINARY_DIR}/icons.h
  COMMAND ${RESWRAP_CMD} -h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.h ${ICONS}
  COMMAND ${RESWRAP_CMD} -i ${CMAKE_CURRENT_BINARY_DIR}/icons.h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${RESWRAP_DEPENDS} ${ICONS}
  VERBATIM
  )

install(TARGETS PathFinder RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Applications)
