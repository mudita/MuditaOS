set(PRODUCT "PurePhone" CACHE STRING "The product to be configured and built.")

macro(validate_product_selection)
    # create list of valid products
    file(GLOB _PRODUCTS_CANDIDATES  "${CMAKE_SOURCE_DIR}/products/*")
    set(PRODUCTS "")
    foreach(_P in ${_PRODUCTS_CANDIDATES})
        if(IS_DIRECTORY ${_P})
            get_filename_component(_DIRNAME ${_P} NAME)
            list(APPEND PRODUCTS ${_DIRNAME})
        endif()
    endforeach()
    list(JOIN PRODUCTS ", " PRODUCTS_STRING)

    if("${PRODUCT}" STREQUAL "")
        message(FATAL_ERROR "Product not selected. Valid products: ${PRODUCTS_STRING}")
    endif()

    if(NOT ${PRODUCT} IN_LIST PRODUCTS)
        message(FATAL_ERROR "${PRODUCT} is not recognized as a valid product name. Valid products: ${PRODUCTS_STRING}")
    endif()
endmacro()
