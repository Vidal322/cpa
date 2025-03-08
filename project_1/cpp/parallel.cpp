#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <papi.h>
#include <eml.h>
#include <omp.h>

using namespace std;

#define SYSTEMTIME clock_t

 
void write_to_file(const char* filename, double time, long long values[], int  size, float energy) {
    ofstream outfile;
    outfile.open(filename, ios::app); // Append mode
    if (!outfile) {
        cerr << "Error opening file!" << endl;
        return;
    }
	outfile << size << ",";
    outfile << fixed << setprecision(3) << time << ",";
    outfile << values[0] << ",";
    outfile << values[1] << ",";
    outfile << values[2] << ",";
    outfile << values[3] << ",";
    outfile << values[4] << ",";
	outfile << fixed << setprecision(3)<< energy << std::endl;
    outfile.close();
}

void OnMult(int m_ar, int m_br, const char* filename, int EventSet) {
	

	emlInit();
	//allocate space for results
	size_t count = 1;
	emlDeviceGetCount(&count);
	emlData_t *data[count];
	
	SYSTEMTIME Time1, Time2;
	
	char st[100];
	double temp;
	int i, j, k;

	double *pha, *phb, *phc;

	long long values[5];
	

		
    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phb = (double *)malloc((m_ar * m_ar) * sizeof(double));
	phc = (double *)malloc((m_ar * m_ar) * sizeof(double));

	for(i=0; i<m_ar; i++)
		for(j=0; j<m_ar; j++)
			pha[i*m_ar + j] = (double) 1.0;



	for(i=0; i<m_br; i++)
		for(j=0; j<m_br; j++)
			phb[i*m_br + j] = (double)(i+1);



	int ret = PAPI_start(EventSet);
	if (ret != PAPI_OK) cout << "ERRO: Start PAPI" << endl;

	emlStart();
    Time1 = clock();

	for(i=0; i<m_ar; i++)
	{	for( j=0; j<m_br; j++)
		{	temp = 0;
			for( k=0; k<m_ar; k++)
			{	
				temp += pha[i*m_ar+k] * phb[k*m_br+j];
			}
			phc[i*m_ar+j]=temp;
		}
	}


    Time2 = clock();
	emlStop(data);

	ret = PAPI_stop(EventSet, values);
	if (ret != PAPI_OK) cout << "ERRO: Stop PAPI" << endl;

	double consumed;
	emlDataGetConsumed(data[0], &consumed);
	emlDataFree(data[0]);
	printf("This device consumed %g J \n", consumed);

	write_to_file(filename, (double)(Time2 - Time1) / CLOCKS_PER_SEC, values, m_ar, consumed);
	


    free(pha);
    free(phb);
    free(phc);
	

}



void OnMultLine1(int m_ar, int m_br, const char* filename, int eventSet) {
    emlInit();
    
    size_t count = 1;
    emlDeviceGetCount(&count);
    emlData_t* data[count];
    
    double Time1, Time2;
    double *pha, *phb, *phc;
    long long values[5];
    
    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phb = (double *)malloc((m_br * m_br) * sizeof(double));
    phc = (double *)malloc((m_ar * m_br) * sizeof(double));

    for (int i = 0; i < m_ar; i++)
        for (int j = 0; j < m_ar; j++)
            pha[i * m_ar + j] = 1.0;

    for (int i = 0; i < m_br; i++)
        for (int j = 0; j < m_br; j++)
            phb[i * m_br + j] = (double)(i + 1);

    for (int i = 0; i < m_ar; i++)
        for (int j = 0; j < m_br; j++)
            phc[i * m_br + j] = 0.0;

    emlStart();
    Time1 = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < m_ar; i++) {
        for (int k = 0; k < m_br; k++) {
            for (int j = 0; j < m_ar; j++) {
                phc[i * m_br + j] += pha[i * m_ar + k] * phb[k * m_br + j];
            }
        }
    }

    Time2 = omp_get_wtime();
    emlStop(data);
    
    double consumed;
    emlDataGetConsumed(data[0], &consumed);
    emlDataFree(data[0]);
    
    printf("This device consumed %g J \n", consumed);
    
    write_to_file(filename, Time2 - Time1, values, m_ar, consumed);
    
    free(pha);
    free(phb);
    free(phc);
}


