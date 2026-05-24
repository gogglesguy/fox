find_package(OpenGL)

add_executable(bitmapviewer bitmapviewer.cpp)
target_link_libraries(bitmapviewer FX::FOX)

add_executable(button button.cpp)
target_link_libraries(button FX::FOX)

add_executable(calendar calendar.cpp)
target_link_libraries(calendar FX::FOX)

add_executable(codecs codecs.cpp)
target_link_libraries(codecs FX::FOX)

add_executable(console console.cpp)
target_link_libraries(console FX::FOX)

add_executable(datatarget datatarget.cpp)
target_link_libraries(datatarget FX::FOX)

add_executable(dctest dctest.cpp)
target_link_libraries(dctest FX::FOX)

add_executable(dialog dialog.cpp)
target_link_libraries(dialog FX::FOX)

add_executable(dictest dictest.cpp)
target_link_libraries(dictest FX::FOX)

add_executable(dirlist dirlist.cpp)
target_link_libraries(dirlist FX::FOX)

add_executable(expression expression.cpp)
target_link_libraries(expression FX::FOX)

add_executable(format format.cpp)
target_link_libraries(format FX::FOX)

add_executable(foursplit foursplit.cpp)
target_link_libraries(foursplit FX::FOX)

add_executable(gaugetest gaugetest.cpp)
target_link_libraries(gaugetest FX::FOX)

add_executable(gltest gltest.cpp)
target_link_libraries(gltest FX::FOX ${OPENGL_LIBRARIES})

add_executable(groupbox groupbox.cpp)
target_link_libraries(groupbox FX::FOX)

add_executable(half half.cpp)
target_link_libraries(half FX::FOX)

add_executable(header header.cpp)
target_link_libraries(header FX::FOX)

add_executable(hello hello.cpp)
target_link_libraries(hello FX::FOX)

add_executable(hello2 hello2.cpp)
target_link_libraries(hello2 FX::FOX)

add_executable(iconlist iconlist.cpp)
target_link_libraries(iconlist FX::FOX)

add_executable(image image.cpp)
target_link_libraries(image FX::FOX)

add_executable(imageviewer imageviewer.cpp)
target_link_libraries(imageviewer FX::FOX)

add_executable(layout layout.cpp)
target_link_libraries(layout FX::FOX)

add_executable(match match.cpp)
target_link_libraries(match FX::FOX)

add_executable(math math.cpp)
target_link_libraries(math FX::FOX)

add_executable(mditest mditest.cpp)
target_link_libraries(mditest FX::FOX)

add_executable(memmap memmap.cpp)
target_link_libraries(memmap FX::FOX)

add_executable(minheritance minheritance.cpp)
target_link_libraries(minheritance FX::FOX)

add_executable(parallel parallel.cpp)
target_link_libraries(parallel FX::FOX)

add_executable(process process.cpp)
target_link_libraries(process FX::FOX)

add_executable(ratio ratio.cpp)
target_link_libraries(ratio FX::FOX)

add_executable(rex rex.cpp)
target_link_libraries(rex FX::FOX)

add_executable(scan scan.cpp)
target_link_libraries(scan FX::FOX)

add_executable(scribble scribble.cpp)
target_link_libraries(scribble FX::FOX)

add_executable(shutter shutter.cpp)
target_link_libraries(shutter FX::FOX)

add_executable(splitter splitter.cpp)
target_link_libraries(splitter FX::FOX)

add_executable(switcher switcher.cpp)
target_link_libraries(switcher FX::FOX)

add_executable(tabbook tabbook.cpp)
target_link_libraries(tabbook FX::FOX)

add_executable(table table.cpp)
target_link_libraries(table FX::FOX)

add_executable(thread thread.cpp)
target_link_libraries(thread FX::FOX)

add_executable(timefmt timefmt.cpp)
target_link_libraries(timefmt FX::FOX)

add_executable(unicode unicode.cpp)
target_link_libraries(unicode FX::FOX)

add_executable(variant variant.cpp)
target_link_libraries(variant FX::FOX)

add_executable(wizard wizard.cpp)
target_link_libraries(wizard FX::FOX)

add_executable(xml xml.cpp)
target_link_libraries(xml FX::FOX)

add_executable(units units.cpp)
target_link_libraries(units FX::FOX)


