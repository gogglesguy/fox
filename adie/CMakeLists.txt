set(SOURCES Adie.cpp
Commands.cpp
Commands.h
FindInFiles.cpp
FindInFiles.h
${CMAKE_CURRENT_BINARY_DIR}/help.cpp
${CMAKE_CURRENT_BINARY_DIR}/help.h
HelpWindow.cpp
${CMAKE_CURRENT_BINARY_DIR}/icons.cpp
${CMAKE_CURRENT_BINARY_DIR}/icons.h
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
bookclr2_gif.gif
bookclr_gif.gif
bookdel2_gif.gif
bookdel_gif.gif
bookmrk2_gif.gif
bookmrk_gif.gif
booknxt2_gif.gif
booknxt_gif.gif
bookprv2_gif.gif
bookprv_gif.gif
bookset2_gif.gif
bookset_gif.gif
browser.gif
capitalize.gif
close_gif.gif
colors_gif.gif
command_bmp.bmp
config_gif.gif
copy_gif.gif
copy2_gif.gif
cut_gif.gif
cut2_gif.gif
delete_gif.gif
delimit_gif.gif
docs_gif.gif
export_gif.gif
express_gif.gif
filehidden_gif.gif
fileshown_gif.gif
filter_gif.gif
fonts_gif.gif
forward_gif.gif
goto_gif.gif
help_gif.gif
import_gif.gif
indent_gif.gif
info_gif.gif
lang_gif.gif
logger.gif
lowercase.gif
miscellaneous_gif.gif
new_gif.gif
newfile_gif.gif
nobrowser.gif
nologger.gif
nowrap_gif.gif
open_gif.gif
opensel_gif.gif
palette_gif.gif
paste_gif.gif
pattern_gif.gif
print_gif.gif
point2_gif.gif
quit_gif.gif
redo_gif.gif
reload_gif.gif
rename_gif.gif
replace_gif.gif
revert_gif.gif
run_gif.gif
save_gif.gif
saveall_gif.gif
saveas_gif.gif
saveto_gif.gif
script_gif.gif
search_case.gif
search_dn.gif
search_gif.gif
search_nocase.gif
search_norex.gif
search_noword.gif
search_noword2.gif
search_rex.gif
search_up.gif
search_word.gif
search_word2.gif
searchfiles_gif.gif
searchicon_gif.gif
searchnext_gif.gif
searchprev_gif.gif
shiftleft_gif.gif
shiftright_gif.gif
small_gif.gif
styles_gif.gif
switch_gif.gif
syntax_gif.gif
terminal_gif.gif
undo_gif.gif
uppercase.gif
wordwrap_gif.gif
)



add_executable(adie ${SOURCES})
target_link_libraries(adie PRIVATE FX::FOX FX::FOX_XINCS)

# Icon and help file generation
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${CMAKE_CURRENT_BINARY_DIR}/icons.h
  COMMAND ${RESWRAP_CMD} -h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.h ${ICONS}
  COMMAND ${RESWRAP_CMD} -i ${CMAKE_CURRENT_BINARY_DIR}/icons.h -o ${CMAKE_CURRENT_BINARY_DIR}/icons.cpp ${ICONS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${RESWRAP_DEPENDS} ${ICONS}
  VERBATIM
  )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/help.cpp ${CMAKE_CURRENT_BINARY_DIR}/help.h
  COMMAND ${RESWRAP_CMD} -h -a -o ${CMAKE_CURRENT_BINARY_DIR}/help.h help.txt
  COMMAND ${RESWRAP_CMD} -i ${CMAKE_CURRENT_BINARY_DIR}/help.h -a -o ${CMAKE_CURRENT_BINARY_DIR}/help.cpp help.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS ${RESWRAP_DEPENDS} help.txt
  VERBATIM
  )

install(TARGETS adie RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Applications)
