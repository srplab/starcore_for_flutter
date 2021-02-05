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
  s.source_files     = 'Classes/**/*'
  s.public_header_files = 'Classes/**/*.{h}'
  s.dependency 'FlutterMacOS'

  s.resources = ['starcore/*']
  s.libraries = 'starlib'
  s.compiler_flags = '-DENV_MACOS -DENV_M64'

  current_path_ori = __FILE__
  current_path = current_path_ori[0..current_path_ori.rindex("/")-1]    # Dir::pwd

  s.xcconfig = {'HEADER_SEARCH_PATHS' => current_path + '/include/starcore','LIBRARY_SEARCH_PATHS' => current_path}

  s.platform = :osx, '10.11'
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'
end
