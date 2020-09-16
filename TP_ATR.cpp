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


DWORD WINAPI ThreadLeituraInspecao;		//Thread para Leitura de mensagens provenientes do sistema de inspeção.
DWORD WINAPI ThreadCapturaDefeitos;    //Thread para Captura de defeitos das tiras metálicas e exibe na tela de defeitos.
DWORD WINAPI ThreadCapturaDados;      //Thread para Captura de dados relativos ao processo de laminação e escreve em um arquivo circular. 
DWORD WINAPI ThreadExibeDefeitos;    //Exibe na tela mensagens de defeitos superficiais obtidos na captura de defeitos.
DWORD WINAPI ThreadExibeDados;      //Exibe na tela mensagens referentes ao processo de laminação.
DWORD WINAPI ThreadLeituraTeclado; //Tratamento de entradas do teclado.

DWORD WINAPI ThreadFun(LPVOID lpParam) {
    cout << "Thread em execucao" << endl;
    return 0;
}

//THREAD PRIMÁRIA
int main()
{
    HANDLE hThreads;
    DWORD IdLeituraDados, IdCapturaDefeitos, IdCapturaDados, IdExibeDefeitos, IdExibeDados, IdLeituraTeclado;

    hThreads = CreateThread(
        NULL,
        0,
        ThreadFun,
        NULL,
        0,
        & IdLeituraDados);

    if (hThreads == NULL) {
        cout << "Erro na criacao da thread" << IdLeituraDados << endl;
    }
    else {
        cout << "Thread criada com sucesso" << endl;
    }

    hThreads = CreateThread(
        NULL,
        0,
        ThreadFun,
        NULL,
        0,
        &IdCapturaDefeitos);

    if (hThreads == NULL) {
        cout << "Erro na criacao da thread" << IdCapturaDefeitos << endl;
    }
    else {
        cout << "Thread criada com sucesso" << endl;
    }
    hThreads = CreateThread(
        NULL,
        0,
        ThreadFun,
        NULL,
        0,
        &IdCapturaDados);

    if (hThreads == NULL) {
        cout << "Erro na criacao da thread" << IdCapturaDados << endl;
    }
    else {
        cout << "Thread criada com sucesso" << endl;
    }
    hThreads = CreateThread(
        NULL,
        0,
        ThreadFun,
        NULL,
        0,
        &IdExibeDefeitos);

    if (hThreads == NULL) {
        cout << "Erro na criacao da thread" << IdExibeDefeitos << endl;
    }
    else {
        cout << "Thread criada com sucesso" << endl;
    }
    hThreads = CreateThread(
        NULL,
        0,
        ThreadFun,
        NULL,
        0,
        &IdExibeDados);

    if (hThreads == NULL) {
        cout << "Erro na criacao da thread" << IdExibeDados << endl;
    }
    else {
        cout << "Thread criada com sucesso" << endl;
    }
    hThreads = CreateThread(
        NULL,
        0,
        ThreadFun,
        NULL,
        0,
        &IdLeituraTeclado);

    if (hThreads == NULL) {
        cout << "Erro na criacao da thread" << IdLeituraTeclado << endl;
    }
    else {
        cout << "Thread criada com sucesso" << endl;
    }

    
}

