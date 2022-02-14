# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#!/bin/sh

CHECK_LANG="$1"
if [ -z "${MY_URDSFDFS+1}" ]; then
  CHECK_LANG="Cesky"
fi

MASTER_LANG="English"
PROFILE_PATH=image/assets/profiles
LANG_PATH=image/assets/lang

json_key_names () {
  grep -E "\"[^\"]+\"\:" $1 | sed -e "s|\s\+\(\"[^\"]\+\"\)\:.\+|\1|"
}

print_missing()
for key in $(json_key_names "$1")
do
  count=`grep -c "$key" $2`
  if [ $count = "0" ]; then
  echo "$key"
  fi
done

echo "missing strings in: ${LANG_PATH}/${CHECK_LANG}.json"
print_missing "${LANG_PATH}/${MASTER_LANG}.json" "${LANG_PATH}/${CHECK_LANG}.json"

echo "missing strings in: ${PROFILE_PATH}/${CHECK_LANG}_upper.json"
print_missing "${PROFILE_PATH}/${MASTER_LANG}_upper.json" "${PROFILE_PATH}/${CHECK_LANG}_upper.json"

echo "missing strings in: ${PROFILE_PATH}/${CHECK_LANG}_lower.json"
print_missing "${PROFILE_PATH}/${MASTER_LANG}_lower.json" "${PROFILE_PATH}/${CHECK_LANG}_lower.json"
