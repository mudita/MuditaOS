Time conversion utility
=======================

# Why?

* There is no locale except C-Locale available in stdlib nano and we need time conversions.
* stdlibnano doesn't provide a way to sustitute C-Locale (it's static)

# How?

There is C-Locale builtin, therefore  it's used to convert time whenever possible.  
When it's not -> specifiers are substituted with our locale (i18 map)  

```
    +------------+     +--------+
    |  Timestamp | <=> | Locale |
    +------------+     +--------+
          *
          |
    +------------+
    |  DateTime  |
    +------------+
        *     *
       /       \
      /         \
 +------+     +------+
 | Time |     | Date |
 +------+     +------+

 ```

Locale  - (`utils::time::Locale`) class for i18n Timestamp internationalization management  
Timestamp - general class to store time data. Init with a EPOCH timestamp. Has input and output formatters.    
DateTime - class for most of possible usecases, initialized with system clock, can return time/date string in reference to past
Date - just a subclass with a default formatter returning the date in particular
Time - just a subclass with a default formatter returning the time, regardless of the date (today/yesterday/5 days ago)

For more see headers
