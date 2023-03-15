#include<iostream>
//#include <mpi.h>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
using namespace std::chrono; 

using namespace std;

const int dim = 100; 

// g++ Foresta.cpp -lallegro -lallegro_primitives -lallegro_main

enum Stato {ALBERO = 0, TERRA = 1, BRUCIA1 = 2, BRUCIA2 = 3, BRUCIA3 = 4, ALBEROPIOGGIA=5, TERRAPIOGGIA=6, BRUCIAPIOGGIA=7};

const double p= 0.02;	// probabilità nascita albero
const double f = 0.0002; // probabilità incendio
const double q= 0.00002; // probabilità pioggia

// Allegro
ALLEGRO_DISPLAY * display;
const int larghezza = 800;
const int altezza = 800;
const int latoQuadrato = 8;





void creaMappa(int **world){
	for(int i=0; i<dim;i++)
	{	for(int j=0; j<dim;j++)
		{	double genera = (double) rand()/RAND_MAX;
			
			if(genera < f)
				world[i][j]=BRUCIA1;
			else if(genera < p)
				world[i][j]=ALBERO;
			else
				world[i][j]=TERRA;
		}
	}		
}
void generaNuvola(int** B,int** world, int i,int j)
{ 
     
     if (world[i][j]==TERRA)
            B[i][j]=TERRAPIOGGIA;
        else if (world[i][j]==ALBERO)
             B[i][j]=ALBEROPIOGGIA;
        else if (world[i][j]==BRUCIA1 || world[i][j]==BRUCIA2 || world[i][j]==BRUCIA3)
             B[i][j]=BRUCIAPIOGGIA;
        
    
    
}

bool controlla_VicinoAlbero( int **world, int x , int y){
    for (int riga= x-1; riga<=x+1;riga++)
        {
            if (riga>=0 && riga<dim)
                {for (int colonna=y-1; colonna<=y+1; colonna++)
                    {if (world[riga][colonna]==BRUCIA1 && (riga!=x || colonna!=y))
                        return true;
                    }
                }
        }
    return false;
}



