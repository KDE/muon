set(cinstallbackend_SRCS
    CInstallBackend.cpp
    CInstallResource.cpp
)

kde4_add_plugin(muon-cinstallbackend SHARED ${cinstallbackend_SRCS})
target_link_libraries(muon-cinstallbackend muonprivate ${QT_QTCORE_LIBRARY}
                            ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS}
                            ${QJSON_LIBRARIES} ${BODEGA_LIBRARIES})

install(TARGETS muon-cinstallbackend DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES muon-cinstall-backend.desktop DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES cinstall-categories.xml DESTINATION ${DATA_INSTALL_DIR}/libmuon/categories)