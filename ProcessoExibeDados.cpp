/*********************************************************************************
*  AUTOMAÇÃO EM TEMPO REAL - ELT127 - TD
*
*  Juliana Arruda Narcizo e Lara Souza Heringer
*
*  Trabalho Final: "Detecção de defeitos em tiras metálicas"
*
*  O programa a seguir é composto pelo processo de exibição de dados na tela
**********************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>		

using namespace std;

int main()
{

	BOOL bCreateProcess;
	STARTUPINFO si;				    
	PROCESS_INFORMATION NewProcess;	

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);	

	bCreateProcess = CreateProcess(
		"C:\\TP_ATR.exe.recipe", 
		NULL,                       
		NULL,                       
		NULL,                       
		FALSE,	                     
		CREATE_NEW_CONSOLE,	     
		NULL,	                     
		"C:\\Windows",              
		&si,			             
		&NewProcess);	            
	if (!bCreateProcess) {
		cout << "Erro na criacao do processo! Codigo = %d\n" << GetLastError << endl;
	}
	else {
		cout << "Processo Criado com sucesso!" << endl;
	}

	printf("Digite uma tecla qualquer para criar uma instancia do Firefox:\n");
	_getch();


	return EXIT_SUCCESS;
}	
