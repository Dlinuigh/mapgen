project(
    'mapgen',
    'cpp',
    version : '3.0',
    default_options : ['warning_level=3', 'cpp_std=c++20'],
)
src = [
    'src/mapgen.cpp',
    'src/graphic.cpp',
    'src/view.cpp',
    'src/widget.cpp',
    'src/data.cpp',
    'src/program.cpp',
]
deps = [
    dependency('SDL3'),
    dependency('SDL3_image'),
    dependency('SDL3_ttf'),
    dependency('jsoncpp'),
]

exe = executable('mapgen', src, dependencies : deps, install : true, cpp_args : ['-I../include', '-std=c++2b'], link_args : ['-I../include'])

test('basic', exe)