void disegnaConAllegro(int** world) {

    al_clear_to_color(al_map_rgb(0, 0, 0)); 

    for (unsigned i = 0; i < dim; i++)
    {
        for (unsigned j = 0; j < dim; j++)
        {
            if( world[i][j]==TERRAPIOGGIA || 	world[i][j]==ALBEROPIOGGIA ||	world[i][j]==BRUCIAPIOGGIA)
            {
                al_draw_filled_rectangle(i * latoQuadrato, j * latoQuadrato,
                        i * latoQuadrato + latoQuadrato,
                        j * latoQuadrato + latoQuadrato, al_map_rgb(171, 205, 239)); // celeste chiaro
            }
            else{
                switch (world[i][j]) {
                
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
    al_rest(0.0901);         // funzione di rallentamento per far vedere meglio la grafica
}




int main(int argc, char *argv[]) 
{
	auto start = high_resolution_clock::now(); 


		ALLEGRO_KEYBOARD_STATE key_state;
		al_init();
		display = al_create_display(larghezza, altezza);
		al_init_primitives_addon();
		if(!al_install_keyboard()){
			return -1;
		}


	// inizializzazione matrici
	int ** world= new int *[dim];
	int ** B= new int *[dim];
	for (int i = 0; i<dim; i++)
	{	
		world[i]=new int[dim];
		B[i]=new int[dim];
	}

		


 	creaMappa(world);

 	for (int i=0; i<dim;i++)
 		for (int j=0; j<dim;j++)
 			B[i][j]=world[i][j];

	for (int l=0;l<1000;l++)
	{ 	

	 	// controllo vicini
	 	for(int i=0; i<dim;i++){
			for(int j=0; j<dim;j++){

				double genera = (double) rand()/RAND_MAX;
				if(genera < q && i>0 && j>0){
					generaNuvola(B,world,i,j);

				}
				if(B[i][j]!=TERRAPIOGGIA && B[i][j]!=BRUCIAPIOGGIA && B[i][j]!=ALBEROPIOGGIA)
				{	
					if (world[i][j]==ALBERO && controlla_VicinoAlbero(world, i,j)) //Se un albero ha un albero vicino che brucia, anche questo prende fuoco
						B[i][j]=BRUCIA1;
					else if (world[i][j]==ALBERO && !controlla_VicinoAlbero(world,i,j)){ //Se un albero non ha alcun vicino che sta bruciando, è soggetto a 
						double genera = (double) rand()/RAND_MAX;						// poter ricevere un fulmine, se lo riceve, brucia
						if (genera<=f)
							B[i][j]=BRUCIA1;
					}
					if (world[i][j]==BRUCIA1)	//Un albero che brucia cambia tonalità
						B[i][j]=BRUCIA2;
					if (world[i][j]==BRUCIA2)	
						B[i][j]=BRUCIA3;
					if (world[i][j]==BRUCIA3)	
						B[i][j]=TERRA;					
				
					if(world[i][j] == TERRA)
					{	double genera = (double) rand()/RAND_MAX;
						if(genera < p)
							B[i][j]=ALBERO;
					}
				}
				else{
				
					// nuvola di pioggia
					if (i<dim-1){
                    if(world[i][j]==BRUCIAPIOGGIA || world[i][j]==TERRAPIOGGIA || world[i][j]==ALBEROPIOGGIA){
                        if (world[i+1][j]==TERRA)
                            B[i+1][j]=TERRAPIOGGIA;
                        else if (world[i+1][j]==ALBERO)
                                    B[i+1][j]=ALBEROPIOGGIA;
                        else if (world[i+1][j]==BRUCIA1 || world[i+1][j]==BRUCIA2 || world[i+1][j]==BRUCIA3 )
                                    B[i+1][j]=BRUCIAPIOGGIA;
                                

                        if(world[i][j]==BRUCIAPIOGGIA)
                            B[i][j]=TERRA;
                        else if (world[i][j]==ALBEROPIOGGIA)
                            B[i][j]=ALBERO;
                        else if (world[i][j]==TERRAPIOGGIA)
                            B[i][j]=TERRA;

                    }

                }
                else if (i==dim-1){
                	if(world[i][j]==BRUCIAPIOGGIA || world[i][j]==TERRAPIOGGIA || world[i][j]==ALBEROPIOGGIA){
                        if (world[0][j]==TERRA)
                            B[0][j]=TERRAPIOGGIA;
                        else if (world[0][j]==ALBERO)
                                    B[0][j]=ALBEROPIOGGIA;
                        else if (world[0][j]==BRUCIA1 || world[0][j]==BRUCIA2 || world[0][j]==BRUCIA3 )
                                    B[0][j]=BRUCIAPIOGGIA;
                                

                        if(world[i][j]==BRUCIAPIOGGIA)
                            B[i][j]=TERRA;
                        else if (world[i][j]==ALBEROPIOGGIA)
                            B[i][j]=ALBERO;
                        else if (world[i][j]==TERRAPIOGGIA)
                            B[i][j]=TERRA;

                    }

                }
                 
			}	

				

			}
		}
		for (int i= 0; i<dim; i++)
			for (int j=0; j<dim;j++)
				world[i][j]=B[i][j];
	 	

	cout<<"iterazione corrente: "<<l<<endl;

 		disegnaConAllegro(world);
 	}

 	auto stop = high_resolution_clock::now(); 

 	auto duration = duration_cast<seconds>(stop - start); 
	cout << duration.count() << endl; 

	

//	al_destroy_display(display);
	
	for (int i = 0; i<dim; i++)
	{	delete[] world[i];
		delete[] B[i];
	}
	delete[] world;
	delete[] B;

	return 0;
}
