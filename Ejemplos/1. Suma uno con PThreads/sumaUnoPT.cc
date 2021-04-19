/**
 *  Suma uno a un total mil veces por cada proceso generado
 *  Recibe como parametro la cantidad de procesos que se quiere arrancar
 *  Author: Arquitectura de computadores (Francisco Arroyo)
 *  Version: 2021/Abr/12
 *
 *  Compilar con:
 *   g++ -g sumaUnoPT.cc -lpthread
**/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <thread>
#include <vector>
#include <sys/time.h>

// Shared variables
long total;
pthread_mutex_t * mutex;
struct cosita {
   long total;
   int contador;
   float r;
};


/*
 *  Do some work, by now add one to a variable
 */
int AddOneWithMutex( long hilo ) {
   int i;
   long myTotal = 0;

   for ( i = 0; i < 1000; i++ ) {
      myTotal++;
      usleep( 1 );
   }

   pthread_mutex_lock( mutex );
   total += myTotal;
   pthread_mutex_unlock( mutex );

   pthread_exit( 0 );

}


/*
 *  Do some work, by now add one to a variable
 */
int AddOne( long hilo ) {
   int i;

   for ( i = 0; i < 1000; i++ ) {
      total++;
      usleep( 1 );
   }

   pthread_exit( 0 );

}


/*
  Serial test
*/
long SerialTest( long hilos ) {
   long i, hilo;

   for ( hilo = 0; hilo < hilos; hilo++ ) {

      for ( i = 0; i < 1000; i++ ) {
         total++;			// Suma uno
         usleep( 1 );
      }

   }

   return total;

}


/*
  Fork test with NO race condition
*/
long ForkTestNoRaceCondition( long hilos ) {
   long hilo;
   std::vector< std::thread * > trabajadores;

   mutex = (pthread_mutex_t *) calloc( 1, sizeof( pthread_mutex_t ) );
   pthread_mutex_init( mutex, NULL );

   for ( hilo = 0; hilo < hilos; hilo++ ) {
      std::thread * nuevo  = new std::thread( AddOneWithMutex, hilo);
      trabajadores.push_back( nuevo );
   }

   for ( auto h: trabajadores ) {
      h->join();
   }

   pthread_mutex_destroy( mutex );
   free( mutex );

   return total;

}

/*
  Fork test with race condition
*/
long ForkTestRaceCondition( long hilos ) {
   long hilo;
   std::vector<std::thread*> trabajadores;


   for ( hilo = 0; hilo < hilos; hilo++ ) {
      std::thread * nuevo  = new std::thread( AddOne, hilo);
      trabajadores.push_back( nuevo );
   }

   for ( auto h: trabajadores ) {
      h->join();
   }

   return total;

}


/*
 *
 */
void startTimer( struct timeval * timerStart) {
   gettimeofday( timerStart, NULL );
}


/*
 *  time elapsed in ms
 */
double getTimer( struct timeval timerStart ) {
   struct timeval timerStop, timerElapsed;

   gettimeofday(&timerStop, NULL);
   timersub(&timerStop, &timerStart, &timerElapsed);
   return timerElapsed.tv_sec*1000.0+timerElapsed.tv_usec/1000.0;
}



int main( int argc, char ** argv ) {
   long hilos;
   clock_t start, finish;
   struct timeval timerStart;
   double used, wused;


   hilos = 100;
   if ( argc > 1 ) {
      hilos = atol( argv[ 1 ] );
   }


   startTimer( & timerStart );
   start = clock();
   total = 0;
   SerialTest( hilos );
   finish = clock();
   used = ((double) (finish - start)) / CLOCKS_PER_SEC;
   wused = getTimer( timerStart );
   printf( "Serial version:      total es \033[91m %ld \033[0m con %ld hilos, CPU time %g seconds, wall time %g \n", total, hilos, used, wused );

   startTimer( & timerStart );
   start = clock();
   total = 0;
   ForkTestRaceCondition( hilos );
   finish = clock();
   used = ((double) (finish - start)) / CLOCKS_PER_SEC;
   wused = getTimer( timerStart );
   printf( "PThr, Race Cond.:    total es \033[91m %ld \033[0m con %ld hilos, CPU time %g seconds, wall time %g \n", total, hilos, used, wused );

   startTimer( & timerStart );
   start = clock();
   total = 0;
   ForkTestNoRaceCondition( hilos );
   finish = clock();
   used = ((double) (finish - start)) / CLOCKS_PER_SEC;
   wused = getTimer( timerStart );
   printf( "PThr, NO Race Cond.: total es \033[91m %ld \033[0m con %ld hilos, CPU time %g seconds. wall time %g \n", total, hilos, used, wused );
}

