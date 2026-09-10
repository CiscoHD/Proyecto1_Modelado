Modelado y programación
========================

Chat
------

Se desarrolla un chat haciendo uso del pacentralizando la comunicación
en un servidor que maneja todo lo relativo a la comunicación entre los
multiples usuarios (clientes).

El servidor se desarrolla en el lenguaje de programción C, mientras
que el se desarrolla en C++. 

# Construcción del proyecto

Se usa Meson como sistema de construcción. 

## Compilación.

Para compilar, dentro de la raiz del proyecto ejecuta: 

```
$ meson setup build
$ meson compile -C build
```

## Ejecutar pruebas unitarias. 

Dentro de la raiz del proyeto ejecuta: 

```
$ meson test -C build
```

# Servidor

# Cliente
