#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_header.h"
#include "cJSON.h"
#define BUFFER_SIZE 261140
#define MAP_SIZE 73

typedef struct
{
    int R , G , B ;
} pixel ;

FILE * open_file( char task[6] , char file[50] )
{
    char *name_file = malloc( 50 * sizeof(char));
    strcpy( name_file , "output_"  );
    char *board_nr = calloc( 3 , sizeof( char  ) ) , *p ;

    if( strcmp ( task , "bonus" ) != 0  )
        strcat( name_file , "task" );

    strcat( name_file , task );

    strcat( name_file ,"_board" );
    
    p = strstr( file , "board" );
    int n = p - file  ;
    
    board_nr[ 0 ] = file[ n + 5] , board_nr[ 1 ]= file[ n + 6 ] , board_nr[ 2 ] = 0 ;
    
    strcat( name_file , board_nr );

    if( task[ 0 ] == '3' )
        {
         strcat( name_file , ".json" );
         
         return fopen( name_file , "wt");   
        }
    else
        {
        strcat( name_file , ".bmp");
    
        return fopen( name_file , "wb");   
        }
    free( name_file );
    free( board_nr );
}

void print_bmp( FILE *out , bmp_fileheader *file_h , bmp_infoheader  *info_h , pixel **map )
{   
   
   int i , j ;
   char zero = 0;
   //printam file_headerul
   fwrite( &file_h -> fileMarker1 , sizeof( char ) , 1 , out  );
   fwrite( &file_h -> fileMarker2 , sizeof( char ) , 1 , out  );
   fwrite( &file_h -> bfSize , sizeof( unsigned int ) , 1 , out  );
   fwrite( &file_h -> unused1 , sizeof( unsigned short ) , 1 , out  );
   fwrite( &file_h -> unused1 , sizeof( unsigned short ) , 1 , out  );
   fwrite( &file_h ->imageDataOffset , sizeof( unsigned int ) , 1 , out  );

  // printam info_headerul
   fwrite( &info_h ->biSize , sizeof( unsigned int ) , 1 , out  );
   fwrite( &info_h ->width , sizeof( int ) , 1 , out  );
   fwrite( &info_h ->height , sizeof( int ) , 1 , out  );
   fwrite( &info_h ->planes , sizeof( unsigned short ) , 1 , out  );
   fwrite( &info_h ->bitPix , sizeof( unsigned short ) , 1 , out  );
   fwrite( &info_h ->biCompression , sizeof( unsigned int ) , 1 , out  );
   fwrite( &info_h ->biSizeImage , sizeof( unsigned int ) , 1 , out  );
   fwrite( &info_h ->biXPelsPerMeter , sizeof( int ) , 1 , out  );
   fwrite( &info_h ->biYPelsPerMeter , sizeof( int ) , 1 , out  );
   fwrite( &info_h ->biClrUsed , sizeof( unsigned int ) , 1 , out  );
   fwrite( &info_h ->biClrImportant , sizeof( unsigned int ) , 1 , out  );

   //printam bit_mapul
   for( i = info_h->height - 1 ; i >= 0 ; i-- )
    {
    for( j = 0 ; j < info_h ->width ; j++ )
        {
         fwrite( &map[ i ][ j ].R , sizeof( char ) , 1 , out  );
         fwrite( &map[ i ][ j ].G , sizeof( char ) , 1 , out  );
         fwrite( &map[ i ][ j ].B , sizeof( char ) , 1 , out  );   
        }
    //printam paddingul 
     fwrite( &zero , sizeof( char ) , 1 , out );
    }
    fclose( out );
}

