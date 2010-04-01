SET ( EXTENSION_NAME "Extensions_PostProcess")

# Create the extension library
ADD_LIBRARY( ${EXTENSION_NAME}
  Scene/PostProcessNode.h
  Scene/PostProcessNode.cpp
  Renderers/OpenGL/PostProcessRenderingView.h
  Renderers/OpenGL/PostProcessRenderingView.cpp
)

TARGET_LINK_LIBRARIES( ${EXTENSION_NAME}
  OpenEngine_Renderers
  OpenEngine_Display
  Extensions_OpenGLRenderer
  ${OPENGL_LIBRARY}
)
