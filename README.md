Modelado y programación
========================

Chat
------

Se desarrolla un chat centralizando la comunicación en un servidor 
que maneja todo lo relativo a la interacción entre los multiples 
usuarios (clientes).

El servidor se desarrolla en el lenguaje de programación C, mientras
que el cliente se desarrolla en C++. 

# Construcción del proyecto

Se usa Meson como sistema de construcción. 

## Compilación.

Para compilar, dentro de la raíz del proyecto ejecuta: 

```
$ meson setup build
$ meson compile -C build
```

## Ejecutar pruebas unitarias. 

Dentro de la raíz del proyecto ejecuta: 

```
$ meson test -C build
```

# Servidor

Para levantar el servidor, una vez compilado el proyecto, dentro de la
raíz del mismo ejecuta: 

```
$ ./build/servidor/servidor <número_de_puerto>
```

# Cliente

Para levantar un cliente, una vez compilado el proyecto, dentro de la
raíz del mismo ejecuta: 

```
$ ./build/cliente/cliente 
```
