/*********************************************************************************
*  AUTOMAÇÃO EM TEMPO REAL - ELT127 - TD
*
*  Juliana Arruda Narcizo e Lara Souza Heringer
*
*  Trabalho Final: "Detecção de defeitos em tiras metálicas"
*
*  O programa a seguir é composto pelo processo de exibição de dados na tela
**********************************************************************************/

do {
    nTecla = _getch();

    if (nTecla == I) {
        SetEvent(hIEvent);
    }
    else if (nTecla == D) {
        cout << nTecla << endl;
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
