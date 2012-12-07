#添加Qt类头文件在此命令中
SET(QHeader 
vMainWindow.h
vMultiView.h
vViewBase.h
vView2D.h
vView3D.h
vToolBarBase.h
vStatusBarNotify.h
vStatusBar.h
vSliderDelegate.h
vColorDelegate.h
vLogoToolBar.h
vTreeView.h
vMainToolBar.h)

QT4_WRAP_CPP(MOCSrcs ${QHeader})

AUX_SOURCE_DIRECTORY(. VIEWER_SOURCE_FILE)

add_definitions(-DV_FRAME_WORK_EXPORTS)

IF(WIN32)
ADD_LIBRARY(FrameWork SHARED
  ${VIEWER_SOURCE_FILE} 
  ${MOCSrcs})
ELSE(WIN32)
ADD_LIBRARY(FrameWork SHARED
  ${VIEWER_SOURCE_FILE} 
  ${MOCSrcs})
ENDIF(WIN32)


#添加对应的lib在这里
TARGET_LINK_LIBRARIES(FrameWork
  DataStorage  
  QVTK
  vtkRendering
  ${QT_LIBRARIES})

IF(WIN32)
INSTALL(TARGETS FrameWork RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
ELSE(WIN32)
INSTALL(TARGETS FrameWork LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})
ENDIF(WIN32)

