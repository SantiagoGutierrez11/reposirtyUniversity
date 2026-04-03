/**
 * @file
 * @brief Implementacion del API de gestion de versiones
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
*/

#include "versions.h"

/**
 * @brief Crea una version en memoria del archivo
 * Valida si el archivo especificado existe y crea su hash
 * @param filename Nombre del archivo
 * @param hash Hash del contenido del archivo
 * @param comment Comentario
 * @param result Nueva version en memoria
 *
 * @return Resultado de la operacion
 */
return_code create_version(char * filename, char * comment, file_version * result);
/**
 * @brief Verifica si existe una version para un archivo
 *
 * @param filename Nombre del archivo
 * @param hash Hash del contenido
 *
 * @return 1 si la version existe, 0 en caso contrario.
 */
int version_exists(char * filename, char * hash);
/**
 * @brief Obtiene el hash de un archivo.
 * @param filename Nombre del archivo a obtener el hash
 * @param hash Buffer para almacenar el hash (HASH_SIZE)
 * @return Referencia al buffer, NULL si ocurre error
 */
char *get_file_hash(char * filename, char * hash);
/**
 * @brief Copia un archivo
 *
 * @param source Archivo fuente
 * @param destination Destino
 *
 * @return 1 en caso de exito, 0 en caso de error
 */
int copy(char * source, char * destination);

/**
* @brief Almacena un archivo en el repositorio
*
* @param filename Nombre del archivo
* @param hash Hash del archivo: nombre del archivo en el repositorio
*
* @return 1 en caso de exito, 0 en caso de error
*/
int store_file(char * filename, char * hash);

/**
* @brief Recupera un archivo del repositorio
*
* @param hash Hash del archivo: nombre del archivo en el repositorio
* @param filename Nombre del archivo
*
* @return 1 en caso de exito, 0 en caso de error
*/
int retrieve_file(char * hash, char * filename);

/**
 * @brief Adiciona una nueva version de un archivo.
 *
 * @param filename Nombre del archivo.
 * @param comment Comentario de la version.
 * @param hash Hash del contenido.
 *
 * @return 1 en caso de exito, 0 en caso de error.
 */
int add_new_version(file_version * v);


//IMPLEMENTACION DE LAS FUNCIONES

// Llena a estructura result recibida por referencia.
// Debe validar:
// 1. Que el archivo exista y sea un archivo regular
// 2. Obtiene y guarda en la estructura el HASH del archivo
// Llena todos los atributos de la estructura y retorna VERSION_CREATED
// En caso de fallar alguna validacion, retorna VERSION_ERROR
return_code create_version(char * filename, char * comment, file_version * result) {

	struct stat s;
	char hash[PATH_MAX];
	//Verificar que el archivo existe y es un archivo regular
	if (stat(filename, &s) == -1) {
		perror("Error al verificar el archivo");
		return VERSION_ERROR;
	}
	if (!S_ISREG(s.st_mode)) {
		perror("El archivo no es un archivo regular");
		return VERSION_ERROR;
	}
	if(!get_file_hash(filename, hash)) {
		perror("Error al obtener el hash del archivo");
		return VERSION_ERROR;
	}
	//Buscar si hash ya existe en la base de datos
	if (version_exists(filename, hash) == 1) {
		perror("La version ya existe en la base de datos");
		return VERSION_ALREADY_EXISTS;
	}
	strncat(result->filename, filename, PATH_MAX);
	strncat(result->hash, hash, HASH_SIZE);
	strncat(result->comment, comment, COMMENT_SIZE);
	return VERSION_CREATED;

}


return_code add(char * filename, char * comment) {

	file_version v;

	// 1. Crea la nueva version en memoria
	// Si la operacion falla, retorna VERSION_ERROR
	// create_version(filename, comment, &v)

	// 2. Verifica si ya existe una version con el mismo hash
	// Retorna VERSION_ALREADY_EXISTS si ya existe
	//version_exists(filename, v.hash)

	// 3. Almacena el archivo en el repositorio.
	// El nombre del archivo dentro del repositorio es su hash (sin extension)
	// Retorna VERSION_ERROR si la operacion falla
	//store_file(filename, v.hash)

	// 4. Agrega un nuevo registro al archivo versions.db
	// Si no puede adicionar el registro, se debe borrar el archivo almacenado en el paso anterior
	// Si la operacion falla, retorna VERSION_ERROR
	//add_new_version(&v)

	// Si la operacion es exitosa, retorna VERSION_ADDED
	return VERSION_ERROR;
}

int add_new_version(file_version * v) {
	FILE * fileDb;
	fileDb = fopen(VERSIONS_DB_PATH, "wb");
	if(fileDb == NULL){
		perror("Error al abrir la base de datos");
		return 0;
	}
	fseek(fileDb, 0, SEEK_END); // mover el puntero al final del archivo para agregar un nuevo registro
	if (fwrite(v, sizeof(file_version), 1, fileDb) < (v, sizeof(file_version) )){
		perror("Error al escribir en la base de datos");
		if (fclose(fileDb) == EOF){
			perror("Error al cerrar la base de datos");
			exit(EXIT_FAILURE); // Salimos del programa , error critico
		}
		return 0;
	} // escribir el nuevo registro en el archivo
	if (fclose(fileDb ) == EOF){
		perror("Error al cerrar la base de datos");
		exit(EXIT_FAILURE); // Salimos del programa , error critico
	}
	// Adiciona un nuevo registro (estructura) al archivo versions.db
	return 1;
}


void list(char * filename) {

	//Abre el la base de datos de versiones (versions.db)
	//Muestra los registros cuyo nombre coincide con filename.
	//Si filename es NULL, muestra todos los registros.

}

char *get_file_hash(char * filename, char * hash) {
	/*
	char *comando; NO SÉ PARA QUE SE USA
	FILE * fp;
	*/
	struct stat s;

	//Verificar que el archivo existe y que se puede obtener el hash
	if (stat(filename, &s) < 0 || !S_ISREG(s.st_mode)) {
		perror("Error al verificar el archivo");
		return NULL;
	}

	sha256_hash_file_hex(filename, hash);
	if(strlen(hash) == 0) {
		perror("Error al obtener el hash del archivo");
		return NULL;
	}
	return hash;

}

int copy(char * source, char * destination) {
	// Copia el contenido de source a destination (se debe usar open-read-write-close, o fopen-fread-fwrite-fclose)
}

// Verifica si existe una version para un archivo
int version_exists(char * filename, char * hash){
	FILE *fileDb;
	file_version r;
	fileDb = fopen(VERSIONS_DB_PATH, "rb");
	if()(fileDb == NULL){
		return 0;
	}
	while(fread(&r, sizeof(r), 1, fileDb)){
		if(strcmp(r.filename, filename) == 0 && strcmp(r.hash, hash) == 0){
			fclose(fileDb);
			return 1;
		}
	}
	fclose(fileDb);
	return 0;
}


int get(char * filename, int version) {

	file_version r;

	//1. Abre la BD y busca el registro r que coincide con filename y version
	//retrieve_file(r.hash, r.filename)
	return 0;
}


// Guarda copia del archivo en .version/hash

int store_file(char * filename, char * hash) {
	char dst_filename[PATH_MAX];
	snprintf(dst_filename, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	return copy(filename, dst_filename);
}

// Recupera version del archivo en .version/hash

int retrieve_file(char * hash, char * filename) {
	char src_filename[PATH_MAX];
	snprintf(src_filename, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	return copy(src_filename, filename);
}

