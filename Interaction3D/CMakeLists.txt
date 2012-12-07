
#添加Qt类头文件在此命令中

SET(QHeader 
vInteraction3D.h
vTransFunction.h
)

AUX_SOURCE_DIRECTORY(. VIEWER_SOURCE_FILE)

#生成Qt的moc文件

QT4_WRAP_CPP(MOCSrcs ${QHeader})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/tool)

add_definitions(-DMARKUP_STL)
add_definitions(-DV_INTERACTION3D_EXPORTS)

ADD_LIBRARY(Interaction3D SHARED
  ${VIEWER_SOURCE_FILE} 
  ${MOCSrcs})

#添加对应的lib在这里
TARGET_LINK_LIBRARIES(Interaction3D
  DataStorage  
  FrameWork
  QVTK
  vtkVolumeRendering
  vtkRendering
  vtkImaging
  ${QT_LIBRARIES})

IF(WIN32)
INSTALL(TARGETS Interaction3D RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/tool)
ELSE(WIN32)
INSTALL(TARGETS Interaction3D LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/tool)
ENDIF(WIN32)