void OnMultLine2(int m_ar, int m_br, const char* filename, int eventSet) {
    emlInit();
    
    // Allocate space for power measurement
    size_t count = 1;
    emlDeviceGetCount(&count);
    emlData_t* data[count];
    
    double Time1, Time2;
    double *pha, *phb, *phc;

    long long values[5];
    
    pha = (double *)malloc((m_ar * m_ar) * sizeof(double));
    phb = (double *)malloc((m_br * m_br) * sizeof(double));
    phc = (double *)malloc((m_ar * m_br) * sizeof(double));

    for (int i = 0; i < m_ar; i++)
        for (int j = 0; j < m_ar; j++)
            pha[i * m_ar + j] = 1.0;

    for (int i = 0; i < m_br; i++)
        for (int j = 0; j < m_br; j++)
            phb[i * m_br + j] = (double)(i + 1);

    for (int i = 0; i < m_ar; i++)
        for (int j = 0; j < m_br; j++)
            phc[i * m_br + j] = 0.0;

    emlStart();
    Time1 = omp_get_wtime();

    #pragma omp parallel private(i, k)
    for (int i = 0; i < m_ar; i++) {
        for (int k = 0; k < m_br; k++) {
            #pragma omp for
            for (int j = 0; j < m_ar; j++) {
                phc[i * m_br + j] += pha[i * m_ar + k] * phb[k * m_br + j];
            }
        }
    }

    Time2 = omp_get_wtime();
    emlStop(data);
    
    double consumed;
    emlDataGetConsumed(data[0], &consumed);
    emlDataFree(data[0]);
    
    printf("This device consumed %g J \n", consumed);
    
    write_to_file(filename, Time2 - Time1, values, m_ar, consumed);
    
    free(pha);
    free(phb);
    free(phc);
}





void handle_error (int retval)
{
  printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
  exit(1);
}

void init_papi() {
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT && retval < 0) {
    printf("PAPI library version mismatch!\n");
    exit(1);
  }
  if (retval < 0) handle_error(retval);

  std::cout << "PAPI Version Number: MAJOR: " << PAPI_VERSION_MAJOR(retval)
            << " MINOR: " << PAPI_VERSION_MINOR(retval)
            << " REVISION: " << PAPI_VERSION_REVISION(retval) << "\n";
}


int main (int argc, char *argv[])
{

	char c;
	int lin, col, nt=1;
	int op;
	
	int EventSet = PAPI_NULL;
  	long long values[5];
  	int ret;

	char filename[40] = "simple.csv";
	
	//PAPI

	ret = PAPI_library_init( PAPI_VER_CURRENT );
	if ( ret != PAPI_VER_CURRENT )
		std::cout << "FAIL" << endl;

	ret = PAPI_create_eventset(&EventSet);
		if (ret != PAPI_OK) cout << "ERRO: create eventset" << endl;


	ret = PAPI_add_event(EventSet,PAPI_L1_DCM );
	if (ret != PAPI_OK) {
		cout << "ERRO: PAPI_L1_DCM" << endl;
	}
	ret = PAPI_add_event(EventSet,PAPI_L2_DCM);
	if (ret != PAPI_OK) cout << "ERRO: PAPI_L2_DCM" << endl;

	ret = PAPI_add_event(EventSet,PAPI_FP_OPS);
	if (ret != PAPI_OK) cout << "ERRO: PAPI_FP_OPS: " << ret << endl;

	ret = PAPI_add_event(EventSet,PAPI_TOT_INS);
	if (ret != PAPI_OK) cout << "ERRO: PAPI_TOT_INS" << endl;

	ret = PAPI_add_event(EventSet,PAPI_TLB_DM);
	if (ret != PAPI_OK) cout << "ERRO: PAPI_TLB_DM: " << ret <<endl;


	ofstream outfile;
    outfile.open(filename);	
    if (!outfile) {
        cerr << "Error opening file!" << endl;
        return -1;
    }
	outfile << "Size,Time,L1_DCM,L2_DCM,FP_OPS,TOT_INS,TLB_DM,Energy(J)" << endl;
	outfile.close();


	//  Multiplication

		for (int j = 0; j < 5; j++) {
			OnMultLine1(1000, 1000, filename, EventSet);
		}

		for (int j = 0; j < 5; j++) {
			OnMultLine1(1400, 1400, filename, EventSet);
		}
		for (int j = 0; j < 5; j++) {
			OnMultLine1(2000, 2000, filename, EventSet);
		}


		for (int j = 0; j < 5; j++) {
			OnMultLine2(1000, 1000, filename, EventSet);
		}

		for (int j = 0; j < 5; j++) {
			OnMultLine2(1400, 1400, filename, EventSet);
		}
		for (int j = 0; j < 5; j++) {
			OnMultLine2(2000, 2000, filename, EventSet);
		}



	// Close PAPI

	ret = PAPI_remove_event( EventSet, PAPI_L1_DCM );
	if ( ret != PAPI_OK )
		std::cout << "FAIL remove event" << endl; 

	ret = PAPI_remove_event( EventSet, PAPI_L2_DCM );
	if ( ret != PAPI_OK )
		std::cout << "FAIL remove event" << endl; 
	
	ret = PAPI_remove_event( EventSet, PAPI_FP_OPS );
	if ( ret != PAPI_OK )
		std::cout << "FAIL remove event" << endl;
	
	ret = PAPI_remove_event( EventSet, PAPI_TOT_INS );
	if ( ret != PAPI_OK )
		std::cout << "FAIL remove event" << endl;
	
	ret = PAPI_remove_event( EventSet, PAPI_TLB_DM );
	if ( ret != PAPI_OK )
		std::cout << "FAIL remove event" << endl;


	ret = PAPI_destroy_eventset( &EventSet );
	if ( ret != PAPI_OK )
		std::cout << "FAIL destroy" << endl;

	return 0;
}
