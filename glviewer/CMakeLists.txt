set(SOURCES FXGLCone.cpp
FXGLCone.h
FXGLCube.cpp
FXGLCube.h
FXGLCylinder.cpp
FXGLCylinder.h
FXGLLine.cpp
FXGLLine.h
FXGLPoint.cpp
FXGLPoint.h
FXGLShape.cpp
FXGLShape.h
FXGLSphere.cpp
FXGLSphere.h
FXGLTriangleMesh.cpp
FXGLTriangleMesh.h
glviewer.cpp
)

add_executable(glviewer ${SOURCES})
target_link_libraries(glviewer FOX)

