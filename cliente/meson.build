inc_dir = include_directories(
    './modelo/include',
    './vista/include',
    './controlador/include'
)

src = ['cliente.c']

src_modelo = [
    './modelo/src/protocolo.c',
    './modelo/src/socket_util.c'
]

src_vista = [
    './vista/src/manejadores_vista.c'
]

src_controlador = [
    './controlador/src/parser.c',
    './controlador/src/controlador_notificaciones.c',
    './controlador/src/controlador_peticiones.c'
]

executable(
    'cliente',
    sources : [src, src_modelo, src_vista, src_controlador],
    dependencies : cjson_dep,
    include_directories: inc_dir,
    install : false
)
