# Install script for directory: D:/SDK/MyGui/MyGUIEngine

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "D:/SDK/MyGui/sdk")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Release" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/SDK/MyGui/lib/Release/MyGUIEngine.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Release" TYPE SHARED_LIBRARY FILES "D:/SDK/MyGui/bin/Release/MyGUIEngine.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/RelWithDebInfo" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/SDK/MyGui/lib/RelWithDebInfo/MyGUIEngine.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/RelWithDebInfo" TYPE SHARED_LIBRARY FILES "D:/SDK/MyGui/bin/RelWithDebInfo/MyGUIEngine.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/MinSizeRel" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/SDK/MyGui/lib/MinSizeRel/MyGUIEngine.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/MinSizeRel" TYPE SHARED_LIBRARY FILES "D:/SDK/MyGui/bin/MinSizeRel/MyGUIEngine.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/Debug" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/SDK/MyGui/lib/Debug/MyGUIEngine_d.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Debug" TYPE SHARED_LIBRARY FILES "D:/SDK/MyGui/bin/Debug/MyGUIEngine_d.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/Debug" TYPE FILE FILES "D:/SDK/MyGui/bin/Debug/MyGUIEngine_d.pdb")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/RelWithDebInfo" TYPE FILE FILES "D:/SDK/MyGui/bin/RelWithDebInfo/MyGUIEngine.pdb")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/MYGUI" TYPE FILE FILES
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Allocator.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_CustomAllocator.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DDItemInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IBItemInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MenuItemType.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MessageStyle.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_WidgetToolTip.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Button.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Canvas.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ComboBox.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DDContainer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Edit.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_HScroll.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ItemBox.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_List.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ListBox.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ListCtrl.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MenuBar.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MenuCtrl.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MenuItem.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Message.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MultiList.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_PopupMenu.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Progress.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ScrollView.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_StaticImage.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_StaticText.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Tab.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TabItem.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_VScroll.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Widget.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Window.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Bitwise.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_CoordConverter.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DataFileStream.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DataStream.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Delegate.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DelegateImplement.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DynLib.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Enumerator.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_EventPair.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TextChangeHistory.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TextIterator.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TextureUtility.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TextView.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Timer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_UString.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Utility.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_WidgetTranslate.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_XmlDocument.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ConsoleLogListener.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_FileLogListener.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ILogFilter.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ILogListener.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LevelLogFilter.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LogLevel.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LogManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LogSource.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LogStream.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Common.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Diagnostic.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Exception.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_GenericFactory.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Macros.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Platform.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Precompiled.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Prerequest.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_RenderFormat.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_WidgetDefines.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_BackwardCompatibility.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DeprecatedTypes.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ICroppedRectangle.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IDataStream.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IObject.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ISerializable.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ITexture.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IUnlinkWidget.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IVertexBuffer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Plugin.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_RTTI.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Singleton.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_BiIndexBase.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ScrollViewBase.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_WidgetInput.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_WidgetUserData.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_EditText.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MainSkin.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_PolygonalSkin.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_RawRect.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_RotatingSkin.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SimpleText.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SubSkin.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TileRect.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IStateInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ISubWidget.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ISubWidgetRect.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ISubWidgetText.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LayerItem.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LayerNode.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_OverlappedLayer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_RenderItem.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SharedLayer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SharedLayerNode.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SkinItem.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ILayer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ILayerItem.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ILayerNode.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IRenderTarget.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ActionController.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ControllerEdgeHide.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ControllerFadeAlpha.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ControllerItem.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ControllerPosition.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ResourceImageSet.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ResourceImageSetPointer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ResourceLayout.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ResourceManualFont.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ResourceManualPointer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ResourceSkin.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ResourceTrueTypeFont.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IFont.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IPointer.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_IResource.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ChildSkinInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_FontData.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LayoutData.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MaskPickInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SubWidgetBinding.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SubWidgetInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Align.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Any.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Colour.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_CommonStateInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_FlowDirection.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ImageInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_KeyCode.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_MouseButton.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_RenderTargetInfo.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TCoord.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TPoint.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TRect.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_TSize.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Types.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Version.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_VertexData.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_WidgetStyle.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ClipboardManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ControllerManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DataManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_DynLibManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_FactoryManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_FontManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_Gui.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_InputManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LanguageManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LayerManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_LayoutManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_PluginManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_PointerManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_RenderManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ResourceManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SkinManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_SubWidgetManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_ToolTipManager.h"
    "D:/SDK/MyGui/MyGUIEngine/include/MyGUI_WidgetManager.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