void Do_task_1( FILE * in , FILE * out , bmp_fileheader *file_h , bmp_infoheader  *info_h , pixel **map )
{
  char *buffer;
  buffer = malloc( BUFFER_SIZE * sizeof( char ) ) ;
  buffer[ strlen( buffer ) ] = 0 ;
  cJSON *signature , *file_size , *reserved , *offset , *size , *width , *bit ;
  cJSON *height , *planes , *bit_count , *compression , *image_size , *x , *y ;
  cJSON *parse , *colors_used , *colors_important , *f_header , *i_header , *bit_map ; 
  char *s = malloc( 10 * sizeof( char ) );
  int ct = 0 , nr , lin , col ;
  //citirea buffer
  fread(buffer, BUFFER_SIZE , 1, in);
  fclose( in );
  
  //parsarea bufferului
  parse = cJSON_Parse( buffer );
  free ( buffer );
  //preluam informatiile din file_header
  f_header = cJSON_GetObjectItemCaseSensitive( parse , "file_header" ); 

  offset =  cJSON_GetObjectItemCaseSensitive( f_header , "offset" );
  file_h->imageDataOffset = offset -> valueint ;

  signature =  cJSON_GetObjectItemCaseSensitive( f_header , "signature" );
  strcpy( s , signature -> valuestring );
  file_h->fileMarker1 = s[ 0 ];
  file_h->fileMarker2 = s[ 1 ];
  free( s );
  reserved =  cJSON_GetObjectItemCaseSensitive( f_header , "reserved" );
  file_h->unused1 = reserved -> valueint ;

  file_size =  cJSON_GetObjectItemCaseSensitive( f_header , "file_size" );
  file_h->bfSize = file_size -> valueint ;
  
  //preluam informatile din infoheader
  i_header = cJSON_GetObjectItemCaseSensitive( parse , "info_header" ); 
  
  colors_used = cJSON_GetObjectItemCaseSensitive( i_header , "colors_used" );
  info_h->biClrUsed = colors_used -> valueint ;

  size = cJSON_GetObjectItemCaseSensitive( i_header , "size" );
  info_h->biSize = size -> valueint;

  colors_important = cJSON_GetObjectItemCaseSensitive( i_header , "colors_important" );
  info_h->biClrImportant = colors_important -> valueint ;

  image_size  = cJSON_GetObjectItemCaseSensitive( i_header , "image_size" );
  info_h->biSizeImage = image_size -> valueint ;

  y = cJSON_GetObjectItemCaseSensitive( i_header , "y_pixels_per_meter" );
  info_h->biYPelsPerMeter = y -> valueint ;

  x = cJSON_GetObjectItemCaseSensitive( i_header , "x_pixels_per_meter" );
  info_h->biXPelsPerMeter = x -> valueint ;
   
  width = cJSON_GetObjectItemCaseSensitive( i_header , "width" );
  info_h->width = width -> valueint ;

  planes = cJSON_GetObjectItemCaseSensitive( i_header , "planes" );
  info_h->planes = planes -> valueint ;

  bit_count = cJSON_GetObjectItemCaseSensitive( i_header , "bit_count" );
  info_h->bitPix = bit_count -> valueint ;

  compression = cJSON_GetObjectItemCaseSensitive( i_header , "compression" );
  info_h->biCompression = compression -> valueint ;

  height = cJSON_GetObjectItemCaseSensitive( i_header , "height" );
  info_h->height = height -> valueint ;
  
  //citim mapa de pixeli
  bit_map = cJSON_GetObjectItemCaseSensitive( parse , "bitmap" );
  cJSON_ArrayForEach(bit , bit_map )
  {
   
    nr = ct / 3 ;
    
    lin = nr / info_h->height ;
    col = nr % info_h->width ; 
    if( ct % 3 == 0 )
        map[ lin ][ col ].R = bit -> valueint ;  
    if( ct % 3 == 1 )
        map[ lin ][ col ].G = bit -> valueint ;    
    if( ct % 3 == 2 )
        map[ lin ][ col ].B = bit -> valueint ; 
    ct ++ ;
  }
    //printf( "%d %d %d " , map[ 72 ][ 72 ].R , map[ 72 ][ 72 ].G , map[ 72 ][ 72 ].B );
   //cream imaginea bmp
   
   cJSON_Delete( parse );

   print_bmp( out , file_h , info_h , map );
 

}

void swap( pixel *a , pixel *b )
{
    pixel *c ;
    c = malloc ( sizeof( pixel) );
    // a = c  
    c->R = a->R ;
    c->G = a->G ;
    c->B = a->B ;
    // a = b
    a->R = b->R ;
    a->G = b->G ;
    a->B = b->B ;
    // b = c
    b->R = c->R ;
    b->G = c->G ;
    b->B = c->B ;
    free( c );
}

void rotate_sqr( bmp_fileheader *file_h , bmp_infoheader  *info_h , pixel **map , int x , int y  )
{
    int i , j ;
    for( i = 0 ; i < 7 ; i++ )
        for( j = 0 ; j < 3 ; j++  )
            swap( &map[ x + i ][ y + j ] , &map[ x + i ][ y  + 6 - j ]);
}

