#define _BSD_SOURCE
#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * BUFFER macro
 * @author : ALVES Quentin
 * @note : Crée un buffer d'octet de taille SIZE et de nom NAME.
 **/
#define BUFFER( NAME, SIZE ) char NAME[ SIZE ]

/**
 * NONE macro
 * @author : ALVES Quentin
 * @note : Définie un alias pour 0 aka NULL pour unix.
 **/
#define NONE 0

/**
 * string_t type alias
 * @author : ALVES Quentin
 **/
typedef const char* string_t;

/**
 * stat_t type alias
 * @author : ALVES Quentin
 **/
typedef struct stat stat_t;

/**
 * bool_t type alias
 * @author : ALVES Quentin
 **/
typedef enum { B_TRUE, B_FALSE } bool_t;

/**
 * file_t type
 * @author : ALVES Quentin
 * @field handle : Contient le pointeur vers le fichier de type unix.
 * @field stats : Contient l'état du fichier, permissions, date de créations...
 **/
typedef struct {
	int handle;
	stat_t stats;
} file_t;

/**
 * directory_t type
 * @author : ALVES Quentin
 * @field handle : Contient le pointeur vers le répertoire de type unix.
 * @field loader : Contient l'élémente courrant du répertoire.
 **/
typedef struct {
	DIR* handle;
	struct dirent* loader;
} directory_t;

/**
 * reset_buffer méthode
 * @author : ALVES Quentin
 * @note : Reset un buffer avec une valeur prédéfinie.
 * @param buffer : Le buffer à reset.
 * @param value : La valeur par défault du buffer.
 **/
void reset_buffer( char* buffer, string_t value ) {
	// Si le buffer n'est pas NULL.
	if ( buffer ) {
		// Si une valeur par défault est définie.
		// On remplace le contenue du buffer par la valeur.
		if ( value && strlen( value ) > 0 ) 
			strcpy( buffer, value );
		else // Sinon on définie un buffer vide.
			buffer[ 0 ] = '\0';
	}
}

/**
 * append_buffer méthode
 * @author : ALVES Quentin
 * @note : Ajoute une suite de caractère à ceux déjà présent.
 * @param buffer : Le buffer souhaiter.
 * @param value : La chaine à ajouter.
 **/
void append_buffer( char* buffer, string_t value ) {
	// Si le buffer n'est pas NULL et la chaine valide.
	// On concaténe les deux.
	if ( buffer && value && strlen( value ) > 0 ) 
		strcat( buffer, value );
}

/**
 * openf fonction
 * @author : ALVES Quentin
 * @note : On ouvre un fichier avec une instance de file_t.
 * @param file : Pointeur vers une instance de file_t pour stocket les données.
 * @param path : Le chemin vers le fichier.
 * @param flags : Les flags utilisé pour ouvrir le fichier.
 * @param mode : Les permisions utilisé pour ouvrir le fichier.
 * @return : bool_t
 **/
bool_t openf( file_t* file, string_t path, int flags, int mode ) {
	// Si le pointeur vers le fichier n'est pas NULL et que le chemin est utilisable.
	if ( file && path && strlen( path ) > 0 ) {
		// On ouvre le fichier.
		if ( mode != NONE )
			file->handle = open( path, flags, mode );
		else
			file->handle = open( path, flags );
			
		// Si le fichier est ouvert.
		if ( file->handle != -1 ) {
			// On récupère les stats du fichier.
			fstat( file->handle, &file->stats );
			
			return B_TRUE;
		}
	}
	
	return B_FALSE;
}

/**
 * is_file_valid fonction
 * @author : ALVES Quentin
 * @note : Retourne vraie si le fichier est valide.
 * @param file : Pointeur constant vers l'instance de file_t à vérifier.
 * @return : bool_t
 **/
bool_t is_file_valid( const file_t* file ) {
	// Si l'instance est valide.
	if ( file && file->handle != -1 )
		return B_TRUE;

	return B_FALSE;
}

