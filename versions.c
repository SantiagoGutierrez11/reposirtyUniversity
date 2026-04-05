/**
 * @file
 * @brief Implementacion del API de gestion de versiones
 * @author Erwin Meza Vega <emezav@unicauca.edu.co>
 * @copyright MIT License
*/

#include "versions.h"

return_code create_version(char * filename, char * comment, file_version * result);
int version_exists(char * filename, char * hash);
char *get_file_hash(char * filename, char * hash);
int copy(char * source, char * destination);
int store_file(char * filename, char * hash);
int retrieve_file(char * hash, char * filename);
int add_new_version(file_version * v);
void delete_version_file(char * hash);
/**
 * @brief Verifica si un archivo ya tiene una version en la base de datos
 * @param filename Nombre del archivo
 * @return 1 si el archivo ya tiene una version, 0 si no
 */
int file_exists_in_db(char * filename):
//IMPLEMENTACION DE LAS FUNCIONES

/**
 * @brief Crea una version en memoria del archivo
 * Valida si el archivo especificado existe y crea su hash
 * @param filename Nombre del archivo
 * @param comment Comentario
 * @param result Nueva version en memoria
 * @return Resultado de la operacion
 */
return_code create_version(char * filename, char * comment, file_version * result) {
	if (!filename || !comment || !result){
		fprintf(stderr, "Argumentos invalidos para crear la version\n");
		return VERSION_ERROR;
	}

	struct stat s;
	char hash[HASH_SIZE];
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
	int ret = 0;
	ret = snprintf(result->filename, PATH_MAX, "%s", filename);
	if (ret < 0 || ret >= PATH_MAX) {
		perror("Error al copiar el nombre del archivo");
		return VERSION_ERROR;
	}
	ret = snprintf(result->hash, HASH_SIZE, "%s", hash);
	if (ret < 0 || ret >= HASH_SIZE) {
		perror("Error al copiar el hash del archivo");
		return VERSION_ERROR;
	}
	ret = snprintf(result->comment, COMMENT_SIZE, "%s", comment);
	if (ret < 0 || ret >= COMMENT_SIZE) {
		perror("Error al copiar el comentario");
		return VERSION_ERROR;
	}
	return VERSION_CREATED;
}

/**
 * @brief Adiciona una nueva version de un archivo al repositorio
 * @param filename Nombre del archivo
 * @param comment Comentario de la version
 * @return Codigo de retorno de la operacion
 */
return_code add(char * filename, char * comment) {

	file_version v;

	// 1. Crea la nueva version en memoria
	// 2. Verifica si ya existe una version con el mismo hash
	// Si la operacion falla, retorna el codigo de error correspondiente
	return_code rv = create_version(filename, comment, &v);
	if(rv != VERSION_CREATED) {
		return rv;
	}
	
	// 3. Almacena el archivo en el repositorio.
	int resStore = store_file(filename, v.hash);
	// El nombre del archivo dentro del repositorio es su hash (sin extension)
	// Retorna VERSION_ERROR si la operacion falla
	if (resStore == 0) {
		perror("Error al almacenar el archivo en el repositorio");
		return VERSION_ERROR;
	}

	// 4. Agrega un nuevo registro al archivo versions.db
	// Si no puede adicionar el registro, se debe borrar el archivo almacenado en el paso anterior
	// Si la operacion falla, retorna VERSION_ERROR
	if (!add_new_version(&v)) {
		perror("Error al adicionar la version a la base de datos");
		delete_version_file(v.hash); // Elimina el archivo almacenado en el repositorio
		return VERSION_ERROR;
	}
	int es_nuevo_archivo = !file_exists_in_db(filename); // Verificar si el archivo es nuevo en la base de datos
	return es_nuevo_archivo ? FILE_ADDED : VERSION_ADDED; // Retorna FILE_ADDED si es un nuevo archivo, VERSION_ADDED si es una nueva version de un archivo existente
}

/**
 * @brief Elimina un archivo del repositorio
 * @param hash Hash del archivo
 */