void Do_task_2( FILE * out , bmp_fileheader *file_h , bmp_infoheader  *info_h , pixel **map )
{
 int i , j ;
 for( i = 1 ; i < info_h -> height ; i += 8 )
    for( j = 1 ; j < info_h -> width ; j+= 8 )
        rotate_sqr( file_h , info_h , map , i , j );
 print_bmp( out , file_h , info_h , map );
}
//returneaza suma dintre culori pentru a afla rapid daca un pixel este alb
int color( pixel **map , int x , int y )
{
    return map[ x ][ y ].G + map[ x ][ y ].R + map[ x ][ y ].B;
}

//recunoasterea unui numar format din pixeli 
int nr_sqr(  pixel **map , int x , int y  )
{   
    if( color( map , x + 5 , y + 5 ) == 765 )
        return 0 ;
    if( color( map , x + 4 , y + 5 ) == 765 )
        return 2 ;
    if( color( map , x + 1 , y + 5 ) != 765 && color( map , x + 1 , y + 1 ) == 765 )
        return 1 ;
    if( color( map , x + 1 , y + 5 ) != 765 && color( map , x + 1 , y + 1 ) != 765 )
        if( color( map , x + 1 , y + 2 ) == 765)
            return 4 ;
    if( color( map , x + 2 , y + 5 ) == 765  )
        if( color( map , x + 4 , y + 1 ) == 765 )
            return 5 ;
        else 
            return 6 ;

    if( color( map , x + 3 , y + 1 ) != 765  )
        if( color( map , x + 2 , y + 1 ) == 765 )
            return 3 ;
        else 
            if( color( map , x + 4 , y + 1 ) == 765 )
                return 9 ;
            else 
                return 8 ;
    if( color( map , x + 1 , y + 5 ) != 765  )
        return 7 ;
    
}

//returneaza numarul patratului de 3X3 din care apartine un numar
int cadran( int l, int c )
{
    if ( l <= 3 )
        { if ( c <= 3 ) return 1;
          if ( c > 3 && c <= 6 ) return 2;
          if ( c > 6 && c <= 9 ) return 3;
        }
    if ( l > 3 && l <= 6 )
        { if ( c <= 3 ) return 4;
          if ( c > 3 && c <= 6 ) return 5;
          if ( c > 6 && c <= 9 ) return 6;
        }
    if ( l > 6 && l <= 9 )
        { if ( c <= 3 ) return 7;
          if ( c > 3 && c <= 6 ) return 8;
          if ( c > 6 && c <= 9 ) return 9;
        }
}
void Do_task_3( FILE * out , bmp_fileheader *file_h , bmp_infoheader  *info_h , pixel **map , char file[ 50 ] )
{
  int i , j , I , J , **sudoku , ok ;
  int **lin, **col, **cad;
  char *text , *p ;
  text = malloc( 10 * sizeof( char ) );
  sudoku = malloc( 10 * sizeof( int * ) );
  lin = malloc( 10 * sizeof( int * ) );
  col = malloc( 10 * sizeof( int * ) );
  cad = malloc( 10 * sizeof( int * ) );
  for( i = 0 ; i< 10 ; i++ )
    {
        cad[ i ] = malloc( 10 * sizeof( int ) ) ;  
        sudoku[ i ] = malloc( 10 * sizeof( int ) ) ; 
        lin[ i ] = malloc( 10 * sizeof( int ) ) ; 
        col[ i ] = malloc( 10 * sizeof( int ) ) ; 
    }

  
 //preluam numerele din pixel map si le punem in sudoku
 for( I = 1 , i = 1 ; i < info_h -> height ; i += 8 , I ++ )
    for( j = 1 , J = 1  ; j < info_h -> width ; j+= 8 , J ++ )
        {sudoku[ I ][ J ] = nr_sqr( map ,  i , j ); 
        }

 //completami vectorii de frecventa
 for( i = 1 ; i <= 9 ; i++ )
    for( j = 1 ; j <= 9 ; j++ )
        {
            lin[ i ][ sudoku [ i ][ j ] ] = 1 ;
            col[ j ][ sudoku [ i ][ j ] ] = 1 ;
            cad[ cadran( i , j ) ][ sudoku [ i ][ j ] ] = 1 ;
        }
 ok = 0 ; 
 //verfificam cu ajutorul vectorilor de frecventa
 for( i = 1 ; i <= 9 && ok == 0 ; i++ )
    for( j = 1 ; j <= 9 && ok == 0  ; j++ )
    {
        if( lin[ i ][ j ] != 1 )
             ok = 1 ;
        if( col[ i ][ j ] != 1 )
             ok = 1 ;     
        if( cad[ i ][ j ] != 1 )
             ok = 1 ;
    }
    p = strstr( file , "board" );
    if( ok == 0 )
        strcpy( text , "Win!");
    else
        strcpy( text , "Loss :(");

    fprintf( out , "{\n" );
    fprintf( out , "\t\"input_file\":\t\"%s\",\n" , p );
    fprintf( out , "\t\"game_state\":\t\"%s\"\n}\n" , text );

    fclose( out );
    for( i = 0 ; i< 10 ; i++ )
    {
        free( cad[ i ] );  
        free( lin[ i ] );  
        free( col[ i ] );   
        free( sudoku[ i ] );   
    }
    free ( cad );
    free ( sudoku );
    free ( col );
    free ( lin );
    free( text );
}

