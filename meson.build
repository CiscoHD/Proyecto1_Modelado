project('proyecto1_chat', ['c', 'cpp'])

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

subdir('servidor')
subdir('cliente')
subdir('test')
