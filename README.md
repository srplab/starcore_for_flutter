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

- c. For android, create sub folder under "main" and add python share libraries as follow

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

- d. For ios, modify file "starcore_for_flutter/starflut/ios/starflut.podspec"

1. set "starcore_path" to the folder of "starcore_for_ios"
2. if starcore native service is used, set "starcore_moduleexport_define" and "starcore_moduleexport_call", 
3. if python is used, uncomment "python config"
4. if ruby is used, uncomment "ruby config"
5. if golang module is used ,uncomment "golang config"

```
  #basic config
  starcore_path = '$(HOME)/Desktop'
  
  starcore_moduleexport_define = 'extern\ \"C\"\ void\ *star_go_GetExportFunctionTable();' 
        #begin
        #  export function define of starcore service module, for example 
        #
        #  starcore_moduleexport_define = 'extern\ \"C\"\ void\ *xxx_GetExportFunctionTable();extern\ \"C\"\ void\ *yyy_GetExportFunctionTable();'
        #=end

  starcore_moduleexport_call = 'star_go_GetExportFunctionTable();'
        #=begin
        #  export function call of starcore service module, for example 
        #
        #  starcore_moduleexport_call = 'xxx_GetExportFunctionTable();yyy_GetExportFunctionTable();'
        #=end

  compiler_flags = '-Wno-unused-function -DENV_IOS' + ' -DENV_MODULEEXPORT='+starcore_moduleexport_define + ' -DENV_MODULECALL='+starcore_moduleexport_call
  link_flags = 'iconv','stdc++','starcore'  # need linked with libstarcore.a
  header_flags = starcore_path+'/starcore_for_ios/include'
  library_flags = starcore_path+'/starcore_for_ios'

  #python config
  compiler_flags = compiler_flags + ' -DENV_WITHPYTHON=\"python3.6\"'    #support python  : python3.6,python3.5,python2.7
  link_flags =  link_flags + ['star_python36','python3.6m','sqlite3','ssl','crypto']
  library_flags = library_flags + ' '+starcore_path+'/starcore_for_ios/python.files/python-3.6'

  #ruby config
  compiler_flags = compiler_flags + ' -DENV_WITHRUBY'
  link_flags =  link_flags + ['star_ruby','ruby-exts','ruby-static','trans','sqlite3','ssl','crypto']
  library_flags = library_flags + ' '+starcore_path+'/starcore_for_ios/ruby.files/ruby-2.4'

  #golang config
  link_flags =  link_flags + ['star_go','vsopenapi_c_stub']
  library_flags = library_flags + ' '+starcore_path+'/go.study/stargo' + ' '+'/Users/srplab/Desktop/go.study/stargo/srplab/stargo/ios.static'

  s.compiler_flags = compiler_flags
  s.ios.library = link_flags
  s.xcconfig = {'HEADER_SEARCH_PATHS' => header_flags,'LIBRARY_SEARCH_PATHS' => library_flags }

```