void read (FILE * in , bmp_fileheader *file_h , bmp_infoheader  *info_h , pixel **map )
{   
    int i , j ;
    int a = 0  ;
    //citim file_headerul
    fread( &file_h->fileMarker1 , sizeof( unsigned char ) , 1 , in  );
    fread( &(file_h->fileMarker2) , sizeof( unsigned char ) , 1 , in  );
    fread( &(file_h->bfSize) , sizeof( unsigned int ) , 1 , in  );
    fread( &(file_h->unused1) , sizeof( unsigned short ) , 1 , in  );
    fread( &(file_h->unused2) , sizeof( unsigned short ) , 1 , in  );
    fread( &(file_h->imageDataOffset) , sizeof( unsigned int ) , 1 , in  );

    //citim info_headerl
    fread( &info_h->biSize , sizeof( unsigned int ) , 1 , in );
    fread( &info_h->width , sizeof( signed int ) , 1 , in );
   
    fread( &info_h->height , sizeof( int ) , 1 , in );
    fread( &info_h->planes , sizeof( unsigned short ) , 1 , in );
    fread( &info_h->bitPix , sizeof( unsigned short ) , 1 , in );
    fread( &info_h->biCompression , sizeof( unsigned int ) , 1 , in );
    fread( &info_h->biSizeImage , sizeof( unsigned int ) , 1 , in );
    fread( &info_h->biXPelsPerMeter , sizeof( int ) , 1 , in );
    fread( &info_h->biYPelsPerMeter , sizeof( int ) , 1 , in );
    fread( &info_h->biClrUsed , sizeof( unsigned int ) , 1 , in );
    fread( &info_h->biClrImportant , sizeof( unsigned int ) , 1 , in );

    //citim matricea de pixeli
    for( i = MAP_SIZE -1 ; i >= 0 ; i-- )
        {
         for( j = 0 ; j < MAP_SIZE ; j++ )
            {
                fread( &map[ i ][ j ].R , sizeof( char ) , 1 , in );
                fread( &map[ i ][ j ].G , sizeof( char ) , 1 , in );
                fread( &map[ i ][ j ].B , sizeof( char ) , 1 , in );
            }
          fread( &a , sizeof( char ) , 1 , in );  
        }
    
}

//pune solutia in vectorul sol
void put_sol( int **board , int **sol)
{   
    int i , j ;
    for( i = 1 ; i <= 9 ; i++ )
       {
        for( j = 1 ; j<= 9 ; j++ )
           { sol[ i ][ j ] = board[ i ][ j ];
            printf( "%d " , sol[ i ][ j ] );
           }
         printf( "\n" );
       }
}

