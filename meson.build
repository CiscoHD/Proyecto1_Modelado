project('proyecto1_chat', ['c'])

unity_dep = dependency(
    'unity',
    fallback : ['unity', 'unity_dep']
)

cjson_dep = dependency(
    'libcjson',
    fallback: ['cjson', 'cjson_dep']
)

glib_dep = dependency(
    'glib-2.0'
)

ncurses_dep = dependency(
    'ncurses'
)

sds_dep = dependency(
    'sds',
    fallback: ['sds', 'sds_dep']
)

subdir('servidor')
subdir('cliente')

