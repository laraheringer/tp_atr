/*********************************************************************************
*  AUTOMAÇÃO EM TEMPO REAL - ELT127 - TD
*
*  Juliana Arruda Narcizo e Lara Souza Heringer
*
*  Trabalho Final: "Detecção de defeitos em tiras metálicas"
*
*  O programa é composto pelo processo de exibição de dados na tela
**********************************************************************************/

#include<Windows.h>
#include <iostream>

using namespace std;

int main() {

	HANDLE hProcess;
	HANDLE hThread;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD dwProcessExibeDefeitos = 0;
	DWORD dwThreadExibeDefeitos = 0;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	BOOL bCreateProcess = NULL;

	bCreateProcess = CreateProcess(
		L "",
		NULL,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi);

	if (bCreateProcess == FALSE) {
		cout << "ERRO NA CRIACAO DO PROCESSO" << GetLastError() << endl;
	}
	else {
		cout << "PROCESSO CRIADO COM SUCESSO" << endl;
	}

	cout << "Process ID" << pi.dwProcessId << endl;
	cout << "Thread ID" << pi.dwThreadId << endl;
	cout << "GetProcessID" << GetProcessId(pi.hProcess) << endl;
	cout << "GetProcessID" << GetProcessId(pi.hThread) << endl;

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return 0;
}