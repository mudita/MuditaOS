# PureOS directories structure

## Audio

Audio assets are split into two directories. Later, they are split further into subdirectories.
Each subdirectory is exclusive to an application that uses it hence it is relatively easy to have specific assets tied
to a specific application.

### Internal partition
Internal partition stores proprietary assets that can't be accessed/modified by a user. Its structure and contents are created
at compile time. For technical info, please check the [CMake section](#CMake).

Structure template:
```
assets/audio/
        ├── app_1
        ├── app_2
        ├── app_3
        └── app_n
```

For instance, Harmony directory:
```
assets/audio/
        └── relaxation
```

Each product can have its structure freely defined. There are no restrictions applied. For instance, for the time being, there is no
proprietary assets path in Pure as it is not required. It is because the Pure MusicPlayer application only loads assets from the user partition.

### User partition
User partition stores assets that may be accessed by using the Mudita Center. In contrast to the [Internal partition](#Internal partition),
its permissions are set to write/read. It follows the same structure as the internal partition, i.e. each application has
its subdirectory used exclusively. Its structure is also created at compile time, and its contents can be propagated with assets or not,
it depends on the product. For technical info, please check the [CMake section](#CMake).

Structure template:
```
user/
└── audio
     ├── app_1
     ├── app_2
     ├── app_3
     └── app_n
```
For instance, Harmony directory:
```
user/
└── audio
     └── relaxation
```
and PurePhone:
```
user/
└── audio
     └── music_player
```

## Storage
It is the directory accessible via MTP. Its purpose is to provide space for storing user files.

```
user/
└── storage
```

## CMake

Script used to create directories:
[AddDirectories](../cmake/modules/AddDirectories.cmake)

There are two targets created by using the script above,
`user_directories_common` and `user_directories`. The first one creates common directories which are required for PureOS.
The latter is used to create required product-related directories. They are both added as a dependency to both `BellHybrid-disk-img` and
`PurePhone-disk-img` targets hence they will be invoked automatically upon generating the product image.


