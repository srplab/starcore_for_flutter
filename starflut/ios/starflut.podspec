#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint starflut.podspec' to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'starflut'
  s.version          = '1.0.0'
  s.summary          = 'bridge for flutter interact with other programming languages'
  s.description      = <<-DESC
bridge for flutter interact with other programming languages, such as c/c++ lua, python, ruby, golang, rust, etc.
                       DESC
  s.homepage         = 'http://www.srplab.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'srplab' => 'srplab.cn@hotmail.com' }
  s.source           = { :path => '.' }

  s.source_files = 'Classes/**/*'
  #s.public_header_files = 'Classes/**/*.h'  #do not set this

  s.dependency 'Flutter'
  s.platform = :ios, '8.0'

  #basic config
  v_STARCORE_PATH = ENV['STARCORE_PATH']                             # '/Users/srplab/Desktop'

  v_STARCORE_PYTHONVERSION = ENV['STARCORE_PYTHONVERSION']           # '3.9'  '3.8'  3.7' '3.6'   '3.5'    '2.7'
  v_STARCORE_PYTHONCOREPATH = ENV['STARCORE_PYTHONCOREPATH']         # the folder of 'python3.6m' 'python3.6.zip' or 'python3.7m','pyton3.7.zip'...

  v_STARCORE_RUBYCOREPATH = ENV['STARCORE_RUBYCOREPATH']             # the folder of 'ruby 2.4'   'ruby 2.5',...

  v_STARCORE_GOLIBRARY = ENV['STARCORE_GOLIBRARY']                   # name of go static library, ex: if the name is libxxx.a, then here is 'xxx'. Important, the 'xxx_GetExportFunctionTable()' must exist.
  v_STARCORE_GOLIBRARYPATH = ENV['STARCORE_GOLIBRARYPATH']           # '/Users/srplab/Desktop/go.study'

  #other share library
  #for GetExportFunctionTable function, user this
  v_STARCORE_GETEXPORTFUNCTIONTABLENAME = ENV['STARCORE_GETEXPORTFUNCTIONTABLENAME']  # 'xxx_GetExportFunctionTable,yyy_GetExportFunctionTable'

  #or else, use this
  v_STARCORE_EXPORTFUNCTIONDEFINE = ENV['STARCORE_EXPORTFUNCTIONDEFINE'] # 'extern \"C\" void *xxx();extern \"C\" void *yyy();'
  v_STARCORE_EXPORTFUNCTIONNAME = ENV['STARCORE_EXPORTFUNCTIONNAME'] # 'xxx,yyy'

  #static library name and path
  v_STARCORE_STATICLIBRARY = ENV['STARCORE_STATICLIBRARY']           # name of static library, ex: if the name is libxxx.a, then here is 'xxx'
  v_STARCORE_STATICLIBRARYPATH = ENV['STARCORE_STATICLIBRARYPATH']   # 'the folder of the static library

  #framework name and path
  V_STARCORE_FRAMEWORK = ENV['STARCORE_FRAMEWORK']           # the name of framework, ex: 'xxx,yyy'
  V_STARCORE_FRAMEWORKPATH = ENV['STARCORE_FRAMEWORKPATH']   # 'the folder of the static framework

  if (V_STARCORE_FRAMEWORK != nil) && (V_STARCORE_FRAMEWORK.length != 0 ) then
    s.framework = V_STARCORE_FRAMEWORK
  end

  starcore_path =  v_STARCORE_PATH   # such as  '/Users/srplab/Desktop', must use absolute path

  current_path_ori = __FILE__
  current_path = current_path_ori[0..current_path_ori.rindex("/")-1]    # Dir::pwd

  starcore_moduleexport_define = ''
  starcore_moduleexport_call = ''

  if (v_STARCORE_EXPORTFUNCTIONDEFINE != nil) && (v_STARCORE_EXPORTFUNCTIONDEFINE.length != 0) then
      starcore_moduleexport_define = starcore_moduleexport_define + v_STARCORE_EXPORTFUNCTIONDEFINE
  end

  if (v_STARCORE_EXPORTFUNCTIONNAME != nil) && (v_STARCORE_EXPORTFUNCTIONNAME.length != 0) then
    v_STARCORE_EXPORTFUNCTIONNAME.split(',').each do |name|
      if( starcore_moduleexport_call.length == 0 ) then
          starcore_moduleexport_call = starcore_moduleexport_call + '(long)' + name;
      else
          starcore_moduleexport_call = starcore_moduleexport_call + ',(long)'+ name;
      end
    end
  end

  #function GETEXPORTFUNCTIONTABLE
  if (v_STARCORE_GETEXPORTFUNCTIONTABLENAME != nil) && (v_STARCORE_GETEXPORTFUNCTIONTABLENAME.length != 0) then
    v_STARCORE_GETEXPORTFUNCTIONTABLENAME.split(',').each do |name|
      starcore_moduleexport_define = starcore_moduleexport_define + 'extern\ \"C\"\ void\ *' + name + '();'
      if( starcore_moduleexport_call.length == 0 ) then
          starcore_moduleexport_call = starcore_moduleexport_call + '(long)' + name;
      else
          starcore_moduleexport_call = starcore_moduleexport_call + ',(long)'+ name;
      end
    end
  end

  if (v_STARCORE_GOLIBRARY == nil) then
    v_STARCORE_GOLIBRARY = 'star_go';
  end
  if (v_STARCORE_GOLIBRARYPATH != nil) && (v_STARCORE_GOLIBRARYPATH.length != 0) && (v_STARCORE_GOLIBRARY != nil) && (v_STARCORE_GOLIBRARY.length != 0) then
    starcore_moduleexport_define = starcore_moduleexport_define + 'extern\ \"C\"\ void\ *' + v_STARCORE_GOLIBRARY + '_GetExportFunctionTable();'
    if( starcore_moduleexport_call.length == 0 ) then
        starcore_moduleexport_call = starcore_moduleexport_call + '(long)' + v_STARCORE_GOLIBRARY + '_GetExportFunctionTable';
    else
        starcore_moduleexport_call = starcore_moduleexport_call + ',(long)' + v_STARCORE_GOLIBRARY + '_GetExportFunctionTable';
    end
  end

  compiler_flags = '-Wno-unused-function -DENV_IOS' + ' -DENV_MODULEEXPORT='+starcore_moduleexport_define + ' -DENV_MODULECALL='+starcore_moduleexport_call
  link_flags = 'iconv','stdc++','starcore'  # need linked with libstarcore.a
  header_flags = "$(inherited) " + current_path+"/Classes " + starcore_path+'/starcore_for_ios/include'
  library_flags = "$(inherited) " + starcore_path+'/starcore_for_ios'
  framework_flags = "$(inherited) "

  #python config
  if (v_STARCORE_PYTHONVERSION != nil) && (v_STARCORE_PYTHONVERSION.length != 0) && (v_STARCORE_PYTHONCOREPATH != nil) && (v_STARCORE_PYTHONCOREPATH.length != 0 ) then
    compiler_flags = compiler_flags + ' -DENV_WITHPYTHON=\"python'+v_STARCORE_PYTHONVERSION+'\"'    #support python  : python3.9,python3.8,python3.7,python3.6,python3.5,python2.7
    link_flags =  link_flags + ['sqlite3','ssl','crypto']
    if( v_STARCORE_PYTHONVERSION == "3.9" ) then
        link_flags =  link_flags + ['star_python39','python3.9']
        system 'ln -s -f '+v_STARCORE_PYTHONCOREPATH+'/python3.9.zip'+' '+current_path+'/python3.9.zip'
        s.resource = ['python3.9.zip']
    elsif( v_STARCORE_PYTHONVERSION == "3.8" ) then
        link_flags =  link_flags + ['star_python38','python3.8']
        system 'ln -s -f '+v_STARCORE_PYTHONCOREPATH+'/python3.8.zip'+' '+current_path+'/python3.8.zip'
        s.resource = ['python3.8.zip']
    elsif( v_STARCORE_PYTHONVERSION == "3.7" ) then
        link_flags =  link_flags + ['star_python37','python3.7m']
        system 'ln -s -f '+v_STARCORE_PYTHONCOREPATH+'/python3.7.zip'+' '+current_path+'/python3.7.zip'
        s.resource = ['python3.7.zip']
    elsif( v_STARCORE_PYTHONVERSION == "3.6" ) then
        link_flags =  link_flags + ['star_python36','python3.6m']
        system 'ln -s -f '+v_STARCORE_PYTHONCOREPATH+'/python3.6.zip'+' '+current_path+'/python3.6.zip'
        s.resource = ['python3.6.zip']
    elsif( v_STARCORE_PYTHONVERSION == "3.5" ) then
        link_flags =  link_flags + ['star_python35','python3.5m']
        system 'ln -s -f '+v_STARCORE_PYTHONCOREPATH+'/python3.5.zip'+' '+current_path+'/python3.5.zip'
        s.resource = ['python3.5.zip']
    elsif( v_STARCORE_PYTHONVERSION == "2.7" ) then
        link_flags =  link_flags + ['starpy','python2.7']
        system 'ln -s -f '+v_STARCORE_PYTHONCOREPATH+'/python2.7.zip'+' '+current_path+'/python2.7.zip'
        s.resource = ['python2.7.zip']
    end
    library_flags = library_flags + ' '+ v_STARCORE_PYTHONCOREPATH
  end

  #ruby config
  if (v_STARCORE_RUBYCOREPATH != nil) && (v_STARCORE_RUBYCOREPATH.length != 0) then
    compiler_flags = compiler_flags + ' -DENV_WITHRUBY'
    link_flags =  link_flags + ['star_ruby','ruby-exts','ruby-static','trans','sqlite3','ssl','crypto']
    library_flags = library_flags + ' '+ v_STARCORE_RUBYCOREPATH
  end

  #golang config
  if (v_STARCORE_GOLIBRARYPATH != nil) && (v_STARCORE_GOLIBRARYPATH.length != 0) && (v_STARCORE_GOLIBRARY != nil) && (v_STARCORE_GOLIBRARY.length != 0) then
    link_flags =  link_flags + [v_STARCORE_GOLIBRARY,'vsopenapi_c_stub']
    library_flags = library_flags + ' '+v_STARCORE_GOLIBRARYPATH
  end

  #other library config
  if (v_STARCORE_STATICLIBRARY != nil) && (v_STARCORE_STATICLIBRARY.length != 0) then
    v_STARCORE_STATICLIBRARY.split(',').each do |name|
      link_flags =  link_flags + [name]
    end
  end

  if (v_STARCORE_STATICLIBRARYPATH != nil) && (v_STARCORE_STATICLIBRARYPATH.length != 0 ) then
    v_STARCORE_STATICLIBRARYPATH.split(',').each do |name|
      library_flags = library_flags + ' '+ name;
    end
  end

  if (V_STARCORE_FRAMEWORKPATH != nil) && (V_STARCORE_FRAMEWORKPATH.length != 0 ) then
    V_STARCORE_FRAMEWORKPATH.split(',').each do |name|
      framework_flags = framework_flags + ' '+ name;
    end
  end

  s.compiler_flags = compiler_flags
  s.ios.library = link_flags
  s.xcconfig = {'HEADER_SEARCH_PATHS' => header_flags,'LIBRARY_SEARCH_PATHS' => library_flags, 'FRAMEWORK_SEARCH_PATHS' => framework_flags}

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386' }
  s.swift_version = '5.0'
end


