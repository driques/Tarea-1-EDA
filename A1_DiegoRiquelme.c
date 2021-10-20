/*
Diego Riquelme
20.595.397-3
Profesora Violeta Chang A1
Analisis de datos y estructura de algoritmos
*/



////////////////////////////////////////////////////////////////
#include <time.h>
////////////////////////////////////////////////////////////////

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>


//Generacion de struct para crear un array de structs con
//la información de cada canción, struct songs va orientado 
//al archivo que contiene las canciones, mientras que playContent 
//al que contiene la información necesaria para la playlist.
typedef struct songs songs;
typedef struct playContent playContent;
struct songs {
    char nameSong[25];
    int genre;
    char duration[9];
    char artist[25];
};


struct playContent{
    char listGenre[25];
    char listDuration[9]; 
};



//funcion que nos entrega el tamaño del archivo, retorna el tamaño, tiene como entrada un 
//array con el nombre del archivo a revisar.
int fileSize(char fileName[]){
	FILE *dataFile;
	char num[9];
	dataFile = fopen(fileName,"r");
	fgets(num,9,dataFile);
	int initNum=atoi(num);
    fclose(dataFile);
	return initNum;
}


//Función que abre el archivo del tipo listado, tiene como entrada el tamaño del listado y el nombre 
//Del archivo, retorna un array struct con los datos correspondientes al mismo.
songs *openFile(int fileSize,char fileName[]){
    printf("Cantidad canciones %d\n",fileSize);

    songs *arraySongs = (songs *)malloc(sizeof(songs)*fileSize);
    char space[2] = " ";
    char *zeros="00:0";
    char *token;
    char charData[255];
    char dato;
    FILE *dataFile;
	dataFile = fopen(fileName,"r");
	int i=0;
	int k=0;
	while (i<=fileSize){
	 	fgets(charData,100,dataFile);
        
 		if (i>0){
           
            strtok(charData,"\n");
            token = strtok(charData,space);
            int j=0;
            while( token != NULL ){
                if (j==0){
                   
                    stpcpy(arraySongs[k].nameSong,token);
                }
                if (j==1){
                   
                    arraySongs[k].genre=atoi(token);
                   
                }
                if (j==2){
                    /*Las siguientes lineas permiten transformar del 
                    MM:SS a HH:MM:SS*/
                    char newConcat[9]="";
                    
                    strcat(newConcat,zeros);
                   
                    strcat(newConcat,token);
                    
            
                    strcpy(arraySongs[k].duration,newConcat);
                    
                }
                if (j==3){
                    strcpy(arraySongs[k].artist,token);   
                }
                
                j++;
                token = strtok(NULL, space);
            }
            k=k+1;
	 	}
	 	i=i+1;
    }
    fclose(dataFile);
    return arraySongs; 
    
   
}


/*Función que permite obtener los datos desde el archivo destinado a la playlist
Tiene como entrada el tamaño del archivo, el nombre y el puntero de un id para guardar 
el genero de la lista a crear, retorna un array struct del tipo playContent*/

playContent *playCapture(int size,char fileName[], int *idPlayList){
    playContent * content =  (playContent *)malloc(sizeof(playContent)*(size+1));
    FILE *dataFile;
    char charData[255];
    char *token;
    dataFile = fopen(fileName,"r");
    int genre;
    int i=0;
    while (i<=size){
        fgets(charData,100,dataFile);
        if (i==0){
            char duration[7];
            genre=atoi(&charData[2]);
            for (int j = 0; j < 8; j++){
                duration[j]=charData[4+j];
            }
            strtok(duration," ");
            
            strcpy(content[i].listDuration,duration);
        }
        else{
            token = strtok(charData," ");
            int j=0;
            while( token != NULL ){
                if (j==1){
                    strcpy(content[i-1].listGenre,token);
                }
                
                token = strtok(NULL," ");
                j++;   
            }
            
        }
        
        i++;
    }
    *idPlayList=genre;
    fclose(dataFile);
    return content;



}
/*Función que permite transformar un array del tipo HH:MM:SS a segundos con el fin de
trabajar más facilmente el calculo de la mejor lista, tiene como entrada el array en cuestion, y retorna 
int segundos.*/
int toSeconds(char hours[]){
    char hoursCopy[8];
    strcpy(hoursCopy,hours);
    char *token;
    int seconds=0;
    token = strtok(hoursCopy,":");
    int j=0;
    while( j<3){
        
        if (j==0){
            seconds=seconds+(atoi(token)*3600);
        }
        if (j==1){
            seconds=seconds+(atoi(token)*60);
        }
        if (j==2){
            seconds=seconds+(atoi(token));
            
            return seconds;
        }
        j++;
        token = strtok(NULL, ":");
    }
   
}