/**
 * set_file_mode méthode
 * @author : ALVES Quentin
 * @note : Redéfinie les permissions du fichier.
 * @param file : Pointeur vers l'instance de file_t à utiliser.
 * @param mode : Les nouvelles permissions.
 **/
void set_file_mode( file_t* file, mode_t mode ) {
	// Si le fichier est valide on change sont mode.
	if ( is_file_valid( file ) == B_TRUE ) 
		fchmod( file->handle, mode );
}

/**
 * readf fonction
 * @author : ALVES Quentin
 * @note : Retourne le nombre de lecture ou NONE.
 * @param file : Pointeur constant vers l'instance de file_t à utiliser.
 * @param buffer : Le buffer à utiliser.
 * @param size : La taille du buffer en octet.
 * @return : int
 **/
int readf( const file_t* file, char* buffer, int size ) {
	// Si le fichier et le buffer sont valide on lit.
	if ( is_file_valid( file )  == B_TRUE && buffer && size > 0 )
		return read( file->handle, buffer, size );
	
	return NONE;
}

/**
 * writef fonction
 * @author : ALVES Quentin
 * @note : Retourne le nombre d'écriture ou NONE.
 * @param file : Pointeur constant vers l'instance de file_t à utiliser.
 * @param buffer : Le buffer à utiliser.
 * @param size : La taille du buffer en octet.
 * @return : int
 **/
int writef( const file_t* file, char* buffer, int size ) {
	// Si le fichier et le buffer sont valide on écrit.
	if ( is_file_valid( file )  == B_TRUE && buffer && size > 0 ) 
		return write( file->handle, buffer, size );
	
	return NONE;
}

/**
 * closef méthode
 * @author : ALVES Quentin
 * @note : Ferme le fichier d'instance file_t.
 * @param file : Pointeur vers l'instance de file_t à utiliser.
 **/
void closef( file_t* file ) {
	// Si le fichier est valide on le ferme.
	if ( is_file_valid( file ) == B_TRUE ) {
		close( file->handle );
		file->handle = NONE;
	}
}

/**
 * opend fonction
 * @author : ALVES Quentin
 * @note : Retourne vraie si le répertoire est ouvert.
 * @param directory : Pointeur vers l'instance de directory_t à utiliser.
 * @param path : Le chemin du répertoire souhaiter.
 * @return : bool_t
 **/
bool_t opend( directory_t* directory, string_t path ) {
	// Si le pointeur vers l'instance de directory_t à utiliser et le chemin sont valide.
	if ( directory && path && strlen( path ) > 0 ) {
		// On ouvre le répertoire.
		directory->handle = opendir( path );
		
		// Si le répertoire est ouvert.
		if ( directory->handle )
			return B_TRUE;
	}

	return B_FALSE;
}

/**
 * is_directory_valid fonction
 * @author : ALVES Quentin
 * @note : Retourne vraie si l'instance de directory_t est valide.
 * @param directory : Pointeur constant vers l'instance de directory_t à vérifier.
 * @return : bool_t
 **/
bool_t is_directory_valid( const directory_t* directory ) {
	// Si le l'instance n'est pas NULL et l'handle n'est pas NULL.
	if ( directory && directory->handle )
		return B_TRUE;

	return B_FALSE;
}

/**
 * directory_is_file fonction
 * @author : ALVES Quentin
 * @note : Retourne vraie si l'élément courrant de l'instance de directory_t est un fichier.
 * @param directory : Pointeur constant vers l'instance de directory_t à utiliser.
 * @return : bool_t
 **/
bool_t directory_is_file( const directory_t* directory ) {
	// Si le répertoire est valide.
	if ( is_directory_valid( directory ) == B_TRUE ) {
		// Si l'élément courant n'est pas NULL et sont type vaut 8 ( fichier ). 
		if ( directory->loader && directory->loader->d_type == 8 )
			return B_TRUE;
	}
	
	return B_FALSE;
}

/**
 * directory_is_dir fonction
 * @author : ALVES Quentin
 * @note : Retourne vraie si l'élément courrant de l'instance de directory_t est un répertoire.
 * @param directory : Pointeur constant vers l'instance de directory_t à utiliser.
 * @return : bool_t
 **/
