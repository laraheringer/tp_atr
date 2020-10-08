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
#include <string>
#include <conio.h>
#include <process.h>				
#include <stdio.h>
#include <list>
#define HAVE_STRUCT_TIMESPEC

using namespace std;

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

// Constantes
#define	ESC				0x1B
#define	TAM_BUFF		4
#define	I		        0x69
#define	D		        0x64
#define	E		        0x65
#define	A		        0x61
#define	L     	        0x6c
#define	C		        0x63

DWORD WINAPI ThreadLeituraInspecao();		//Thread para Leitura de mensagens provenientes do sistema de inspeção.
DWORD WINAPI ThreadCapturaDados();       //Thread que irá capturar mensagens de dados da thread de leitura
DWORD WINAPI ThreadCapturaDefeitos();       //Thread que irá capturar mensagens de defeitos da thread de leitura
DWORD WINAPI ThreadLeituraTeclado();       //Thread que irá ler as entradas do usuario


// Objetos de sincronização
HANDLE novoDado;        //Semáforo para indicar que uma nova mensagem foi adicionada no buffer
HANDLE posicaoLivre;    //Semáforo para indicar que um novo espaço no buffer está disponível
HANDLE hMutex;          // Mutex de proteçao a variável pLivre

HANDLE hEscEvent;     // Evento que sinaliza o termino da execução
HANDLE hIEvent;       // Evento que sinaliza o bloqueio da Thread de Leitura
HANDLE hDEvent;      // Evento que sinaliza o bloqueio da Thread de Captura de Defeitos
HANDLE hEEvent;      // Evento que sinaliza o bloqueio da Thread Captura de Dados do Processo
HANDLE hLEvent;     // Evento que sinaliza o bloqueio da Thread de Exibição de Dados
HANDLE hAEvent;     // Evento que sinaliza o bloqueio da Thread de Exibição de Defeitos
HANDLE hCEvent;      //Evento que indica que a Thread Exibiçao de Dados deve limpar o console
HANDLE hDadoEvent;      // Sinaliza a Thread Captura Dados que há um dado a ser lido
HANDLE hDefeitoEvent;      // Sinaliza a Thread Captura Defeitos que há um defeito a ser lido

//Variáveis globais
int nTecla;
list <string> buffer;
list <string> ::iterator pDado;
list <string> ::iterator pDefeito;
int nseq = 0;

//Funcoes auxiliares
string CompletaZeros(int numero, int digitos) {
    string numString = to_string(numero);
    int nZeros = digitos - numString.length();

    for (int i = 0; i < nZeros; i++) {
        numString = "0" + numString;
    }

    return(numString);
}


string GeraAlfaNumerico(int nCaracteres, bool hifen = false) {
    string alfaNumerico = "";

    for (int i = 0; i < nCaracteres; i++) {
        alfaNumerico += 'A' + rand() % 26;
    }

    int numero = rand() % 9999 + 1;

    alfaNumerico += hifen ? "-" + CompletaZeros(numero, 4) : CompletaZeros(numero, 4);

    return alfaNumerico;
}

string HoraLocal() {
    SYSTEMTIME time;
    GetLocalTime(&time);

    return to_string(time.wHour) + ":" + to_string(time.wMinute) + ":" + to_string(time.wSecond) + ":" + to_string(time.wMilliseconds);
}

string MensagemDefeito() {
    string mensagem;
    nseq == 99999 ? nseq = 1 : nseq++;
    int cadeira = rand() % 6 + 1;
    int gravidade = rand() % 10 + 1; // Qual deve ser o range?
    int classe = rand() % 9 + 1;


    mensagem = CompletaZeros(nseq, 5) + "/22/" + CompletaZeros(cadeira, 2) + "/" + CompletaZeros(gravidade, 2) + "/" + to_string(classe) + "/" + GeraAlfaNumerico(2) + "/" + HoraLocal();
    return(mensagem);
}

