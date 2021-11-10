function(download_asset2 json install_path cache_dir)
    add_custom_target(json-target
        COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/download_asset2.py
            --json ${json}
            github
            --install_dir ${install_path}
            --cache_dir ${cache_dir}
        COMMENT "Download binary assets listed in json file"
        )
endfunction()