void delete_version_file(char * hash) {
	char filePath[PATH_MAX];
	int res = snprintf(filePath, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	if (res < 0 || res >= PATH_MAX) {
		perror("Error al construir la ruta del archivo");
		return;
	}
	if (remove(filePath) != 0) {
		perror("Error al eliminar el archivo del repositorio");
	}
}

/**
 * @brief Adiciona una nueva version a la base de datos
 * @param v Estructura de la version
 * @return 1 en caso de exito, 0 en caso de error
 */
int add_new_version(file_version * v) {
	FILE * fileDb;
	fileDb = fopen(VERSIONS_DB_PATH, "ab"); // se abre el archivo en modo append para agregar un nuevo registro al final
	if(fileDb == NULL){
		perror("Error al abrir la base de datos");
		return 0;
	}
	if (fwrite(v, sizeof(file_version), 1, fileDb) != 1){
		perror("Error al escribir en la base de datos");
		fclose(fileDb);
		return 0;
	} // escribir el nuevo registro en el archivo
	fclose(fileDb);
	// Adiciona un nuevo registro (estructura) al archivo versions.db
	return 1;
}

/**
 * @brief Lista las versiones de un archivo o todos los archivos
 * @param filename Nombre del archivo, NULL para todos
 */
void list(char * filename) {

	FILE *fileDb;
	fileDb = fopen(VERSIONS_DB_PATH, "rb");
	int count_versions = 0;
	if(fileDb == NULL){
		perror("Error al abrir la base de datos");
		return;
	}
	file_version r;
	if(filename == NULL) {
		// Listar todo el repositorio
		while(fread(&r, sizeof(r), 1, fileDb)){
			printf("Archivo: %s, Hash: %s, Comentario: %s\n", r.filename, r.hash, r.comment);
		}
		fclose(fileDb);
	}else {
		// Listar las versiones del archivo solicitado
		while(fread(&r, sizeof(r), 1, fileDb)){
			if(strcmp(r.filename, filename) == 0){
				count_versions++;
				printf("Archivo: %s, Hash: %s, Comentario: %s, Version:%d\n", r.filename, r.hash, r.comment, count_versions);
			}
		}
		fclose(fileDb);
	}
}

char *get_file_hash(char * filename, char * hash) {
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

/**
 * @brief Copia un archivo
 * @param source Archivo fuente
 * @param destination Archivo destino
 * @return 1 exito, 0 error
 */
int copy(char * source, char * destination) {
	FILE *fdSource, *fdDestination;
	fdSource = fopen(source, "rb");
	if (fdSource == NULL) {
		perror("Error al abrir el archivo fuente");
		return 0;
	}
	fdDestination = fopen(destination, "wb");
	if (fdDestination == NULL) {
		fclose(fdSource);
		perror("Error al abrir el archivo destino");
		return 0;
	}
	char buffer[4096]; // Lee y escribe en bloques de 4KB
	size_t bytesRead;
	while((bytesRead = fread(buffer, 1, sizeof(buffer), fdSource)) > 0) {
		if (fwrite(buffer, 1, bytesRead, fdDestination) != bytesRead) {
			perror("Error al escribir en el archivo destino");
			fclose(fdSource);
			fclose(fdDestination);
			return 0;
		}
	}
	if(ferror(fdSource)){ // verificar si ocurrió un error durante la lectura
		perror("Error al leer el archivo fuente");
		fclose(fdSource);
		fclose(fdDestination);
		return 0;
	}

	fclose(fdSource);
	fclose(fdDestination);
	return 1;
}

/**
 * @brief Verifica si existe una version para un archivo
 * @param filename Nombre del archivo
 * @param hash Hash del contenido
 * @return 1 si existe, 0 no
 */
int version_exists(char * filename, char * hash){
	FILE *fileDb;
	file_version r;
	fileDb = fopen(VERSIONS_DB_PATH, "rb");
	if(fileDb == NULL){
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

/**
 * @brief Obtiene una version del archivo
 * @param filename Nombre del archivo
 * @param version Numero de version
 * @return 1 exito, 0 error
 */
int get(char * filename, int version) {
	int version_count = 0;
	file_version r;
	FILE *fileDb;
	fileDb = fopen(VERSIONS_DB_PATH, "rb");
	if(fileDb == NULL){
		perror("Error al abrir la base de datos");
		return 0;
	}
	int found = 0; // Bandera para indicar si se encontró la versión solicitada
	while(fread(&r, sizeof(r), 1, fileDb)){
		if(strcmp(r.filename, filename) == 0){
			version_count++;
			if(version_count == version){	
				found = 1; // Se encontró la versión solicitada
				break;
			}
		}
	}
	if(fclose(fileDb) == EOF){
		perror("Error al cerrar la base de datos");
		return 0;
	}
	if(!found){
		// no se encontro la version solicitada
		perror("No se encontro la version solicitada");
		return 0;
	}

	if(retrieve_file(r.hash, r.filename) == 0){
		// no se pudo recuperar el archivo del repositorio
		perror("Error al recuperar el archivo del repositorio");
		return 0;
	}
	// se recupero el archivo del repositorio exitosamente
	return 1;
}

/**
 * @brief Almacena un archivo en el repositorio
 * @param filename Nombre del archivo
 * @param hash Hash del archivo
 * @return 1 exito, 0 error
 */
int store_file(char * filename, char * hash) {
	char dst_filename[PATH_MAX];
	int res = snprintf(dst_filename, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	if (res < 0 || res >= PATH_MAX) {
		perror("Error al construir la ruta del archivo");
		return 0;
	}
	return copy(filename, dst_filename);
}
/**
 * @brief Recupera un archivo del repositorio
 * @param hash Hash del archivo
 * @param filename Nombre del archivo destino
 * @return 1 exito, 0 error
 */
int retrieve_file(char * hash, char * filename) {
	char src_filename[PATH_MAX];
	int res = snprintf(src_filename, PATH_MAX, "%s/%s", VERSIONS_DIR, hash);
	if (res < 0 || res >= PATH_MAX) {
		perror("Error al construir la ruta del archivo");
		return 0;
	}
	return copy(src_filename, filename);
}
// Verifica si el archivo ya fue agregado alguna vez (sin importar el hash)
int file_exists_in_db(char * filename) {
    FILE *fileDb;
    file_version r;
    fileDb = fopen(VERSIONS_DB_PATH, "rb");
    if(fileDb == NULL) return 0;
    
    while(fread(&r, sizeof(r), 1, fileDb)) {
        if(strcmp(r.filename, filename) == 0) {
            fclose(fileDb);
            return 1;  // el archivo ya tiene al menos una version
        }
    }
    fclose(fileDb);
    return 0;
}
