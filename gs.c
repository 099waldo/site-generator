#include <stdio.h> // printf
#include <stdlib.h> // malloc
#include <string.h> // memcpy, strlen, strcpy
#include <dirent.h> // for getting all files in directory

#define TEMPLATEFILE "template.html"
#define DIVIDER_STRING "<++>"

#define CONTENT_DIR "content/"
#define WWW_DIR "www/"

void updateLast(char *buffer, char lastchar){
	for(int i=0;i<strlen(DIVIDER_STRING)-1;i++){
		buffer[i] = buffer[i+1];
	}
	buffer[strlen(DIVIDER_STRING)-1] = lastchar;
	buffer[strlen(DIVIDER_STRING)] = '\0';
}

int splice(char* templatefile, char* contentfile, char* outputfile){
	FILE *fp = fopen(templatefile, "r");
	FILE *fc = fopen(contentfile, "r");
	FILE *fw = fopen(outputfile, "w");

	if(fp == NULL){
		perror("Failed to open template file");
		return 1;
	}
	if(fc == NULL){
		perror("Failed to open content file");
		return 1;
	}
	if(fw == NULL){
		perror("Failed to open output file");
		return 1;
	}

	char *divider = malloc((strlen(DIVIDER_STRING)+1)*sizeof(char));
	strcpy(divider, DIVIDER_STRING);
	char *last = malloc((strlen(DIVIDER_STRING)+1)*sizeof(char));
	strcpy(last, DIVIDER_STRING);

	int rc=0;
	while(rc != EOF){
		rc = getc(fp);
		if(rc == EOF){
			break;
		}

		updateLast(last, (char)rc);
		if(strcmp(last, divider) == 0){
			fseek(fw, -(strlen(DIVIDER_STRING)-1), SEEK_CUR);
			// Now write from the content file.
			while(rc != EOF){
				rc = getc(fc);
				if(rc == EOF){
					break;
				}
				fputc(rc, fw);
			}
			rc = 0;
		}
		fputc(rc, fw);
	}

	free(divider);
	free(last);

	fclose(fp);
	fclose(fc);
	fclose(fw);

	return 0;
}

int checkExt(char* filename, char* ext){
	unsigned int i=0;
	while(filename[i] != '\0'){
		if(filename[i] == *ext){
			return 1;
		}
		i++;
	}
	return 0;
}

// outputfilename = dirname+fullname
char *outputfilename(char *fullname, char *dirname){
	int length = strlen(fullname);
	int dirlength = strlen(dirname);

	char *output=malloc((length+dirlength+1)*sizeof(char));
	memcpy(output, dirname, sizeof(dirname));
	for(int i=0;i<length;i++){
		output[i+dirlength] = fullname[i];
	}
	output[length+dirlength] = '\0';
	return output;
}

int main(void){
	struct dirent *files;
	DIR *dir = opendir(CONTENT_DIR);
	if(dir == NULL){
		printf("Director cannot be opened\n");
		return 0;
	}
	char html[5] = "html";
	while((files = readdir(dir)) != NULL){
		if(checkExt(files->d_name, html)){
			char *pathout = outputfilename(files->d_name, WWW_DIR);
			char *pathin = outputfilename(files->d_name, CONTENT_DIR);

			splice(TEMPLATEFILE, pathin, pathout);

			free(pathout);
			free(pathin);
		}
	}
	closedir(dir);
	return 0;
}