bool_t directory_is_dir( const directory_t* directory ) {
	// Si le répertoire est valide.
	if ( is_directory_valid( directory ) == B_TRUE ) {
		// Si l'élément courant n'est pas NULL et sont type vaut 4 ( répertoire ). 
		if ( directory->loader && directory->loader->d_type == 4 )
			return B_TRUE;
	}
	
	return B_FALSE;
}

/**
 * directory_is_dir fonction
 * @author : ALVES Quentin
 * @note : Retourne le nom de l'élément courrant d'une instance valide de directory_t,
 *		   sinon une chaine vide.
 * @param directory : Pointeur constant vers l'instance de directory_t à utiliser.
 * @return : string_t
 **/
string_t directory_name( const directory_t* directory ) {
	// Si le répertoire est valide et l'élément courant n'est pas NULL
	if ( is_directory_valid( directory ) == B_TRUE && directory->loader ) 
		return directory->loader->d_name;
	
	return "\0";
}

/**
 * readd fonction
 * @author : ALVES Quentin
 * @note : Retourne vraie si la lecture du prochain élément d'un répertoire n'est pas NULL.
 * @param directory : Pointeur vers l'instance de directory_t à utiliser.
 * @return : bool_t
 **/
bool_t readd( directory_t* directory ) {
	// Si le répertoire est valide.
	if ( is_directory_valid( directory ) == B_TRUE ) {
		// On lit le prochain élément.
		directory->loader = readdir( directory->handle );
		
		// Si l'élément est valide.
		if ( directory->loader ) {
			// Si l'élément ne commence pas par un .
			if ( directory->loader->d_name[ 0 ] != '.' )
				return B_TRUE;
			else // Sinon on le saute.
				return readd( directory );
		}
	}
	
	return B_FALSE;
}

/**
 * closed méthode
 * @author : ALVES Quentin
 * @note : Ferme le répertoire d'instance directory_t.
 * @param directory : Pointeur vers l'instance de directory_t à utiliser.
 **/
void closed( directory_t* directory ) {
	if ( is_directory_valid( directory ) == B_TRUE ) {
		closedir( directory->handle );
	
		directory->handle = NULL;
		directory->loader = NULL;
	}
}

/**
 * closed méthode
 * @author : ALVES Quentin
 * @note : Crée un répertoire <destination> avec les mêmes permissions 
 *		   que le répertoire <source>.
 * @param destination : Chemin vers le répertoire de destination.
 * @param source : Chemin vers le répertoire d'origine.
 **/
void create_dir( string_t destination, string_t source ) {
	if ( 
		destination && strlen( destination ) > 0 && 
		source && strlen( source ) > 0 
	) {
		stat_t dir_stats;
		stat( source, &dir_stats );
		mkdir( destination, dir_stats.st_mode );
	}
}

/**
 * is_directory fonction
 * @author : ALVES Quentin
 * @note : Retourne vraie si le chemin donnée ne contient pas de .
 * @param path : Chemin à vérifer.
 * @return : bool_t
 **/
bool_t is_directory( string_t path ) {
	// Si le chemin est uilisable.
	if ( path && strlen( path ) > 0 ) {
		// On récupère sa longeur.
		int len = strlen( path );
		
		// Tant qu'on à pas atteint le permier caratère.
		while ( len > 0 ) {
			len -= 1;
			
			// Si le caratère courrant est un . alors c'est un fichier.
			if ( path[ len ] == '.' )
				return B_FALSE;
		}
	}
	
	return B_TRUE;
}

/**
 * copy méthode
 * @author : ALVES Quentin
 * @note : Crée un répertoire <destination> avec les mêmes permissions 
 *		   que le répertoire <source>.
 * @param destination : Chemin vers le répertoire de destination.
 * @param source : Chemin vers le répertoire d'origine.
 **/
