project('mapgen', 'cpp', version:'1.0', default_options : ['warning_level=3',
                     'cpp_std=c++20'])
src = [
  'mapgen.cpp',
  'graphic.cpp'
]
deps = [
  dependency('SDL3'),
  dependency('SDL3_image'),
  dependency('SDL3_ttf'),
  dependency('jsoncpp')
]

exe = executable('mapgen', src, dependencies: deps, install:true)

test('basic', exe)
