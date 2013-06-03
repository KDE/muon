set(firefoxappsbackend_SRCS
    FirefoxAppsResource.cpp
    FirefoxAppsBackend.cpp
#     FirefoxAppsReviewsBackend.cpp
#     FirefoxAppsTransaction.cpp
)

kde4_add_plugin(muon-firefoxappsbackend SHARED ${firefoxappsbackend_SRCS})
target_link_libraries(muon-firefoxappsbackend ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBRARY} muonprivate)

install(TARGETS muon-firefoxappsbackend DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES muon-firefoxapps-backend.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES firefoxapps-categories.xml DESTINATION ${DATA_INSTALL_DIR}/libmuon/categories)
