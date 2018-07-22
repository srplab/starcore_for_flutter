#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html
#
Pod::Spec.new do |s|
  s.name             = 'starflut'
  s.version          = '0.5.0'
  s.summary          = 'bridge for flutter interact with other programming languages'
  s.description      = <<-DESC
bridge for flutter interact with other programming languages, such as c/c++ lua, python, ruby, golang, rust, etc.
                       DESC
  s.homepage         = 'http://www.srplab.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'srplab' => 'srplab.cn@hotmail.com' }
  s.source           = { :path => '.' }

  s.source_files = 'Classes/**/*.{m,mm}'

  s.public_header_files = 'Classes/**/*.{h}'
  #s.header_mappings_dir = './Classes'

  s.dependency 'Flutter'

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
  
  s.ios.deployment_target = '8.0'
end

