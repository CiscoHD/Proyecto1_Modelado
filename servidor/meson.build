inc_dir = include_directories('./include')

src = ['./src/servidor.c', './src/socket_util.c']

executable('servidor',
    sources : src,
    dependencies : [cjson_dep, glib_dep],
    include_directories: inc_dir,
    install : false
)