/*La funcion toHours ademas de transformar los segundos al formato HH:MM:SS, exporta
los datos al archivo .out con la playlist lista. */
void toHours(int sec,int quantSongs,char genre[],songs playlist[]){
    FILE *playlistFile = fopen("playlist.out","w");
    int hr=0, min=0;
    char *hourReturn;
    hr = sec /3600;
    sec = sec % 3600;
    min = sec / 60;
    sec %= 60;
    strtok(genre,"\n");
    //Creacion del archivo
    fprintf(playlistFile,"%d %d:%d:%d %s\n",quantSongs,hr,min,sec,genre);
    for (int i = 0; i < quantSongs; i++){
        fprintf(playlistFile,"%s ",playlist[i].nameSong);
        fprintf(playlistFile,"%s ",playlist[i].duration);
        fprintf(playlistFile,"%s\n",playlist[i].artist);
    }
    fclose(playlistFile);

    
    
    
    
    

}

/*Funcion que verifica que no se encuentre una canción ya seleccionada, tiene como entrada el struct array de canciones generadas y su largo, junto con
la canción a comparar, y retorna un 0 o un 1 si es que se encuentra o no dentro de este.*/

int notInSongs(songs sameGenre[],char song[], int sizeSameGenre){
    for (int i = 0; i < sizeSameGenre; i++){
        if (strcmp(song,sameGenre[i].nameSong)==0){
            
            return 1;
        }    
    }
    return 0;
    
}


