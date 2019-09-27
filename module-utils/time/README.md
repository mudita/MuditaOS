Time conversion utility
=======================

# Why?

* There is no locale except C-Locale available in stdlib nano and we need time conversions.
* stdlibnano doesn't provide a way to sustitute C-Locale (it's static)

# How?

There is C-Locale builtin, therefore to convert time when it's possible it's used.
When it's not -> specifiers are substituted with our locale (i18 map)
