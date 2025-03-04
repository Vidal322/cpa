//#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <papi.h>

using namespace std;

#define SYSTEMTIME clock_t

 
void write_to_file(const char* filename, double time, long long values[], int  size) {
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
    outfile << values[4];
    outfile.close();
}

void OnMult(int m_ar, int m_br, const char* filename, int EventSet) {
	
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

	ret = PAPI_stop(EventSet, values);
	if (ret != PAPI_OK) cout << "ERRO: Stop PAPI" << endl;

	write_to_file(filename, (double)(Time2 - Time1) / CLOCKS_PER_SEC, values, m_ar);


	


    free(pha);
    free(phb);
    free(phc);
	

}


void OnMultLine(int m_ar, int m_br, const char* filename, int EventSet)
{

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

    Time1 = clock();

	for(i=0; i<m_ar; i++)
	{	for( j=0; j<m_br; j++)
		{	
			for( k=0; k<m_ar; k++)	
			{	
				phc[i*m_ar + k] += pha[i*m_ar + j] * phb[j*m_br+k];
			}
		}
	}

    Time2 = clock();
	ret = PAPI_stop(EventSet, values);
	if (ret != PAPI_OK) cout << "ERRO: Stop PAPI" << endl;

	write_to_file(filename, (double)(Time2 - Time1) / CLOCKS_PER_SEC, values, m_ar);


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
	outfile << "Size,Time,L1_DCM,L2_DCM,FP_OPS,TOT_INS,TLB_DM" << endl;
	outfile.close();


	//  Multiplication

	for (int i = 600; i <= 3000; i += 400) {
		for (int j = 0; j < 5; j++) {
			OnMult(i, i, filename, EventSet);

			outfile.open(filename, ios::app);
			if (!outfile) {
				cerr << "Error opening file!" << endl;
				return -1;
			}
			outfile << "\n";
			outfile.close();
		}
	}

	for (int i = 4096; i <= 10240; i += 2048) {
		for (int j = 0; j < 5; j++) {
			OnMultLine(i, i, filename, EventSet);

			outfile.open(filename, ios::app);
			if (!outfile) {
				cerr << "Error opening file!" << endl;
				return -1;
			}
			outfile << "\n";
			outfile.close();
		}
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
