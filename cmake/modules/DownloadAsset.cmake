function(download_asset asset_name_in asset_name_out asset_repo asset_version)
    add_custom_command(OUTPUT ${asset_repo}
        COMMAND ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            "$<$<BOOL:$ENV{ASSETS_TOKEN}>:-t$ENV{ASSETS_TOKEN}>"
            --workdir ${CMAKE_BINARY_DIR}
            --repository ${asset_repo}
            download
            --product ${PRODUCT}
            --version ${asset_version}
            --assetRepoName ${asset_name_in}
            --assetOutName ${asset_name_out}
        COMMENT "Downloading updater.bin"
    )

    add_custom_target(${asset_name_out}-target DEPENDS ${asset_repo})

    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/${asset_repo} DESTINATION "./"
        COMPONENTS Standalone Update)
endfunction()