//backtracking pentru compleetarea matricei de sudoku
void back( int **board , int x , int y , int **sol , int **lin , int **col , int **cad )
{   
    int i , n = cadran( x , y )  ;
    //daca gasim un patratel gol
    if( board[ x ][ y ] == 0 )
    {   
        //cautam un numar 
        for( i = 1 ; i <= 9 ; i++ )
            {
                board[ x ][ y ] = i ;
                //daca este un numar valid
                if( lin[ x ][ i ] == 0 && col[ y ][ i ] == 0 && cad[ n ][ i ] == 0 )
                    {
                      lin[ x ][ i ] = 1 ; 
                      col[ y ][ i ] = 1 ;
                      cad[ n ][ i ] = 1 ;
                      
                      //daca este solutia finala
                      if( x == 9 && y == 9 )
                        put_sol( board , sol );  
                      else
                        {
                          if( y == 9 )
                            back( board , x + 1 , 1 , sol , lin , col , cad );
                          else 
                            back( board , x , y + 1 , sol , lin , col , cad );   
                        }

                      lin[ x ][ i ] --; 
                      col[ y ][ i ] --;
                      cad[ n ][ i ] --;  
                    }
                board[ x ][ y ] = 0 ;
            }
    }
    //daca patratul este deja completat
    else
    {
    if( x == 9 && y == 9 )
            put_sol( board , sol );  
    else
        {
         if( y == 9 )
            back( board , x + 1 , 1 , sol , lin , col , cad );
         else 
            back( board , x , y + 1 , sol , lin , col , cad );

        }  
    }

    

}

//seteaza un pixel de culoarea roz
void set_color( int x , int y , pixel **map )
{  
    map[ x ][ y ].R = 255 ;
    map[ x ][ y ].G = 0 ;
    map[ x ][ y ].B = 255 ;
}