string GeraNumeroRealEmString(int maxNumero) {
    float numero = rand() / (RAND_MAX / (float)maxNumero);
    string numStr = to_string(numero);

    numStr.resize(5);

    return numStr;
}

string MensagemDadosProcesso() {
    string mensagem;
    nseq == 99999 ? nseq = 1 : nseq++;
    int cadeira = rand() % 6 + 1;
    int classe = rand() % 9 + 1;
    string temp = GeraNumeroRealEmString(999);
    string vel = GeraNumeroRealEmString(999);

    mensagem = CompletaZeros(nseq, 5) + "/11/" + CompletaZeros(cadeira, 2) + "/" + GeraAlfaNumerico(3, true) + "/" + temp + "/" + vel + "/" + HoraLocal();
    return(mensagem);
}

int TipoMensagem(string mensagem) {
    char tipo = mensagem[6];
    return ((int)tipo) - ((int)'0');
}

//THREAD PRIMÁRIA
int main()
{
    HANDLE hThreads[4];
    DWORD IdLeituraDados, IdCapturaDados, IdCapturaDefeitos, IdLeituraTeclado;
    DWORD dwRet;

    srand((unsigned int)time(NULL));

    novoDado = CreateSemaphore(NULL, 0, TAM_BUFF, "NovoDado");
    posicaoLivre = CreateSemaphore(NULL, TAM_BUFF, TAM_BUFF, "PosicaoLivre");
    hEscEvent = CreateEvent(NULL, TRUE, FALSE, "EscEvento");
    hIEvent = CreateEvent(NULL, TRUE, FALSE, "IEvento");
    hDEvent = CreateEvent(NULL, TRUE, FALSE, "DEvento");
    hEEvent = CreateEvent(NULL, TRUE, FALSE, "EEvento");
    hLEvent = CreateEvent(NULL, TRUE, FALSE, "LEvento");
    hAEvent = CreateEvent(NULL, TRUE, FALSE, "AEvento");
    hCEvent = CreateEvent(NULL, TRUE, FALSE, "CEvento");
    hDefeitoEvent = CreateEvent(NULL, TRUE, FALSE, "DefeitoEvento");
    hDadoEvent = CreateEvent(NULL, TRUE, FALSE, "DadoEvento");
    hMutex = CreateMutex(NULL, FALSE, "hMutex");

    BOOL status;
    STARTUPINFO si;
    PROCESS_INFORMATION NewProcess;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    status = CreateProcess(
        "Debug\\ProcessoExibeDefeitos.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        "Debug",
        &si,
        &NewProcess);
    if (!status)
        cout << "Erro na criacao do processo Exibicao de Defeitos!  ERRO: " << GetLastError() << endl;
    else
        cout << "Processo Exibicao de defeitos criado com sucesso! " << endl;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    status = CreateProcess(
        "Debug\\ProcessoExibeDados.exe",
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        "Debug",
        &si,
        &NewProcess);
    if (!status)
        cout << "Erro na criacao do processo Exibicao de Dados!  ERRO: " << GetLastError() << endl;
    else
        cout << "Processo Exibicao de dados criado com sucesso! " << endl;


    hThreads[0] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadLeituraInspecao,
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
        (CAST_FUNCTION)ThreadCapturaDados,
        NULL,
        0,
        (CAST_LPDWORD)&IdCapturaDados);

    if (hThreads[1] == NULL) {
        cout << "Erro na criacao da thread\n" << IdCapturaDados << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    hThreads[2] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadCapturaDefeitos,
        NULL,
        0,
        (CAST_LPDWORD)&IdCapturaDefeitos);

    if (hThreads[2] == NULL) {
        cout << "Erro na criacao da thread\n" << IdCapturaDefeitos << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    hThreads[3] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadLeituraTeclado,
        NULL,
        0,
        (CAST_LPDWORD)&IdLeituraTeclado);

    if (hThreads[3] == NULL) {
        cout << "Erro na criacao da thread\n" << IdLeituraTeclado << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }


    cout << "Esperando encerramento das threads\n" << endl;

    dwRet = WaitForMultipleObjects(4, hThreads, TRUE, INFINITE);

    // Fecha todos os handles de objetos do kernel
    for (int i = 0; i < 4; ++i)
        CloseHandle(hThreads[i]);

    CloseHandle(posicaoLivre);
    CloseHandle(novoDado);
    CloseHandle(hEscEvent);
    CloseHandle(hIEvent);
    CloseHandle(hDEvent);
    CloseHandle(hEEvent);

    cout << "Encerrando thread principal\n" << endl;

    return EXIT_SUCCESS;
}

