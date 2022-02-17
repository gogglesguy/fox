include(CheckIncludeFileCXX)

set(SOURCES CopyDialog.cpp
CopyDialog.h
PathFinder.cpp
PathFinder.h
Preferences.cpp
Preferences.h
PropertyDialog.cpp
PropertyDialog.h
icons.cpp
icons.h
)

set(ICONS addbook.bmp
bigicons.bmp
closepanel.gif
clrbook.gif
config_gif.gif
copy.bmp
copyit.gif
cut.bmp
delbook.bmp
deleteit.bmp
desktop.bmp
details.bmp
dirup.bmp
enter.gif
execute.bmp
file_gif.gif
filehidden.gif
fileshown.gif
foldernew.gif
foxbig.gif
foxmini.gif
goback.bmp
goforw.bmp
gotodir.bmp
home.gif
hosts.bmp
iconpath.gif
linkit.bmp
location.gif
maphost.bmp
miscellaneous_gif.gif
moveit.gif
newfolder.bmp
paste.bmp
pattern_gif.gif
properties.bmp
quit_gif.gif
renameit.gif
rotateleft.gif
rotateright.gif
run_bmp.bmp
setbook.gif
setdir.gif
smallicons.bmp
sorting.bmp
unmaphost.bmp
warningicon_gif.gif
work.gif
)


check_include_file_cxx(sys/wait.h HAVE_SYS_WAIT_H)
if(HAVE_SYS_WAIT_H)
  add_definitions(-DHAVE_SYS_WAIT_H)
endif()

add_executable(PathFinder ${SOURCES})
target_link_libraries(PathFinder FOX)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

install(TARGETS PathFinder RUNTIME DESTINATION bin)

