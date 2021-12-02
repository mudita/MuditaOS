# Both these functions use the same tool - please mind that first function downloads single asset, whereas second one multiple ones

function(download_asset_release asset_name_in asset_name_out asset_repo asset_version)
    add_custom_command(OUTPUT ${asset_repo}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            "$<$<BOOL:$ENV{ASSETS_TOKEN}>:-t$ENV{ASSETS_TOKEN}>"
            github
            --owner mudita
            --repository ${asset_repo}
            --install_dir ${CMAKE_BINARY_DIR}
            download
            --name_in ${asset_name_in}
            --name_out ${asset_name_out}
            --product ${PRODUCT}
            --version ${asset_version}
        COMMENT "Downloading ${asset_name_out}"
    )

    add_custom_target(${asset_name_out}-target DEPENDS ${asset_repo})

    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/${asset_repo} DESTINATION "./"
        COMPONENTS Standalone Update)
endfunction()

function(download_asset_json json install_path cache_dir type)
    set(json ${CMAKE_SOURCE_DIR}/${json})
    add_custom_target(json-target
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            ${type}
            --install_dir ${install_path}
            --json ${json}
        COMMENT "Download binary assets listed in json file"
        )
endfunction()
