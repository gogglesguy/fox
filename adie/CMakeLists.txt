set(SOURCES Adie.cpp
Commands.cpp
Commands.h
FindInFiles.cpp
FindInFiles.h
help.cpp
HelpWindow.cpp
icons.cpp
main.cpp
Modeline.cpp
Modeline.h
Preferences.cpp
Preferences.h
ShellCommand.cpp
ShellCommand.h
ShellDialog.cpp
ShellDialog.h
Syntax.cpp
Syntax.h
SyntaxParser.cpp
SyntaxParser.h
TextWindow.cpp
TextWindow.h)

set(ICONS adie_gif.gif
backward_gif.gif
big_gif.gif
bookdel_gif.gif
booknext_gif.gif
bookprev_gif.gif
bookset_gif.gif
browser.gif
capitalize.gif
close_gif.gif
colors_gif.gif
command_bmp.bmp
config_gif.gif
copy_gif.gif
cut_gif.gif
delete_gif.gif
delimit_gif.gif
filehidden.gif
fileshown.gif
fonts_gif.gif
forward_gif.gif
goto_gif.gif
help_gif.gif
indent_gif.gif
info_gif.gif
lang_gif.gif
logger.gif
lowercase.gif
miscellaneous_gif.gif
new_gif.gif
nobrowser.gif
nologger.gif
open_gif.gif
palette_gif.gif
paste_gif.gif
pattern_gif.gif
print_gif.gif
quit_gif.gif
redo_gif.gif
reload_gif.gif
replace_gif.gif
save_gif.gif
saveall_gif.gif
saveas_gif.gif
saveto_gif.gif
search_gif.gif
searchicon_gif.gif
searchnext_gif.gif
searchprev_gif.gif
shiftleft_gif.gif
shiftright_gif.gif
small_gif.gif
styles_gif.gif
undo_gif.gif
uppercase.gif
)



add_executable(adie ${SOURCES})
target_link_libraries(adie FOX)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/icons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/icons.h
  COMMAND reswrap -h -o icons.h ${ICONS}
  COMMAND reswrap -i icons.h -o icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/help.cpp ${CMAKE_CURRENT_SOURCE_DIR}/help.h
  COMMAND reswrap -h -a -o help.h help.txt
  COMMAND reswrap -i help.h -a -o help.cpp help.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS reswrap
  VERBATIM
  )

install(TARGETS adie RUNTIME DESTINATION bin)