/*Función creadora de la playlist, tiene como entrada el genero de la playlist, el array de structs de canciones, el tamanio de todas
las canciones, la duracion maxima de la playlist, y 2 punteros para guardar el tamanio de la playlist generada, y el tiempo de esta*/
songs* makePlayList(int genre,songs *arrayOfSongs, int sizeAllSongs, char maxDuration[], int *lenPlaylist, int * timePlaylist){
    int amountSongs = 0;
    songs *sameGenre;
    
    //Se verifica cuantas canciones pertenecen a un mismo genero
    for (int i = 0; i < sizeAllSongs; i++){
        if (arrayOfSongs[i].genre==genre){
            amountSongs++;
        }
    }
    printf("Cantidad mismas canciones: %d\n",amountSongs);
    //Se genera un array struct para guardar la playlist del mismo genero.
    //Luego se procede a ordenar.
    sameGenre= (songs *)malloc(sizeof(songs)*amountSongs);
    int k=0;
    for (int i = 0; i < sizeAllSongs; i++){
            if (arrayOfSongs[i].genre==genre && notInSongs(sameGenre,arrayOfSongs[i].nameSong,amountSongs)==0){
                    sameGenre[k]=arrayOfSongs[i];
                    k++;
                
            }
    }
    amountSongs=k;
    printf("Cantidad mismas canciones sin repetir: %d\n",amountSongs);
   //Bubble Sort
    for (int i = 0; i < amountSongs; i++){
           for (int j = 0; j < amountSongs-1-i; j++){
                if (toSeconds(sameGenre[j].duration)<toSeconds(sameGenre[j+1].duration)){
                        songs temporal=sameGenre[j];
                        sameGenre[j]=sameGenre[j+1];
                        sameGenre[j+1]=temporal;
                    
            }
       }
       
    }
    
    //Se comienza a tomar las decisiones para calcular las canciones que obtendra la playlist
    //Se elige la opcion optima local para un array ordenado descendentemente (mayor explicacion en el informe)
    int totalSeconds=0;
    for (int i = 0; i < amountSongs; ++i){
       
        totalSeconds=totalSeconds+toSeconds(sameGenre[i].duration);
    }

    int sumTime=0,i=0;
    //Si la suma total del tiempo de las canciones de un mismo genero supera al que se pide en la playlist,
    //se procede de la siguiente forma
    if (totalSeconds>toSeconds(maxDuration)){
       //Se hace una suma de los tiempos mientras sea menor que el maximo de ellos.
        while (sumTime < toSeconds(maxDuration)){
            sumTime=sumTime+toSeconds(sameGenre[i].duration);
            i++;
        };

        //Luego, se pregunta si la opcion optima local, se encuentra en el tiempo calculado,
        //o en una cancion anterior, para ello se calculan ambos tiempos, y la distancia al valor requerido
        //retorna la distancia mas cercana al tiempo requerido.
        
        int restTime=sumTime-toSeconds(sameGenre[i].duration);
       
        int higherTime=fabs(toSeconds(maxDuration)-sumTime);
        int lowerTime=fabs(toSeconds(maxDuration)-restTime);
        
        if(lowerTime<higherTime){
            *timePlaylist=restTime;
            
            i=i-1;
            *lenPlaylist=i;
            songs * returnPlaylist= (songs *)malloc(sizeof(songs)*i);
            for (int j = 0; j < i; j++){
                returnPlaylist[j]=sameGenre[j];
            }
            return returnPlaylist;
            
        }
        else {
           
            *timePlaylist=sumTime;
            *lenPlaylist=i;
            songs * returnPlaylist= (songs *)malloc(sizeof(songs)*i);
            for (int j = 0; j < i; j++)
            {
                returnPlaylist[j]=sameGenre[j];
            }
            return returnPlaylist;
        }
    }
    //Si no supera el limite requerido, se retornan todas
    //las canciones de un mismo genero
    else {
        *lenPlaylist=amountSongs;
        *timePlaylist=totalSeconds;
        return sameGenre;}
     
}


/*Funcion driver*/

int main(){
    //////////////////////////////////////////////
    int num=0;
	int fiboi=0;
	int fibor=0;
	time_t tiniIt, tfinIt, tiniRe, tfinRe;
	clock_t ciniIt, cfinIt, ciniRe, cfinRe;
    ciniIt=clock();
	//inicio tiempo




    //Generacion de variables a utilizar
    char lista[]="listado.in";
    char play[]="play.in";
    int genrePlayList=0;
    char *strGenre;
    int totalSecondsPlaylist;
    int lenPlaylist=0;
    int len = fileSize(lista);
    //Invocacion de funciones
    songs *dataList= openFile(len,lista);
    playContent * dataPlaylist=playCapture(len,play,&genrePlayList);
    songs* playlist=makePlayList(genrePlayList,dataList,len,dataPlaylist[0].listDuration, &lenPlaylist,&totalSecondsPlaylist);
    //Guardado
    toHours(totalSecondsPlaylist,lenPlaylist,dataPlaylist[genrePlayList-1].listGenre, playlist);
    printf("Playlist generada con exito en el archivo playlist.out\n");
    free(dataPlaylist);
    free(dataList);
    free(playlist);
    //////////////////////////////////////////////
  
    cfinIt=clock();
	printf("Tiempo utilizado generacion playList:  %f segundos.\n",(double)(cfinIt-ciniIt)/CLOCKS_PER_SEC);
    return 0;
}