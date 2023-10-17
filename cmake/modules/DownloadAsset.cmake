# Both these functions use the same tool - please mind that first function downloads single asset, whereas second one multiple ones

function(download_asset_release asset_name_in asset_name_out asset_repo asset_version cache_dir)
    add_custom_command(OUTPUT ${asset_repo}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            --cache_dir ${cache_dir}
            github
            --owner mudita
            --repository ${asset_repo}
            --install_dir ${SYSROOT_BIN_PATH}
            download
            --name_in ${asset_name_in}
            --name_out ${asset_name_out}
            --product ${PRODUCT}
            --version ${asset_version}
            COMMENT "Downloading ${asset_name_out} to install dir: ${CMAKE_BINARY_DIR}"
    )

    add_custom_target(${asset_name_out}-target DEPENDS ${asset_repo})

#    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/${asset_repo} DESTINATION "./" COMPONENTS Standalone Update)
endfunction()

function(download_asset_release_json
        target
        json
        install_path
        asset_repo
        version
        cache_dir
        )
    add_custom_target(
        ${target}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            --cache_dir ${cache_dir}
            github
            --owner mudita
            --repository ${asset_repo}
            --install_dir ${install_path}
            download
            --json ${json}
            --version ${version}
            --product ${PRODUCT}
            COMMENT "Downloading ${target} from ${json} for ${PRODUCT} to install dir: ${install_path}"
    )
endfunction()

function(download_asset_json
        target
        json
        install_path
        repository
        cache_dir)
    add_custom_target(${target}
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            --cache_dir ${cache_dir}
            github
            --owner mudita
            --repository ${repository}
            --install_dir ${install_path}
            json
            --json ${json}
        COMMENT "Download ${target} binary assets listed in ${json} for ${PRODUCT} to install dir: ${install_path}"
        )
endfunction()
