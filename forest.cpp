#include <iostream>
#include <mpi.h>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
using namespace std;

#define MASTER 0

const int dim = 100; 

// g++ Foresta.cpp -lallegro -lallegro_primitives -lallegro_main

enum Stato {ALBERO = 0, TERRA = 1, BRUCIA1 = 2, BRUCIA2 = 3, BRUCIA3 = 4, ALBEROPIOGGIA=5, TERRAPIOGGIA = 6, BRUCIAPIOGGIA=7};

const double probAlbero= 0.02; // probabilità nascita albero (p)
const double probIncendio = 0.00002; // probabilità incendio (f)
const double probPioggia= 0.00008; // probabilità pioggia (q)

// Allegro
ALLEGRO_DISPLAY * display;
const int larghezza = 800;
const int altezza = 800;
const int latoQuadrato = 8;



void generaNuvola(int* supportoSub,int* sottoMatrice, int i,int j, int *vettoriSu,int *vettoriGiu,int dimLocale)
{ 
    if(i==0){
        if (vettoriSu[j]==TERRA)
            vettoriSu[j]=TERRAPIOGGIA;
        else if (vettoriSu[j]==ALBERO)
            vettoriSu[j]=ALBEROPIOGGIA;
        else if (vettoriSu[j]==BRUCIA1 || vettoriSu[j]==BRUCIA2 || vettoriSu[j]==BRUCIA3)
            vettoriSu[j]=BRUCIAPIOGGIA;
    }
    else if (i*dim+j>0 && i*dim+j<((dim*dimLocale)-dim)){
        if (sottoMatrice[i*dim+j]==TERRA){
            supportoSub[i*dim+j]=TERRAPIOGGIA;
            supportoSub[i*dim+j-1]=TERRAPIOGGIA;
        }
        else if (sottoMatrice[i*dim+j]==ALBERO){
            supportoSub[i*dim+j]=ALBEROPIOGGIA;
            supportoSub[i*dim+j-1]=ALBEROPIOGGIA;

        }
        else if (sottoMatrice[i*dim+j]==BRUCIA1 || sottoMatrice[i*dim+j]==BRUCIA2 || sottoMatrice[i*dim+j]==BRUCIA3){
            supportoSub[i*dim+j]=BRUCIAPIOGGIA;
            supportoSub[i*dim+j-1]=BRUCIAPIOGGIA;

        }
        
    }
    else if (i*dim+j==dimLocale*dim-dim){
    	if (vettoriGiu[j]==TERRA)
            vettoriGiu[j]=TERRAPIOGGIA;
        else if (vettoriGiu[j]==ALBERO)
            vettoriGiu[j]=ALBEROPIOGGIA;
        else if (vettoriGiu[j]==BRUCIA1 || vettoriGiu[j]==BRUCIA2 || vettoriGiu[j]==BRUCIA3)
            vettoriGiu[j]=BRUCIAPIOGGIA;
    }
}



bool controlla_VicinoAlbero(int *sottoMatrice, int x, int y,int dimLocale){
    
	
    for (int riga= x-1; riga<=x+1;riga++)
        {
        	if (riga>=0 && riga<dimLocale)
        	 for (int colonna=y-1; colonna<=y+1; colonna++)
                {	
                	if (sottoMatrice[riga*dim+colonna]==BRUCIA1 && (riga!=x || colonna!=y)  && colonna>=0 && colonna<dim)
                        return true;

                }
            
           
         
        }
    return false;
}


