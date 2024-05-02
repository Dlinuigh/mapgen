project('mapgen', 'cpp', version:'1.0', default_options : ['warning_level=3',
                     'cpp_std=c++20'])
src = [
  'mapgen.cpp',
  'graphic.cpp'
]
deps = [
  dependency('sdl3'),
  dependency('sdl3-image'),
  dependency('sdl3-ttf'),
  dependency('RapidJSON')
]

exe = executable('mapgen', src, dependencies: deps, install:true)

test('basic', exe)