void copy( string_t path_in, string_t path_out ) {
	// Si le chemin d'entrée n'est pas un répertoire.
	if ( is_directory( path_in ) == B_FALSE ) {
		// Si le chemin de sortie est un répertoire.
		if ( is_directory( path_out ) == B_TRUE ) {
			printf( "Un fichier ne peut être copier que dans un nouveau fichier !\n" );
			return;
		}
		
		// On crée les instance de file_t.
		file_t file_in;
		file_t file_out;
		
		// On ouvre les deux fichiers.
		if ( 
			openf( &file_in, path_in, O_RDONLY, NONE ) == B_TRUE &&
			openf( &file_out, path_out, O_WRONLY | O_CREAT | O_EXCL, 0666 ) == B_TRUE 
		) {
			// On change le mode du fichier de sortie pour le faire correspondre à l'entrée.
			set_file_mode( &file_out, file_in.stats.st_mode );
			
			// On affiche copie du fichier.
			printf( "Copie du fichier <%s> vers <%s>\n", path_in, path_out );
			
			// On crée un buffer pour stocker les données.
			BUFFER( buffer, 4096 );
			while ( 1 ) {
				// on lit un bout ( readcount ) du fichier qu'on stocke dans le buffer
				int rcnt = readf( &file_in, buffer, sizeof(buffer) );
				
				// Tant qu'on lit quelque chose.
				if ( rcnt != 0 ) {
					//  partie ecriture
					int pos = 0;
					while ( rcnt != 0 ) {
						// buffer + pos : buffer correspond a un pointeur et pour avoir
						// l'endroit actuel ou on en est du buffer
						int wcnt = writef( &file_out, buffer + pos, rcnt );
						rcnt -= wcnt;
						pos += wcnt; 
					}
				} else
					break;
		    }
		}
		
		// On ferme les fichiers.
		closef( &file_in );
		closef( &file_out );
	} else {
		// Si le chemin de sortie n'est pas un répertoire.
		if ( is_directory( path_out ) == B_FALSE ) {
			printf( "Un répertoire ne peut être copier que dans un nouveau répertoire !\n" );
			return;
		}
		
		// On crée l'instance de directory_t.
		directory_t directory;
		
		// On ouvre le répertoire avec le chemin d'entrée.
		if ( opend( &directory, path_in ) == B_TRUE ) {
			// On crée les buffers de chemins.
			BUFFER( buff_path_in, 4096 );
			BUFFER( buff_path_out, 4096 );
			
			// Tant que l'on trouve un élément dans le répertoire.
			while( readd( &directory ) == B_TRUE ) {
				// On reset les buffer.
				reset_buffer( buff_path_in, path_in );
				reset_buffer( buff_path_out, path_out );

				// On ajoute l'élement courrant au chemin d'entrée.
				append_buffer( buff_path_in, "/" );
				append_buffer( buff_path_in, directory_name( &directory ) );

				// On ajoute l'élement courrant au chemin de sortie.
				append_buffer( buff_path_out, "/" );
				append_buffer( buff_path_out, directory_name( &directory ) );
					
				// Si le chemin de sortie esr un répertoire.
				if ( is_directory( buff_path_out ) == B_TRUE ) {
					// On affiche copie du répertoire.
					printf( "Copie du répertoire <%s> vers <%s>\n", buff_path_in, buff_path_out );
				
					// On crée le nouveau répertoire.
					create_dir( buff_path_out, buff_path_in );
				}
					
				// On copie récursivement le contenue du répertoire.
				copie( buff_path_in, buff_path_out );
			}

			// On ferme le répertoire.
			closed( &directory );
		}
	}
}

/**
 * main fonction
 * @author : ALVES Quentin
 * @node : Programme principale.
 * @param argc : Nombre d'argument fournie.
 * @param argv : Valeur des arguments sous forme de texte.
 * @return : int
 **/
int main( int argc, char** argv ) {
	if ( argc == 3 ) {
		copy( argv[ 1 ], argv[ 2 ] );
	} else
		printf( "Il faut donnée 2 répertoire pour éffectuée une copie !\n" );

	return 0;
}