//ppune un numar in matricea de pixeli
void put_nr( int x , int y , int nr , pixel **map )
{
    if( nr == 1 )
    {
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );   
     set_color( x + 4 , y + 5 , map );
     set_color( x + 5 , y + 5 , map );
    }
    if( nr == 2 )
    {
     set_color( x + 1 , y + 1 , map );
     set_color( x + 1 , y + 2 , map );
     set_color( x + 1 , y + 3 , map );   
     set_color( x + 1 , y + 4 , map );
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 3 , y + 1 , map );
     set_color( x + 3 , y + 2 , map );
     set_color( x + 3 , y + 3 , map );   
     set_color( x + 3 , y + 4 , map );
     set_color( x + 4 , y + 1 , map );
     set_color( x + 5 , y + 1 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 5 , y + 2 , map );
     set_color( x + 5 , y + 3 , map );
     set_color( x + 5 , y + 4 , map );   
     set_color( x + 5 , y + 5 , map );
    }
    if( nr == 3 )
     {
     set_color( x + 1 , y + 1 , map );
     set_color( x + 1 , y + 2 , map );
     set_color( x + 1 , y + 3 , map );   
     set_color( x + 1 , y + 4 , map );
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 3 , y + 1 , map );
     set_color( x + 3 , y + 2 , map );
     set_color( x + 3 , y + 3 , map );   
     set_color( x + 3 , y + 4 , map );
     set_color( x + 4 , y + 5 , map );
     set_color( x + 5 , y + 1 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 5 , y + 2 , map );
     set_color( x + 5 , y + 3 , map );
     set_color( x + 5 , y + 4 , map );   
     set_color( x + 5 , y + 5 , map );
    }
    if( nr == 4 )
    {
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );   
     set_color( x + 4 , y + 5 , map );
     set_color( x + 5 , y + 5 , map );
     set_color( x + 1 , y + 1 , map );
     set_color( x + 2 , y + 1 , map );
     set_color( x + 3 , y + 1 , map );
     set_color( x + 3 , y + 2 , map );
     set_color( x + 3 , y + 3 , map );
     set_color( x + 3 , y + 4 , map );
    }
    if( nr == 5 )
    {
     set_color( x + 1 , y + 1 , map );
     set_color( x + 1 , y + 2 , map );
     set_color( x + 1 , y + 3 , map );   
     set_color( x + 1 , y + 4 , map );
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 1 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 3 , y + 1 , map );
     set_color( x + 3 , y + 2 , map );
     set_color( x + 3 , y + 3 , map );   
     set_color( x + 3 , y + 4 , map );
     set_color( x + 4 , y + 5 , map );
     set_color( x + 5 , y + 5 , map );
     set_color( x + 5 , y + 1 , map );
     set_color( x + 5 , y + 2 , map );
     set_color( x + 5 , y + 3 , map );
     set_color( x + 5 , y + 4 , map );   
     set_color( x + 5 , y + 5 , map );
    }
    if( nr == 6 )
    {
     set_color( x + 1 , y + 1 , map );
     set_color( x + 1 , y + 2 , map );
     set_color( x + 1 , y + 3 , map );   
     set_color( x + 1 , y + 4 , map );
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 1 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 3 , y + 1 , map );
     set_color( x + 3 , y + 2 , map );
     set_color( x + 3 , y + 3 , map );   
     set_color( x + 3 , y + 4 , map );
     set_color( x + 4 , y + 1 , map );
     set_color( x + 4 , y + 5 , map );
     set_color( x + 5 , y + 5 , map );
     set_color( x + 5 , y + 1 , map );
     set_color( x + 5 , y + 2 , map );
     set_color( x + 5 , y + 3 , map );
     set_color( x + 5 , y + 4 , map );   
     set_color( x + 5 , y + 5 , map );
    }
    if( nr == 7 )
    {
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );   
     set_color( x + 4 , y + 5 , map );
     set_color( x + 5 , y + 5 , map );
     set_color( x + 1 , y + 1 , map );
     set_color( x + 1 , y + 2 , map );   
     set_color( x + 1 , y + 3 , map );
     set_color( x + 1 , y + 4 , map );
    }
    if( nr == 8 )
    {
     set_color( x + 1 , y + 1 , map );
     set_color( x + 1 , y + 2 , map );
     set_color( x + 1 , y + 3 , map );   
     set_color( x + 1 , y + 4 , map );
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 1 , map );
     set_color( x + 2 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 3 , y + 1 , map );
     set_color( x + 3 , y + 2 , map );
     set_color( x + 3 , y + 3 , map );   
     set_color( x + 3 , y + 4 , map );
     set_color( x + 4 , y + 1 , map );
     set_color( x + 4 , y + 5 , map );
     set_color( x + 5 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 5 , y + 1 , map );
     set_color( x + 5 , y + 2 , map );
     set_color( x + 5 , y + 3 , map );
     set_color( x + 5 , y + 4 , map );   
     set_color( x + 5 , y + 5 , map );
    }
    if( nr == 9 )
    {
     set_color( x + 1 , y + 1 , map );
     set_color( x + 1 , y + 2 , map );
     set_color( x + 1 , y + 3 , map );   
     set_color( x + 1 , y + 4 , map );
     set_color( x + 1 , y + 5 , map );
     set_color( x + 2 , y + 1 , map );
     set_color( x + 2 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 3 , y + 1 , map );
     set_color( x + 3 , y + 2 , map );
     set_color( x + 3 , y + 3 , map );   
     set_color( x + 3 , y + 4 , map );
     set_color( x + 4 , y + 5 , map );
     set_color( x + 5 , y + 5 , map );
     set_color( x + 3 , y + 5 , map );
     set_color( x + 5 , y + 2 , map );
     set_color( x + 5 , y + 3 , map );
     set_color( x + 5 , y + 4 , map );   
     set_color( x + 5 , y + 1 , map );
    }
}

