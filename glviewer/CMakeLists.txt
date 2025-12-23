if(WITH_OPENGL)

  find_package(OpenGL REQUIRED COMPONENTS OpenGL)
  if (NOT TARGET OpenGL::GLU)
    message(FATAL_ERROR "OpenGL Utility Library not found")
  endif()

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
    target_link_libraries(glviewer FX::FOX OpenGL::GLU)
endif()