void disegnaConAllegro(int* world) {
    al_clear_to_color(al_map_rgb(0, 0, 0)); 
    for (unsigned i = 0; i < dim; i++)
    {
        for (unsigned j = 0; j < dim; j++)
        {
            if( world[i*dim+j]==TERRAPIOGGIA ||	world[i*dim+j]==ALBEROPIOGGIA ||world[i*dim+j]==BRUCIAPIOGGIA)
            {
                al_draw_filled_rectangle(i * latoQuadrato, j * latoQuadrato, i * latoQuadrato + latoQuadrato,
                        j * latoQuadrato + latoQuadrato, al_map_rgb(171, 205, 239)); // celeste chiaro
            }
            else{
                switch (world[i*dim+j]) {
                case 0: // albero
                    al_draw_filled_rectangle(i * latoQuadrato, j * latoQuadrato,
                            i * latoQuadrato + latoQuadrato,
                            j * latoQuadrato + latoQuadrato, al_map_rgb(0,200,0)); // verde
                    break;
                case 1:  // terra
                    al_draw_filled_rectangle(i * latoQuadrato, j * latoQuadrato,
                            i * latoQuadrato + latoQuadrato,
                            j * latoQuadrato + latoQuadrato, al_map_rgb(128, 98, 16)); // marroncino
                    break;
               
                case 2: // brucia1
                    al_draw_filled_rectangle(i * latoQuadrato, j * latoQuadrato,
                            i * latoQuadrato + latoQuadrato,
                            j * latoQuadrato + latoQuadrato, al_map_rgb(225, 0, 0)); // rosso
                    break;
				case 3: // brucia2
                    al_draw_filled_rectangle(i * latoQuadrato, j * latoQuadrato,
                            i * latoQuadrato + latoQuadrato,
                            j * latoQuadrato + latoQuadrato, al_map_rgb(255, 50,30)); // rosso chiaro
                    break;
                case 4: // brucia3
                    al_draw_filled_rectangle(i * latoQuadrato, j * latoQuadrato,
                            i * latoQuadrato + latoQuadrato,
                            j * latoQuadrato + latoQuadrato, al_map_rgb(255, 130, 100)); // arancione chiaro
                  break;
                
                default:
                    break;
                }
            }
        }

 

    }
    al_flip_display();
    al_rest(0.08);         // funzione di rallentamento per far vedere meglio la grafica
}


void controllaVettoreSu(int *vettoriSu,  int * supportoSub, int j){
    if(vettoriSu[j]==BRUCIAPIOGGIA || vettoriSu[j]==TERRAPIOGGIA || vettoriSu[j]==ALBEROPIOGGIA){
        if (vettoriSu[j]==TERRAPIOGGIA){
            vettoriSu[j]=TERRA;
            supportoSub[j]=TERRAPIOGGIA;
        }
        else if (vettoriSu[j]==ALBEROPIOGGIA){
            vettoriSu[j]=ALBERO;
            supportoSub[j]=ALBEROPIOGGIA;
        }
        else if (vettoriSu[j]==BRUCIAPIOGGIA ){
            vettoriSu[j]=TERRA;
            supportoSub[j]=BRUCIAPIOGGIA;
        }
    }

}





