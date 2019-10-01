Time conversion utility
=======================

# Why?

* There is no locale except C-Locale available in stdlib nano and we need time conversions.
* stdlibnano doesn't provide a way to sustitute C-Locale (it's static)

# How?

There is C-Locale builtin, therefore to convert time when it's possible it's used.  
When it's not -> specifiers are substituted with our locale (i18 map)  

```
 +--------+     +--------+
 |  Time  | <=> | Locale |
 +--------+     +--------+
    *
    |
 +-----------+
 |  SysTime  |
 +-----------+
 ```

Locale  - ( utils::time::Locale) class for i18n Time internationalization management  
Time    - generall class for Time transformations  
SysTime - class for most of possible usecases, initialized with system clock, can return time string in reference to past  


For more see headers