void Do_task_4( FILE * in , FILE * out , bmp_fileheader *file_h , bmp_infoheader  *info_h , pixel **map )
{   
    int **lin, **col, **cad , **sudoku , i , j , I , J  , ok , **sol , **copy ;
    sudoku = malloc( 10 * sizeof( int * ) );
    lin = malloc( 10 * sizeof( int * ) );
    sol = malloc( 10 * sizeof( int * ) );
    copy = malloc( 10 * sizeof( int * ) );
    col = malloc( 10 * sizeof( int * ) );
    cad = malloc( 10 * sizeof( int * ) );
    for( i = 0 ; i< 10 ; i++ )
    {
        cad[ i ] = malloc( 10 * sizeof( int ) ) ;
        copy[ i ] = malloc( 10 * sizeof( int ) ) ;  
        sudoku[ i ] = malloc( 10 * sizeof( int ) ) ; 
        lin[ i ] = calloc( 10 , sizeof( int ) ) ; 
        col[ i ] = calloc( 10 , sizeof( int ) ) ; 
        sol[ i ] = calloc( 10 , sizeof( int ) ) ;
    }

    //citim imaginea bmp
    read( in  , file_h ,  info_h , map);
    
    //preluam numerele din harta de biti si le punem in sudoku
    for( I = 1 , i = 1 ; i < info_h -> height ; i += 8 , I ++ )
        for( j = 1 , J = 1  ; j < info_h -> width ; j+= 8 , J ++ )
            {   
                //recunoastem numarul
                sudoku[ I ][ J ] = nr_sqr( map ,  i , j );
                //creem o copie
                copy[ I ][ J ] = sudoku[ I ][ J ];
            } 
    //completam vectorii de frecventa cu datele necompletate
    for( i = 1 ; i <= 9  ; i++ )
        for( j = 1 ; j <= 9  ; j++ )
             {
                lin[ i ][ sudoku [ i ][ j ] ] ++ ;
                col[ j ][ sudoku [ i ][ j ] ] ++ ;
                cad[ cadran( i , j ) ][ sudoku [ i ][ j ] ] ++ ;
             } 
    //backtracking pentru a gasi solutia 
    back( sudoku , 1 , 1 , sol , lin , col , cad );

    //daca solutia finala nu ramane necompletat
    if( sol[ 1 ][ 1 ] != 0 )
    {
     for( I = 1 , i = 1 ; i < info_h -> height ; i += 8 , I ++ )
        for( j = 1 , J = 1  ; j < info_h -> width ; j+= 8 , J ++ ) 
           if( copy[ I ][ J ] == 0 )
            {
              //facem update la matricea de biti prin adaugarea numerelor 
              put_nr( i , j , sol[ I ][ J ] , map ); 
            }
    }

    //afisam matricea
    print_bmp( out , file_h , info_h , map );
    for( i = 0 ; i< 10 ; i++ )
    {
        free( cad[ i ] );  
        free( lin[ i ] );  
        free( col[ i ] );   
        free( sudoku[ i ] );
        free( sol[ i ] );  
        free( copy[ i ] ); 
    }
    free( copy );
    free ( cad );
    free ( sudoku );
    free ( col );
    free ( lin ); 
    free( sol );       
}

void clear_board( pixel **map )
{
  int i , j , I , J ;
  for( i = 1 ; i < 73 ; i += 8  )
    for( j = 1   ; j < 73 ; j+= 8  )  
        for( I = 1; I <= 5 ; I++ )
            for( J = 1; J <= 5 ; J++ )
            {
              map[ i + I ][ j + J ].R = 255; 
              map[ i + I ][ j + J ].G = 255;
              map[ i + I ][ j + J ].B = 255; 
            }
}

void x_board( pixel **map )
{
  int i , j , I , J ;
  for( i = 1 ; i < 73 ; i += 8  )
    for( j = 1   ; j < 73 ; j+= 8  )  
        for( I = 1; I <= 5 ; I++ )
            {
              map[ i + I ][ j + I ].R = 0; 
              map[ i + I ][ j + I ].G = 0;
              map[ i + I ][ j + I ].B = 255;
              map[ i + I ][ j + 6 - I ].R = 0;
              map[ i + I ][ j + 6 - I ].G = 0;
              map[ i + I ][ j + 6 - I ].B = 255;
              
            }
}

