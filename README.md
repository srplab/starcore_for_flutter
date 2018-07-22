# starflut

A new flutter plugin project, which supports flutter to interact with other scripting languages such as python, java, ruby, golang, rust, etc. It is easy to us, supports android and ios platform.

starflut is based on "starcore-for-android" and "starfore-for-ios project". 

## First release at 2018/07/22

## Getting Started

For help getting started with Flutter, view offical site
[documentation](https://flutter.io/).

### 1. Get source code

```sh
 $ git clone https://github.com/srplab/starcore_for_flutter
```

- a. download "starcore_for_android"

```sh
 $ git clone https://github.com/srplab/starcore_for_android
```

or download from
[srplab：starcore_for_android.2.6.0.zip](http://www.srplab.com/data/starcore_for_android.2.6.0.zip).

- b. download "starcore_for_ios"

```sh
 $ git clone https://github.com/srplab/starcore_for_ios
```

or download from
[srplab：starcore_for_ios.2.6.0.tar.gz](http://www.srplab.com/data/starcore_for_ios.2.6.0.tar.gz).


### 2. How to use, with an example of calling python


- a. Create project


```sh
$ flutter create teststarflut
```

- b. Modify "pubspec.yaml", add "starflut" package

```
dev_dependencies:
  flutter_test:
    sdk: flutter

  starflut:
        path: ../starcore_for_flutter/starflut
```

- c. For android, add python share libraries as follow

```
android
  app
    src
      main
        assets
          unicodedata.cpython-36m.so
          zlib.cpython-36m.so
        java
        jniLibs
          arm64-v8a
            libpython3.6m.so
            libstar_python36.so
          armeabi 
            libpython3.6m.so
            libstar_python36.so
          armeabi-v7a
            libpython3.6m.so
            libstar_python36.so
          x86
            libpython3.6m.so
            libstar_python36.so
          x86_64
            libpython3.6m.so
            libstar_python36.so
        res
```
