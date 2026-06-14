# git-lite

Sistema de control de versiones minimalista implementado en C,
inspirado en el funcionamiento interno de Git.

## Descripcion

git-lite es un sistema de control de versiones desarrollado desde cero
en C utilizando POSIX APIs. Permite gestionar versiones de archivos
mediante hashes SHA-256, garantizando integridad y trazabilidad de
los cambios realizados.

## Funcionalidades

- Agregar archivos al sistema de versionamiento
- Listar versiones registradas de un archivo
- Recuperar versiones anteriores de archivos
- Almacenamiento de versiones basado en hashes SHA-256
- Implementacion usando POSIX APIs

## Tecnologias

- Lenguaje: C
- APIs: POSIX
- Algoritmo de hashing: SHA-256
- Sistema operativo: Linux / Unix

## Como ejecutarlo

1. Clona el repositorio
   git clone https://github.com/SantiagoGutierrez11/reposirtyUniversity.git

2. Compila el proyecto
   gcc -o git-lite main.c -lssl -lcrypto

3. Ejecuta el programa
   ./git-lite

## Comandos disponibles

- add [archivo]: Agrega un archivo al sistema de versiones
- list [archivo]: Lista todas las versiones de un archivo
- get [archivo] [version]: Recupera una version especifica

## Autores

Santiago Gutierrez — Ingenieria de Sistemas, Universidad del Cauca
LinkedIn: https://www.linkedin.com/in/santiago-felipe-gutierrez-astaiza-710736416/

Cristian Campo — Ingenieria de Sistemas, Universidad del Cauca