void Do_bonus( FILE * in , FILE * out , bmp_fileheader *file_h , bmp_infoheader  *info_h , pixel **map )
{   
    int **lin, **col, **cad , **sudoku , i , j , I , J  , ok , **sol , **copy ;
    sudoku = malloc( 10 * sizeof( int * ) );
    lin = malloc( 10 * sizeof( int * ) );
    sol = malloc( 10 * sizeof( int * ) );
    copy = malloc( 10 * sizeof( int * ) );
    col = malloc( 10 * sizeof( int * ) );
    cad = malloc( 10 * sizeof( int * ) );
    for( i = 0 ; i< 10 ; i++ )
    {
        cad[ i ] = malloc( 10 * sizeof( int ) ) ;
        copy[ i ] = malloc( 10 * sizeof( int ) ) ;  
        sudoku[ i ] = malloc( 10 * sizeof( int ) ) ; 
        lin[ i ] = calloc( 10 , sizeof( int ) ) ; 
        col[ i ] = calloc( 10 , sizeof( int ) ) ; 
        sol[ i ] = calloc( 10 , sizeof( int ) ) ;
    }

    //citim un fisier de tip bmp
    read( in  , file_h ,  info_h , map);
    
    //preluam numerele din imagine
    for( I = 1 , i = 1 ; i < info_h -> height ; i += 8 , I ++ )
        for( j = 1 , J = 1  ; j < info_h -> width ; j+= 8 , J ++ )
            {
                sudoku[ I ][ J ] = nr_sqr( map ,  i , j );
                copy[ I ][ J ] = sudoku[ I ][ J ];
            } 
    
    //completam matricile de frecventa cu datele curente
    for( i = 1 ; i <= 9  ; i++ )
        for( j = 1 ; j <= 9  ; j++ )
             {
                lin[ i ][ sudoku [ i ][ j ] ] ++ ;
                col[ j ][ sudoku [ i ][ j ] ] ++ ;
                cad[ cadran( i , j ) ][ sudoku [ i ][ j ] ] ++ ;
             } 
    
    //backtracking pentru gasirea unei solutii
    back( sudoku , 1 , 1 , sol , lin , col , cad );

    //daca gasim o solutie finala
    if( sol[ 1 ][ 1 ] != 0 )
    {
     for( I = 1 , i = 1 ; i < info_h -> height ; i += 8 , I ++ )
        for( j = 1 , J = 1  ; j < info_h -> width ; j+= 8 , J ++ ) 
           if( copy[ I ][ J ] == 0 )
            
              put_nr( i , j , sol[ I ][ J ] , map );  
        
    }
    //daca nu gasim o solutie
    else
    {   
        //facem tabla alba 
        clear_board( map );
        //punem x-uri in toate patratelele
        x_board( map );
    }

    //afisam matricea
    print_bmp( out , file_h , info_h , map );

    for( i = 0 ; i< 10 ; i++ )
    {
        free( cad[ i ] );  
        free( lin[ i ] );  
        free( col[ i ] );   
        free( sudoku[ i ] );
        free( sol[ i ] );  
        free( copy[ i ] ); 
    }
    free( copy );
    free ( cad );
    free ( sudoku );
    free ( col );
    free ( lin ); 
    free( sol );       
}




int main( int argc , char *argv[] )
{   
    FILE *IN , *OUT1 , *OUT2 , *OUT3 , *OUT4 , *OUTBonus;
    bmp_fileheader *file_h ;
    file_h = malloc( sizeof( bmp_fileheader ) );
    bmp_infoheader *info_h ;
    info_h = calloc( 1 , sizeof( bmp_infoheader ) );
    pixel **map ;
    int i ;
    //alocam harta de pixeli
    map = malloc( MAP_SIZE * sizeof( pixel * ) ) ;
    for( i = 0 ; i < MAP_SIZE ; i++  )
        map[ i ] = malloc( MAP_SIZE * sizeof( pixel ) );
    //char iesire[] = "output_task" + argv[ 2 ] + "board" + nr_board( argv[ 1 ] ) + ".bmp" ;
    //cream fisierele de iesire de care avem nevoie

    if( strcmp( argv[ 2 ] , "123" ) == 0 ) 
    {   
        IN = fopen ( argv[ 1 ] , "rt");
        OUT1 = open_file( "1" , argv[ 1 ] );
        Do_task_1( IN , OUT1 , file_h , info_h , map );
        OUT2 = open_file( "2" , argv[ 1 ] );
        Do_task_2( OUT2 , file_h , info_h , map );
        OUT3 = open_file( "3" , argv[ 1 ] );
        Do_task_3( OUT3 , file_h , info_h , map , argv[ 1 ] );
        
    }
    if( strcmp( argv[ 2 ] , "bonus" ) == 0 ) 
        {
         IN = fopen ( argv[ 1 ] , "rb");
         OUTBonus = open_file( "bonus" , argv[ 1 ] );
         Do_bonus( IN , OUTBonus , file_h , info_h , map );
        }
    if( strcmp( argv[ 2 ] , "4" ) == 0 ) 
       { 
        
        IN = fopen ( argv[ 1 ] , "rb");
        OUT4 = open_file( "4" , argv[ 1 ]);
        Do_task_4( IN , OUT4 , file_h , info_h , map );

       }
    //eliberam memoria alocata
    for( i = 0 ; i < MAP_SIZE ; i++)
        free( map[ i ] );

    free( map );
    free( info_h );
    free( file_h );
    return 0 ;
}