DWORD WINAPI ThreadLeituraInspecao() {
    HANDLE Events[3] = { hEscEvent, hIEvent, posicaoLivre };
    HANDLE BloqueioEventos[2] = { hEscEvent, hIEvent };
    DWORD status;
    int nTipoEvento;

    while (TRUE) {
        Sleep(3000);

        // Gera e coloca no buffer a mensagem de dados do processo
        string dado = MensagemDadosProcesso();
        cout << "Aguardando posicao livre\n" << endl;
        status = WaitForMultipleObjects(3, Events, FALSE, INFINITE);
        nTipoEvento = status - WAIT_OBJECT_0;

        if (nTipoEvento == 0) break;

        if (nTipoEvento == 1) {
            cout << "Thread LeituraInspecao bloqueando a pedido do usuario\n" << endl;
            ResetEvent(hIEvent);
            status = WaitForMultipleObjects(2, BloqueioEventos, FALSE, INFINITE);
            nTipoEvento = status - WAIT_OBJECT_0;
            if (nTipoEvento == 0) break;
            ResetEvent(hIEvent);
            cout << "Thread LeituraInspecao acordando a pedido do usuario\n" << endl;
        }

        cout << "Novo dado inserido:" << dado << "\n" << endl;
        WaitForSingleObject(hMutex, INFINITE);
        buffer.push_back(dado);
        if (buffer.size() == 1) {
            pDado = buffer.begin();
            pDefeito = buffer.begin();
        }
        ReleaseMutex(hMutex);
        ReleaseSemaphore(novoDado, 1, NULL);

        // Gera e coloca no buffer a mensagem de defeitos
        string defeito = MensagemDefeito();
        cout << "Aguardando posicao livre\n" << endl;
        status = WaitForMultipleObjects(3, Events, FALSE, INFINITE);
        nTipoEvento = status - WAIT_OBJECT_0;

        if (nTipoEvento == 0) break;

        if (nTipoEvento == 1) {
            cout << "Thread LeituraInspecao bloqueando a pedido do usuario\n" << endl;
            ResetEvent(hIEvent);
            status = WaitForMultipleObjects(2, BloqueioEventos, FALSE, INFINITE);
            nTipoEvento = status - WAIT_OBJECT_0;
            if (nTipoEvento == 0) break;
            ResetEvent(hIEvent);
            cout << "Thread LeituraInspecao acordando a pedido do usuario\n" << endl;
        }

        cout << "Novo defeito inserido:" << defeito << "\n" << endl;
        WaitForSingleObject(hMutex, INFINITE);
        buffer.push_back(defeito);
        if (buffer.size() == 1) {
            pDado = buffer.begin();
            pDefeito = buffer.begin();
        }
        ReleaseMutex(hMutex);
        ReleaseSemaphore(novoDado, 1, NULL);
    }

    cout << "Thread LeituraInspecao encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);
}

