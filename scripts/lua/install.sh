#!/usr/bin/env bash
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

function validate_product_selection() {
    case ${PRODUCT,,} in
        pure | purephone)
            PRODUCT_SHORTNAME="pure"
            PRODUCT="PurePhone"
            return 0 ;;
        bell | bellhybrid)
            PRODUCT_SHORTNAME="bell"
            PRODUCT="BellHybrid"
            return 0 ;;
        *)
            echo "Wrong product: \"${PRODUCT}\""
    esac
}

PRODUCT=$1
DESTINATION=$2

if validate_product_selection; then
    if [ -d ${DESTINATION} ]; then
        rm -Rf ${DESTINATION}
    fi
    parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
    cd "$parent_path"

    mkdir -p ${DESTINATION}
    cp -r products/${PRODUCT}/assets ${DESTINATION}/assets/
    cp -r share ${DESTINATION}/share
    cp -r migration/migration.lua ${DESTINATION}/share
    cp *.lua ${DESTINATION}

    if [ -n "$3" ]; then
        echo "Generating UDM update package"
        cp update_udm.lua ${DESTINATION}/update.lua
        cp products/${PRODUCT}/update_product.lua ${DESTINATION}
    fi
    rm ${DESTINATION}/update_udm.lua
    cd -
fi