int main(int argc, char *argv[]) {
	srand(time(NULL));

  	int numProcessi;
	int rank;
	  
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcessi);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	double start,fine;

  
	al_init();
	al_init_primitives_addon();
	if(rank == MASTER)
	{	

		display = al_create_display(larghezza, altezza);	
	
	}

	int dimLocale = dim / numProcessi; // numero dim locale ad ogni thread
	
	if (rank == numProcessi - 1) //assegno le righe rimanenti all'ultimo thread
	{
	   dimLocale += dim % numProcessi;
	}

    int *matrice =(int*) malloc(dim*dim * sizeof(int));
    for(int i =0; i<dim; ++i)
       for(int j=0; j<dim; ++j)
            matrice[i*dim+j]= TERRA;

   
	// inizializza 
	if(rank == MASTER){
		start=MPI_Wtime();
		for(int i =0; i<dim; ++i)
		{	for(int j=0; j<dim; ++j)
			{    
	            double genera = (double) rand()/RAND_MAX;
	            
	            if(genera < probIncendio)
	                matrice[i*dim+j]= BRUCIA1;
	            else if(genera < probAlbero)
	                matrice[i*dim+j]= ALBERO;
	            else
	                matrice[i*dim+j]= TERRA; 
	              
	             
	        }  
	    }
	}
    

    int * sottoMatrice=(int*) malloc(dim*dimLocale * sizeof(int));
    int * supportoSub=(int*) malloc(dim*dimLocale * sizeof(int));
    //int sottoMatrice[dimLocale][dim];
    //int supportoSub[dimLocale][dim];

    // orlatura su e giù
     int * vettoriSu=(int*) malloc(dim* sizeof(int));

   // int vettoriGiu[dim]; // // matrice di orlatura giu
    int * vettoriGiu=(int*) malloc(dim* sizeof(int));

   // int * matr=&(matrice[0][0]);

    MPI_Scatter (matrice, dim*dimLocale, MPI_INT, sottoMatrice,dim*dimLocale,MPI_INT,MASTER,MPI_COMM_WORLD);
	
      int cellaSu = (rank == 0) ? numProcessi - 1 : rank - 1; // if (rank == 0) numProcessi - 1 else rank - 1
      int cellaGiu = (rank == numProcessi - 1) ? 0 : rank + 1;
  


 for(int l=0;l<1000;l++)
 {
//  		cout<<"iterazione num: "<<l<<endl;

		MPI_Send(&sottoMatrice[0], dim, MPI_INT, cellaSu, 10, MPI_COMM_WORLD);
        MPI_Send(&sottoMatrice[(dimLocale)*dim-dim], dim, MPI_INT, cellaGiu, 11, MPI_COMM_WORLD);

        MPI_Recv(vettoriGiu, dim, MPI_INT, cellaGiu, 10, MPI_COMM_WORLD, &status);
        MPI_Recv(vettoriSu, dim, MPI_INT, cellaSu, 11, MPI_COMM_WORLD, &status);
      
        
        for (int i = 0; i<dimLocale;++i)
        	for (int j=0; j<dim;++j)
        		supportoSub[i*dim+j]=sottoMatrice[i*dim+j];


		for (unsigned i = 0; i < dimLocale; i++)
    	{
            for (unsigned j = 0; j < dim; j++)
            {
            	
                if(vettoriSu[j]==BRUCIAPIOGGIA)
                    vettoriSu[j]=TERRA;
                else if (vettoriSu[j]==ALBEROPIOGGIA)
                    vettoriSu[j]=ALBERO;
                else if (vettoriSu[j]==TERRAPIOGGIA)
                    vettoriSu[j]=TERRA;

                if (vettoriSu[j]==BRUCIA1)
                    vettoriSu[j]=TERRA;
                else if (vettoriSu[j]==BRUCIA2)
                    vettoriSu[j]=BRUCIA3;
                else if (vettoriSu[j]==BRUCIA3)
                    vettoriSu[j]=TERRA;

                if(vettoriGiu[j]==BRUCIAPIOGGIA)
                    vettoriGiu[j]=TERRA;
                else if (vettoriGiu[j]==ALBEROPIOGGIA)
                    vettoriGiu[j]=ALBERO;
                else if (vettoriGiu[j]==TERRAPIOGGIA)
                    vettoriGiu[j]=TERRA;

                if (vettoriGiu[j]==BRUCIA1)
                    vettoriGiu[j]=TERRA;
                else if (vettoriGiu[j]==BRUCIA2)
                    vettoriGiu[j]=BRUCIA3;
                else if (vettoriGiu[j]==BRUCIA3)
                    vettoriGiu[j]=TERRA;

            	controllaVettoreSu (vettoriSu, supportoSub,j);

                if ((vettoriSu[j]==BRUCIA1 || vettoriSu[j]==BRUCIA2 || vettoriSu[j]==BRUCIA3) && supportoSub[1*dim+j]==ALBERO)
                    supportoSub[1*dim+j]=BRUCIA1;
	            else if (supportoSub[1*dim+j]==BRUCIA1 || supportoSub[1*dim+j]==BRUCIA2 || supportoSub[1*dim+j]==BRUCIA3)
	                 vettoriSu[j]=BRUCIA1;

                if ((vettoriGiu[j]==BRUCIA1 || vettoriGiu[j]==BRUCIA2 || vettoriGiu[j]==BRUCIA3) && supportoSub[(dimLocale-1)*dim+j]==ALBERO)
                    supportoSub[(dimLocale-1)*dim+j]=BRUCIA1;
	            else if (supportoSub[(dimLocale-1)*dim+j]==BRUCIA1 || supportoSub[(dimLocale-1)*dim+j]==BRUCIA2 || supportoSub[(dimLocale-1)*dim+j]==BRUCIA3)
	                vettoriGiu[j]=BRUCIA1;  
                double genera = (double) rand()/RAND_MAX;

                 if (sottoMatrice[i*dim+j]==ALBERO && !controlla_VicinoAlbero(sottoMatrice, i,j,dimLocale)) { //Se un albero non ha alcun vicino che sta bruciando, è soggetto a
                    double genera = (double) rand()/RAND_MAX;                       // poter ricevere un fulmine, se lo riceve, brucia
                    if (genera<=probIncendio) // propabilità incendio
                        supportoSub[i*dim+j]=BRUCIA1;
                    }
                 
                if(genera < probPioggia)
                    generaNuvola(supportoSub,sottoMatrice,i,j,vettoriSu,vettoriGiu,dimLocale);
                
                if(supportoSub[i*dim+j]!=TERRAPIOGGIA && supportoSub[i*dim+j]!=BRUCIAPIOGGIA && supportoSub[i*dim+j]!=ALBEROPIOGGIA)
                {	if (sottoMatrice[i*dim+j]==ALBERO && controlla_VicinoAlbero(sottoMatrice, i,j,dimLocale)) //Se un albero ha un albero vicino che brucia, anche questo prende fuoco
                		{if (i==0)
                			vettoriSu[j]=BRUCIA1;
                		else if (i>0 && (i*dim<(dim*(dimLocale-1))))
               		  		supportoSub[i*dim+j]=BRUCIA1;
               		  	else if (i*dim==(dim*(dimLocale-1)))
               		  		vettoriGiu[j]=BRUCIA1;
               		  	}
                
               		
			       		
	                
	                if (sottoMatrice[i*dim+j]==BRUCIA1)    //Un albero che brucia cambia tonalità
	                    supportoSub[i*dim+j]=BRUCIA2;

	                else if (sottoMatrice[i*dim+j]==BRUCIA2)   
	                    supportoSub[i*dim+j]=BRUCIA3;
	                
	                else if (sottoMatrice[i*dim+j]==BRUCIA3)   
	                    supportoSub[i*dim+j]=TERRA;


               	}
                            
                    
                if(sottoMatrice[i*dim+j]== TERRA || (vettoriSu[j]==TERRA && i==0) ||  (vettoriGiu[j]==TERRA && (i*dim==(dim*(dimLocale-1)))) ) //Spostare i casi 0 e dimLocale-1
                    {double genera = (double) rand()/RAND_MAX;
                        if (genera<probAlbero && i>0 && (i*dim<(dim*(dimLocale-1)))) {
                            supportoSub[i*dim+j]=ALBERO;
                        }
                        else if (genera<probAlbero && i==0)
                            vettoriSu[j]=ALBERO;
                        else if (genera<probAlbero && (i*dim==(dim*(dimLocale-1))))
                            vettoriGiu[j]=ALBERO;
                    }
                
				// movimento nuvola di pioggia 

                if (i*dim<(dim*(dimLocale-1))){
                    if(sottoMatrice[i*dim+j]==BRUCIAPIOGGIA || sottoMatrice[i*dim+j]==TERRAPIOGGIA || sottoMatrice[i*dim+j]==ALBEROPIOGGIA){
                        if (sottoMatrice[(i*dim)+dim+j]==TERRA)
                            supportoSub[(i*dim)+dim+j]=TERRAPIOGGIA;
                        else if (sottoMatrice[(i*dim)+dim+j]==ALBERO)
                                    supportoSub[(i*dim)+dim+j]=ALBEROPIOGGIA;
                        else if (sottoMatrice[(i*dim)+dim+j]==BRUCIA1 || sottoMatrice[(i*dim)+dim+j]==BRUCIA2 || sottoMatrice[(i*dim)+dim+j]==BRUCIA3 )
                                    supportoSub[(i*dim)+dim+j]=BRUCIAPIOGGIA;
                                

                        if(sottoMatrice[i*dim+j]==BRUCIAPIOGGIA)
                            supportoSub[i*dim+j]=TERRA;
                        else if (sottoMatrice[i*dim+j]==ALBEROPIOGGIA)
                            supportoSub[i*dim+j]=ALBERO;
                        else if (sottoMatrice[i*dim+j]==TERRAPIOGGIA)
                            supportoSub[i*dim+j]=TERRA;

                    }

                }
                else if ((i*dim)>=(dim*(dimLocale-1))){
                	
                        if (supportoSub[i*dim+j]==BRUCIAPIOGGIA || supportoSub[i*dim+j]==TERRAPIOGGIA || supportoSub[i*dim+j]==ALBEROPIOGGIA){
                            if (vettoriGiu[j]==TERRA)
                                vettoriGiu[j]=TERRAPIOGGIA;
                            else if (vettoriGiu[j]==ALBERO)
                                vettoriGiu[j]=ALBEROPIOGGIA;
                            else if (vettoriGiu[j]==BRUCIA1 || vettoriGiu[j]==BRUCIA2 ||vettoriGiu[j]==BRUCIA3 )
                                vettoriGiu[j]=BRUCIAPIOGGIA;

                            if(supportoSub[i*dim+j]==BRUCIAPIOGGIA)
                                supportoSub[i*dim+j]=TERRA;
                            else if (supportoSub[i*dim+j]==ALBEROPIOGGIA)
                                supportoSub[i*dim+j]=ALBERO;
                            else if (supportoSub[i*dim+j]==TERRAPIOGGIA)
                                supportoSub[i*dim+j]=TERRA;
                        }          
                } 
            
                
			}   
        } // fine doppio for


  		// Send e Recive relative al movimento della nuvola

        MPI_Send(vettoriSu, dim, MPI_INT, cellaSu, 2, MPI_COMM_WORLD);
        MPI_Send(vettoriGiu, dim, MPI_INT, cellaGiu, 0, MPI_COMM_WORLD);

        MPI_Recv(&supportoSub[dim*(dimLocale-1)], dim, MPI_INT, cellaGiu, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&supportoSub[0], dim, MPI_INT, cellaSu, 0, MPI_COMM_WORLD, &status);
    		

        for (int i = 0; i<dimLocale;i++)
        	for (int j=0; j<dim;j++)
        		sottoMatrice[i*dim+j]=supportoSub[i*dim+j];
		
	    MPI_Gather (sottoMatrice, dim*dimLocale, MPI_INT, matrice,dim*dimLocale,MPI_INT,MASTER,MPI_COMM_WORLD);
     
	//if (rank==MASTER)
	//	cout<<"iterazione num: "<<l<<endl;
	if(rank==MASTER)
		disegnaConAllegro(matrice);

 }	// fine for passi


  	MPI_Barrier(MPI_COMM_WORLD);
	free(matrice);
    free(supportoSub);
    free(sottoMatrice);
    free(vettoriSu);
    free(vettoriGiu);

    if (rank==MASTER){
    	fine=MPI_Wtime();
    	cout<<"Il tempo tot è: "<<fine-start<<endl;
    }


    MPI_Finalize();

 	al_destroy_display(display);

    
  	return 0;


}