DWORD WINAPI ThreadCapturaDefeitos() {
    HANDLE Events[4] = { hEscEvent, hDEvent, novoDado, hDefeitoEvent };
    HANDLE BloqueioEventos[2] = { hEscEvent, hDEvent };
    DWORD status;
    int nTipoEvento;

    while (TRUE) {
        status = WaitForMultipleObjects(4, Events, FALSE, INFINITE);
        nTipoEvento = status - WAIT_OBJECT_0;

        if (nTipoEvento == 0) break;

        if (nTipoEvento == 1) {
            cout << "Thread CapturaDefeitos bloqueando a pedido do usuario\n" << endl;
            ResetEvent(hDEvent);
            status = WaitForMultipleObjects(2, BloqueioEventos, FALSE, INFINITE);
            nTipoEvento = status - WAIT_OBJECT_0;
            if (nTipoEvento == 0) break;
            ResetEvent(hDEvent);
            cout << "Thread CapturaDefeitos acordando a pedido do usuario\n" << endl;
        }
        else {
            WaitForSingleObject(hMutex, INFINITE);
            if (pDefeito != buffer.end()) {
                string mensagem = *pDefeito;
                if (TipoMensagem(mensagem) == 2) {
                    cout << "Novo defeito lido:" << mensagem << "\n" << endl;
                    bool pDadoInvalidado = pDefeito == pDado;
                    pDefeito = buffer.erase(pDefeito);
                    if (pDadoInvalidado) pDado = pDefeito;
                    ReleaseSemaphore(posicaoLivre, 1, NULL);
                }
                else {
                    pDefeito++;
                    SetEvent(hDadoEvent);
                }
            }
            else {
                ResetEvent(hDefeitoEvent);
            }
            ReleaseMutex(hMutex);
        }
    }

    cout << "Thread CapturaDefeitos encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);
}

DWORD WINAPI ThreadCapturaDados() {
    HANDLE Events[4] = { hEscEvent, hEEvent, novoDado, hDadoEvent };
    HANDLE BloqueioEventos[2] = { hEscEvent, hEEvent };
    DWORD status;
    int nTipoEvento;

    while (TRUE) {
        status = WaitForMultipleObjects(4, Events, FALSE, INFINITE);
        nTipoEvento = status - WAIT_OBJECT_0;

        if (nTipoEvento == 0) break;

        if (nTipoEvento == 1) {
            cout << "Thread CapturaDados bloqueando a pedido do usuario\n" << endl;
            ResetEvent(hEEvent);
            status = WaitForMultipleObjects(2, BloqueioEventos, FALSE, INFINITE);
            nTipoEvento = status - WAIT_OBJECT_0;
            if (nTipoEvento == 0) break;
            ResetEvent(hEEvent);
            cout << "Thread CapturaDados acordando a pedido do usuario\n" << endl;
        }
        else {
            WaitForSingleObject(hMutex, INFINITE);
            if (pDado != buffer.end()) {
                string mensagem = *pDado;
                if (TipoMensagem(mensagem) == 1) {
                    cout << "Novo dado lido:" << mensagem << "\n" << endl;
                    bool pDefeitoInvalidado = pDefeito == pDado;
                    pDado = buffer.erase(pDado);
                    if (pDefeitoInvalidado) pDefeito = pDado;
                    ReleaseSemaphore(posicaoLivre, 1, NULL);
                }
                else {
                    pDado++;
                    SetEvent(hDefeitoEvent);
                }
            }
            else {
                ResetEvent(hDadoEvent);
            }
            ReleaseMutex(hMutex);
        }
    }


    cout << "Thread CapturaDados encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);
}

DWORD WINAPI ThreadLeituraTeclado() {


    do {
        nTecla = _getch();

        if (nTecla == I) {
            SetEvent(hIEvent);
        }
        else if (nTecla == D) {
            SetEvent(hDEvent);
        }
        else if (nTecla == E) {
            SetEvent(hEEvent);
        }
        else if (nTecla == A) {
            SetEvent(hAEvent);
        }
        else if (nTecla == L) {
            SetEvent(hLEvent);
        }
        else if (nTecla == C) {
            SetEvent(hCEvent);
        }

    } while (nTecla != ESC);
    SetEvent(hEscEvent);

    cout << "Thread LeituraTeclado encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);
}