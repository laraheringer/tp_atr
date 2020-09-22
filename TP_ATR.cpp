/*********************************************************************************
*  AUTOMAÇÃO EM TEMPO REAL - ELT127 - TD
*  
*  Juliana Arruda Narcizo e Lara Souza Heringer
*
*  Trabalho Final: "Detecção de defeitos em tiras metálicas"
*
*  O programa é composto por 6 threads, além da thread primária, que tem como função
*  ler dados do sistema, capturar os dados, exibir dados em duas telas e ler entradas 
*  de um teclado.
**********************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <conio.h>

using namespace std;

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;


DWORD WINAPI ThreadLeituraInspecao;		//Thread para Leitura de mensagens provenientes do sistema de inspeção.
DWORD WINAPI ThreadCapturaDefeitos;    //Thread para Captura de defeitos das tiras metálicas e exibe na tela de defeitos.
DWORD WINAPI ThreadCapturaDados;      //Thread para Captura de dados relativos ao processo de laminação e escreve em um arquivo circular. 
DWORD WINAPI ThreadExibeDefeitos;    //Exibe na tela mensagens de defeitos superficiais obtidos na captura de defeitos.
DWORD WINAPI ThreadExibeDados;      //Exibe na tela mensagens referentes ao processo de laminação.
DWORD WINAPI ThreadLeituraTeclado; //Tratamento de entradas do teclado.

DWORD WINAPI ThreadFun() {
    cout << "Thread em execucao\n" << endl;
    Sleep(8000);
    cout << "Encerrando thread\n" << endl;
    return 0;
}

//THREAD PRIMÁRIA
int main()
{
    HANDLE hThreads[6];
    DWORD IdLeituraDados, IdCapturaDefeitos, IdCapturaDados, IdExibeDefeitos, IdExibeDados, IdLeituraTeclado;
    DWORD dwRet;

    hThreads[0] = (HANDLE) (HANDLE) _beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadFun,
        NULL,
        0,
        (CAST_LPDWORD)&IdLeituraDados);

    if (hThreads[0] == NULL) {
        cout << "Erro na criacao da thread\n" << IdLeituraDados << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    hThreads[1] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadFun,
        NULL,
        0,
        (CAST_LPDWORD)&IdCapturaDefeitos);

    if (hThreads[1] == NULL) {
        cout << "Erro na criacao da thread\n" << IdCapturaDefeitos << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    hThreads[2] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadFun,
        NULL,
        0,
        (CAST_LPDWORD)&IdCapturaDados);

    if (hThreads[2] == NULL) {
        cout << "Erro na criacao da thread\n" << IdCapturaDados << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    hThreads[3] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadFun,
        NULL,
        0,
        (CAST_LPDWORD)&IdExibeDefeitos);

    if (hThreads[3] == NULL) {
        cout << "Erro na criacao da thread\n" << IdExibeDefeitos << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    hThreads[4] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadFun,
        NULL,
        0,
        (CAST_LPDWORD)&IdExibeDados);

    if (hThreads[4] == NULL) {
        cout << "Erro na criacao da thread\n" << IdExibeDados << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    hThreads[5] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadFun,
        NULL,
        0,
        (CAST_LPDWORD)&IdLeituraTeclado);

    if (hThreads[5] == NULL) {
        cout << "Erro na criacao da thread\n" << IdLeituraTeclado << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    Sleep(5000);
    cout << "Esperando encerramento das threads\n" << endl;

    dwRet = WaitForMultipleObjects(6, hThreads, TRUE, INFINITE);

    // Fecha todos os handles de objetos do kernel
    for (int i = 0; i < 6; ++i)
        CloseHandle(hThreads[i]);

    cout << "Encerrando thread principal\n" << endl;

    return EXIT_SUCCESS;

}

