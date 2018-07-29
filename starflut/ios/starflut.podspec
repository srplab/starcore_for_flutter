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
  v_STARCORE_PATH = ENV['STARCORE_PATH']                             # '/Users/srplab/Desktop'
  v_STARCORE_PYTHONVERSION = ENV['STARCORE_PYTHONVERSION']           # '3.6'   '3.5'    '2.7'
  v_STARCORE_PYTHONLIBRARY = ENV['STARCORE_PYTHONLIBRARY']           # 'star_python36,python3.6m'
  v_STARCORE_RUBYVERSION = ENV['STARCORE_RUBYVERSION']               # '2.4'   '2.5
  v_STARCORE_GOLIBRARYPATH = ENV['STARCORE_GOLIBRARYPATH']           # '/Users/srplab/Desktop/go.study'
  v_STARCORE_EXPORTDEFINE = ENV['STARCORE_EXPORTDEFINE']             # 'extern\ \"C\"\ void\ *star_go_GetExportFunctionTable();'
  v_STARCORE_EXPORTFUNCTION = ENV['STARCORE_EXPORTFUNCTION']         # 'star_go_GetExportFunctionTable();'
  v_STARCORE_EXPORTLIBRARY = ENV['STARCORE_EXPORTLIBRARY']           # 'star_go,vsopenapi_c_stub'
  v_STARCORE_EXPORTLIBRARYPATH = ENV['STARCORE_EXPORTLIBRARYPATH']   # '/Users/srplab/Desktop/go.study'


  starcore_path =  v_STARCORE_PATH   # such as  '$(HOME)/Desktop'

  if v_STARCORE_EXPORTDEFINE == nil then
    v_STARCORE_EXPORTDEFINE = ''
  end
  starcore_moduleexport_define = v_STARCORE_EXPORTDEFINE
  if (v_STARCORE_GOLIBRARYPATH != nil) && (v_STARCORE_GOLIBRARYPATH.length != 0) then
    starcore_moduleexport_define = starcore_moduleexport_define + 'extern\ \"C\"\ void\ *star_go_GetExportFunctionTable();'
  end

        #begin
        #  export function define of starcore service module, for example 
        #
        #  starcore_moduleexport_define = 'extern\ \"C\"\ void\ *xxx_GetExportFunctionTable();extern\ \"C\"\ void\ *yyy_GetExportFunctionTable();'
        #=end

  if v_STARCORE_EXPORTFUNCTION == nil then
    v_STARCORE_EXPORTFUNCTION = ''
  end
  starcore_moduleexport_call = v_STARCORE_EXPORTFUNCTION
  if (v_STARCORE_GOLIBRARYPATH != nil) && (v_STARCORE_GOLIBRARYPATH.length != 0) then
    starcore_moduleexport_call = starcore_moduleexport_call + 'star_go_GetExportFunctionTable();'
  end

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
  if (v_STARCORE_PYTHONVERSION != nil) && (v_STARCORE_PYTHONVERSION.length != 0) && (v_STARCORE_PYTHONLIBRARY != nil) && (v_STARCORE_PYTHONLIBRARY.length != 0) then
    compiler_flags = compiler_flags + ' -DENV_WITHPYTHON=\"python'+v_STARCORE_PYTHONVERSION+'\"'    #support python  : python3.6,python3.5,python2.7
    link_flags =  link_flags + v_STARCORE_PYTHONLIBRARY.split(',') + ['sqlite3','ssl','crypto']   # 'star_python36','python3.6m'
    library_flags = library_flags + ' '+starcore_path+'/starcore_for_ios/python.files/python-'+v_STARCORE_PYTHONVERSION
  end

  #ruby config
  if (v_STARCORE_RUBYVERSION != nil) && (v_STARCORE_RUBYVERSION.length != 0) then
    compiler_flags = compiler_flags + ' -DENV_WITHRUBY'
    link_flags =  link_flags + ['star_ruby','ruby-exts','ruby-static','trans','sqlite3','ssl','crypto']
    library_flags = library_flags + ' '+starcore_path+'/starcore_for_ios/ruby.files/ruby-'+v_STARCORE_RUBYVERSION   # 2.4, 2.5, etc
  end

  #golang config
  if (v_STARCORE_GOLIBRARYPATH != nil) && (v_STARCORE_GOLIBRARYPATH.length != 0) then
    link_flags =  link_flags + ['star_go','vsopenapi_c_stub']
    library_flags = library_flags + ' '+v_STARCORE_GOLIBRARYPATH
  end

  #other library config
  if (v_STARCORE_EXPORTLIBRARY != nil) && (v_STARCORE_EXPORTLIBRARY.length != 0) then
    link_flags =  link_flags + v_STARCORE_EXPORTLIBRARY.split(',')
  end
  if (v_STARCORE_EXPORTLIBRARYPATH != nil) && (v_STARCORE_EXPORTLIBRARYPATH.length != 0 ) then
    library_flags = library_flags + ' '+v_STARCORE_EXPORTLIBRARYPATH
  end

  s.compiler_flags = compiler_flags
  s.ios.library = link_flags
  s.xcconfig = {'HEADER_SEARCH_PATHS' => header_flags,'LIBRARY_SEARCH_PATHS' => library_flags }
  
  s.ios.deployment_target = '8.0'
end

