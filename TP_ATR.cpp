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
#include <process.h>
#include <string>
#include <conio.h>
#include <pthread.h>
#include <semaphore.h>

#define HAVE_STRUCT_TIMESPEC

using namespace std;

typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

// Constantes
#define	ESC				0x1B
#define	TAM_BUFF		200

DWORD WINAPI ThreadLeituraInspecao();		//Thread para Leitura de mensagens provenientes do sistema de inspeção.
DWORD WINAPI ThreadCapturaDados();       //Thread que irá capturar mensagens de dados da thread de leitura
DWORD WINAPI ThreadCapturaDefeitos();       //Thread que irá capturar mensagens de defeitos da thread de leitura
DWORD WINAPI ThreadExibeDefeitos();    //Exibe na tela mensagens de defeitos superficiais obtidos na captura de defeitos.
DWORD WINAPI ThreadExibeDados();      //Exibe na tela mensagens referentes ao processo de laminação.

// Objetos de sincronização
sem_t novoDado;        //Semáforo para indicar que uma nova mensagem foi adicionada no buffer
sem_t posicaoLivre;    //Semáforo para indicar que um novo espaço no buffer está disponível

//Variáveis globais
int nTecla;
string buffer[TAM_BUFF];
int pLivre = 0;
int pDados = 0;
int pDefeitos = 0;
int nseq = 0;

//Funcoes auxiliares
void Wait(sem_t* Semaforo) {
    int status;
    status = sem_wait(Semaforo);
    if (status != 0) {
        printf("Erro na obtencao do semaforo! Codigo = %x\n", errno);
        exit(0);
    }
}

void Signal(sem_t* Semaforo) {
    int status;
    status = sem_post(Semaforo);
    if (status != 0) {
        printf("Erro na liberacao do semaforo! Codigo = %x\n", errno);
        exit(0);
    }
}

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

    int numero = (rand() % 9999) + 1;

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
    int cadeira = (rand() % 6) + 1;
    int gravidade = (rand() % 10) + 1; // Qual deve ser o range?
    int classe = (rand() % 9) + 1;


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
    HANDLE hThreads[3];
    DWORD IdLeituraDados, IdCapturaDados, IdCapturaDefeitos,IdExibeDefeitos,idExibeDados;
    DWORD dwRet;
    int status;

    srand((unsigned int)time(NULL));

    status = sem_init(&novoDado, 0, 0);
    if (status != 0) {
        printf("Erro na inicializacao do semaforo ! Codigo = %d\n", errno);
        exit(0);
    }

    status = sem_init(&posicaoLivre, 0, TAM_BUFF);
    if (status != 0) {
        printf("Erro na inicializacao do semaforo ! Codigo = %d\n", errno);
        exit(0);
    }

    hThreads[0] = (HANDLE) (HANDLE) _beginthreadex(
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
    hThreads[2] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadExibeDefeitos,
        NULL,
        0,
        (CAST_LPDWORD)&IExibeDefeitos);

    if (hThreads[2] == NULL) {
        cout << "Erro na criacao da thread\n" << IdExibeDefeitosDefeitos << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    do {
        nTecla = _getch();
    } while (nTecla != ESC);
    cout << "Esperando encerramento das threads\n" << endl;

    dwRet = WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);

    hThreads[2] = (HANDLE)(HANDLE)_beginthreadex(
        NULL,
        0,
        (CAST_FUNCTION)ThreadExibeDados,
        NULL,
        0,
        (CAST_LPDWORD)&IExibeDados);

    if (hThreads[2] == NULL) {
        cout << "Erro na criacao da thread\n" << IdExibeDados << endl;
    }
    else {
        cout << "Thread criada com sucesso\n" << endl;
    }

    do {
        nTecla = _getch();
    } while (nTecla != ESC);
    cout << "Esperando encerramento das threads\n" << endl;

    dwRet = WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);

    // Fecha todos os handles de objetos do kernel
    for (int i = 0; i < 3; ++i)
        CloseHandle(hThreads[i]);

    cout << "Encerrando thread principal\n" << endl;

    return EXIT_SUCCESS;

}

DWORD WINAPI ThreadLeituraInspecao() {
    do {
        Sleep(10000);

        string dado = MensagemDadosProcesso();
        Wait(&posicaoLivre);
        cout << "Novo dado inserido:" << dado << endl;
        buffer[pLivre] = dado;
        pLivre = (pLivre + 1) % TAM_BUFF;
        Signal(&novoDado);

        string defeito = MensagemDefeito();
        Wait(&posicaoLivre);
        cout << "Novo defeito inserido:" << defeito << endl;
        buffer[pLivre] = defeito;
        pLivre = (pLivre + 1) % TAM_BUFF;
        Signal(&novoDado);

    } while (nTecla != ESC);


    cout << "Thread LeituraInspecao encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);
}

DWORD WINAPI ThreadCapturaDefeitos() {
    do {
        Wait(&novoDado);
        string mensagem = buffer[pDefeitos];
        if (TipoMensagem(mensagem) == 2) {
            cout << "Novo defeito lido:" << mensagem << endl;
            Signal(&posicaoLivre);
        }
        else {
            Signal(&novoDado);
        }
        pDefeitos = (pDefeitos + 1) % TAM_BUFF;

    } while (nTecla != ESC);


    cout << "Thread CapturaInspecao encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);
}

DWORD WINAPI ThreadCapturaDados() {
    do {
        Wait(&novoDado);
        string mensagem = buffer[pDados];
        if (TipoMensagem(mensagem) == 1) {
            cout << "Novo dado lido:" << mensagem << endl;
            Signal(&posicaoLivre);
        }
        else {
            Signal(&novoDado);
        }
        pDados = (pDados + 1) % TAM_BUFF;

    } while (nTecla != ESC);


    cout << "Thread CapturaInspecao encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);
}
DWORD WINAPI ThreadExibeDefeitos() {

    do {
        wait(&novoDado);
        string dprocesso = buffer[pDefeitos];
        if (TipoMensagem(mensagem) == 2) {
            cout << "Defeito detectado: \n" << MensagemDefeito() << endl;
            Signal(&posicaoLivre);
        }
        else {
            Signal(&novoDado);
        }
        pDefeitos = (pDefeitos + 1) % TAM_BUFF;

    } while (nTecla != ESC);

    cout << "Thread ExibeDefeitos encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);

}
DWORD WINAPI ThreadExibeDados() {

    do {
        wait(&novoDado);
        string dprocesso = buffer[pDefeitos];
        if (TipoMensagem(mensagem) == 1) {
            cout << "STATUS-->Processo de laminacao: \n" << MensagemDadosProcesso() << endl;
            Signal(&posicaoLivre);
        }
        else {
            Signal(&novoDado);
        }
        pDados = (pDados + 1) % TAM_BUFF;

    } while (nTecla != ESC);

    cout << "Thread ExibeDefeitos encerrando execucao\n" << endl;
    return(0);
    _endthreadex(0);

}