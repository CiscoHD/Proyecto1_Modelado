project('proyecto1_chat', ['c', 'cpp'])

unity_dep = dependency(
    'unity',
    fallback : ['unity', 'unity_dep']
)
cjson_dep = dependency(
    'libcjson',
    fallback: ['cjson', 'cjson_dep']
)

subdir('servidor')
subdir('cliente')
subdir('test')
