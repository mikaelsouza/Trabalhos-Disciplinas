#include <stdio.h>
#include <stdlib.h>
#define N_MAX 1027

typedef struct NoArvore{
    int dado;
    struct NoArvore *noEsquerdo;
    struct NoArvore *noDireito;
}NoArvore;

typedef struct ArvoreRaiz{
    NoArvore *topo;
}ArvoreRaiz;

ArvoreRaiz arvore1;
ArvoreRaiz arvore2;

void inicializarArvore(ArvoreRaiz *arvore){
    arvore->topo = NULL;
}

NoArvore* criarNo(int numero) {
    NoArvore* aux;
    aux = (NoArvore *) malloc (sizeof(NoArvore));
    aux->dado = numero;
    aux->noEsquerdo = NULL;
    aux->noDireito = NULL;
    return aux;
}

NoArvore* rotacaoSimplesEsquerda(NoArvore *raiz){
    NoArvore *aux = raiz->noDireito;

    raiz->noDireito = aux->noEsquerdo;
    aux->noEsquerdo = raiz;
    return aux;
}

NoArvore* rotacaoSimplesDireita(NoArvore *raiz){
    NoArvore *aux = raiz->noEsquerdo;

    raiz->noEsquerdo = aux->noDireito;
    aux->noDireito = raiz;
    return aux;
}

NoArvore* balancearArvore(NoArvore *raiz){
    if (raiz == NULL){
        return raiz;
    }
    if(calculaFatorBalanceamento(raiz) == 2){
        if(calculaFatorBalanceamento(raiz->noEsquerdo) == -1){
            raiz->noEsquerdo = rotacaoSimplesEsquerda(raiz->noEsquerdo);
        }
        raiz = rotacaoSimplesDireita(raiz);
    } else if(calculaFatorBalanceamento(raiz) == -2){
        if (calculaFatorBalanceamento(raiz->noDireito) == 1){
            raiz->noDireito = rotacaoSimplesDireita(raiz->noDireito);
        }
        raiz = rotacaoSimplesEsquerda(raiz);
    }
    return raiz;
}

NoArvore* inserirElemento(int numero, NoArvore *raiz){
    if(raiz == NULL){
        return criarNo(numero);
    }
    if(numero < raiz->dado){
        raiz->noEsquerdo = inserirElemento(numero, raiz->noEsquerdo);
    }else if (numero > raiz->dado){
        raiz->noDireito = inserirElemento(numero, raiz->noDireito);
    }
    return balancearArvore(raiz);
}

void inserirNaArvore(int numero, ArvoreRaiz* arvore){
    arvore->topo = inserirElemento(numero, arvore->topo);
}

NoArvore* buscarElemento(int numero, NoArvore *raiz){
    if(raiz == NULL){
        return NULL;
    }
    if (numero < raiz->dado){
        return buscarElemento(numero, raiz->noEsquerdo);
    }
    if(numero > raiz->dado){
        return buscarElemento(numero,raiz->noDireito);
    }
    return raiz;
}

NoArvore* buscarNaArvore(int numero, ArvoreRaiz arvore){
    return buscarElemento(numero, arvore.topo);
}

NoArvore* maiorDosMenores(NoArvore *raiz){
    if (raiz != NULL){
        if(raiz->noDireito != NULL){
            return maiorDosMenores(raiz->noDireito);
        }
    }
    return raiz;
}

NoArvore* menorDosMaiores(NoArvore *raiz){
    if(raiz != NULL){
        if(raiz->noEsquerdo != NULL){
            return menorDosMaiores(raiz);
        }
    }
    return raiz;
}

int verificarFilhos(NoArvore *raiz){
    if ((raiz->noEsquerdo == NULL && raiz->noDireito == NULL) || (raiz->noEsquerdo == NULL) || (raiz->noDireito == NULL))
        return 1;
    else return 0;
}

NoArvore* excluirFilho(NoArvore *raiz){
    if(raiz->noEsquerdo == NULL && raiz->noDireito == NULL){
        free(raiz);
        raiz = NULL;
    }else if(raiz->noEsquerdo == NULL) {
        NoArvore *aux = raiz;
        raiz = raiz->noDireito;
        free(aux);
    }else if(raiz->noDireito == NULL) {
        NoArvore *aux = raiz;
        raiz = raiz->noEsquerdo;
        free(aux);
    }
    return raiz;
}

NoArvore* removerElemento(int numero, NoArvore *raiz){
    if(raiz == NULL) {
        return raiz;
    }
    if (numero < raiz->dado){
        raiz->noEsquerdo = removerElemento(numero, raiz->noEsquerdo);
    }
    if (numero > raiz->dado){
        raiz->noDireito = removerElemento(numero, raiz->noDireito);
    }
    if (numero == raiz->dado) {
        if(verificarFilhos(raiz)){
            raiz = excluirFilho(raiz);
        }else{
            NoArvore *aux = raiz->noEsquerdo;
            aux = maiorDosMenores(aux);
            raiz->dado = aux->dado;
            aux->dado = numero;
            raiz->noEsquerdo = removerElemento(numero, raiz->noEsquerdo);
        }
    }
    return balancearArvore(raiz);
}

void removerDaArvore(int numero, ArvoreRaiz *arvore){
    arvore->topo = removerElemento(numero, arvore->topo);
}

int maior(int numeroA, int numeroB){
    if(numeroA > numeroB){
        return numeroA;
    } else {
        return numeroB;
    }
}

int calculaAltura(NoArvore *raiz){
    if(raiz == NULL){
        return 0;
    } else {
        return (maior(calculaAltura(raiz->noEsquerdo),calculaAltura(raiz->noDireito)) + 1);
    }
}

int calculaFatorBalanceamento(NoArvore *raiz){
    if(raiz == NULL){
        return 0;
    }
    return (calculaAltura(raiz->noEsquerdo) - calculaAltura(raiz->noDireito));
}

void lerDeArquivo(ArvoreRaiz *arvore, char *nomeDoArquivo){
    FILE *arquivo;
    int valor;
    arquivo = fopen(nomeDoArquivo, "r");
    if(arquivo == 0){
        printf("\nOps. Erro ao abrir o arquivo.\nProvavelmente nao existe na pasta do programa.\n\n");
    } else {
        while((fscanf(arquivo,"%d",&valor) != EOF)){
            inserirNaArvore(valor, arvore);
        }
        printf("\nSucesso. Os elementos do arquivo foram inseridos na arvore.\n\n");
    }
    fclose(arquivo);
}

void imprimirElementosOrdenados(NoArvore *raiz){
    if(raiz != NULL){
        imprimirElementosOrdenados(raiz->noEsquerdo);
        printf("%d\n", raiz->dado);
        imprimirElementosOrdenados(raiz->noDireito);
    }
}

int imprimirArvore(NoArvore *raiz){
    int i, alt = calculaAltura(raiz);
    char string[alt][N_MAX*7];

    for (i = 0; i < alt; i++)
        sprintf(string[i], "%80s", " ");

    imprimirArvoreInterno(raiz, 0, 0, 0, alt, string);

    for (i = 0; i < alt; i++)
        printf("%s\n", string[i]);
}

void apagarNo(NoArvore *no){
    if(no != NULL){
        apagarNo(no->noEsquerdo);
        apagarNo(no->noDireito);
        free(no);
    }
}

void apagarArvore(ArvoreRaiz *arvore){
    apagarNo(arvore->topo);
    arvore->topo = NULL;
}

void concatenaNo(NoArvore *no, ArvoreRaiz *raiz){

    if(no != NULL){
        inserirElemento(no->dado, raiz->topo);
        concatenaNo(no->noEsquerdo, raiz);
        concatenaNo(no->noDireito, raiz);
    }

}
void concatenaArvores(ArvoreRaiz *raiz1, ArvoreRaiz *raiz2){
    concatenaNo(raiz2->topo, raiz1);
    apagarArvore(raiz2);
}

void imprimirArvoreOrdenadaInterno(NoArvore *raiz){
    if(raiz != NULL){
        imprimirArvoreOrdenadaInterno(raiz->noEsquerdo);
        printf("%d ", raiz->dado);
        imprimirArvoreOrdenadaInterno(raiz->noDireito);
    }
}


void imprimirArvoreOrdenada(ArvoreRaiz *arvore){
    imprimirArvoreOrdenadaInterno(arvore->topo);
}

int imprimirArvoreInterno(NoArvore *raiz, int e_esquerda, int distancia, int nivelAtual, int alt, char str[][N_MAX*7]){
    char strAux[alt];
    int i, esquerda, direita, tamanhoNo = 7; //tamanhoNo: tamanho do nó que será impresso. No caso, um inteiro de 4 posiçoes + parênteses

    if (!raiz) return 0;

    sprintf(strAux, "(%05d)", raiz->dado);

      esquerda  = imprimirArvoreInterno(raiz->noEsquerdo,  1, distancia, nivelAtual + 1, alt, str);
      direita = imprimirArvoreInterno(raiz->noDireito, 0, distancia + esquerda + tamanhoNo, nivelAtual + 1, alt, str);

    for (i = 0; i < tamanhoNo; i++)
        str[nivelAtual][distancia + esquerda + i] = strAux[i];

    if (nivelAtual && e_esquerda) {

        for (i = 0; i < tamanhoNo + direita; i++)
            str[nivelAtual - 1][distancia + esquerda + tamanhoNo/2 + i] = '-';

        str[nivelAtual - 1][distancia + esquerda + tamanhoNo/2] = '.';

    } else if (nivelAtual && !e_esquerda) {

        for (i = 0; i < esquerda + tamanhoNo; i++)
            str[nivelAtual - 1][distancia - tamanhoNo/2 + i] = '-';

        str[nivelAtual - 1][distancia + esquerda + tamanhoNo/2] = '.';
    }

    return esquerda + tamanhoNo + direita;
}

void imprimirOpcoes(int menu){

    if(menu == 0){
        printf("\n----------------------------- ARVORE AVL -----------------------------\n");
        printf("\nEscolha a opcao que gostaria de utilizar:\n");
        printf("1 - Inserir elementos em uma arvore.\n");
        printf("2 - Remover elemento de uma arvore.\n");
        printf("3 - Imprimir elementos da arvore ordenados.\n");
        printf("4 - Concatenar duas arvores.\n");
        printf("5 - Buscar um elemento em uma arvore\n");
        printf("6 - Imprimir arvore no formato de arvore\n");
        printf("0 - Sair do programa.\n\n");
        printf("Opcao: ");
    }

    if(menu == 1){
        printf("\nInserir elementos na:\n");
        printf("1 - Arvore 1\n");
        printf("2 - Arvore 2\n");
    }

    if(menu == 2){
        printf("\nRemover elemento da:\n")/
        printf("1 - Arvore 1\n");
        printf("2 - Arvore 2\n");
    }
    if(menu == 3){
        printf("\nImprimir:\n");
        printf("1 - Arvore 1\n");
        printf("2 - Arvore 2\n");
    }
    if(menu == 4){
            printf("\nDefina como sera feita a concatenacao:\n");
            printf("1 - Concatenar Arvore 2 a Arvore 1\n");
            printf("2 - Concatenar Arvore 1 a Arvore 2\n");
    }
    if(menu == 5){
        printf("\nBuscar elemento na:\n");
        printf("1 - Arvore 1\n");
        printf("2 - Arvore 2\n");
    }
    if(menu == 6){
        printf("\nImprimir:\n");
        printf("\nNota: Esta funcao foi encontrada na internet e esta\ndisponivel apenas para o teste de balanceamento\nutilize arvore com poucos elementos\n\n");
        printf("1 - Arvore 1\n");
        printf("2 - Arvore 2\n");
    }
}

void menuInsercaoArvore(){
    int opcao = -1;
    char nomeDoArquivo[10];
    imprimirOpcoes(1);
    scanf("%d", &opcao);
    switch(opcao){
        case 1:
            printf("\nNome do arquivo para insercao de elementos na arvore:\nO arquivo deve ter no maximo 10 caracteres no nome\n");
            scanf("%s", nomeDoArquivo);
            lerDeArquivo(&arvore1, nomeDoArquivo);
            break;
        case 2:
            printf("\nNome do arquivo para insercao de elementos na arvore:\nO arquivo deve ter no maximo 10 caracteres no nome\n");
            scanf("%s", nomeDoArquivo);
            lerDeArquivo(&arvore2, nomeDoArquivo);
        default:
            printf("Opcao Invalida.\n");
    }
}

void menuRemocaoArvore(){
    NoArvore *aux;
    int opcao = -1;
    int elemento;
    imprimirOpcoes(2);
    scanf("%d", &opcao);
    switch(opcao){
        case 1:
            printf("\nElemento para remover da arvore: ");
            scanf("%d", &elemento);
            aux = buscarNaArvore(elemento, arvore1);
            if(aux){
                removerDaArvore(elemento, &arvore1);
                printf("\nSucesso. O elemento foi removido.\n");
            } else printf("\nEste elemento nao esta na arvore.\n");
            break;
        case 2:
            printf("\nElemento para remover da arvore: ");
            scanf("%d", &elemento);
            aux = buscarNaArvore(elemento, arvore2);
            if(aux){
                removerDaArvore(elemento, &arvore2);
                printf("\nSucesso. O elemento foi removido.\n");
            } else printf("\nEste elemento nao esta na arvore.\n");
            break;
        default:
            printf("Opcao Invalida.\n\n");
    }
}

void menuImpressaoArvoreOrdenado(){
    int opcao;
    imprimirOpcoes(3);
    scanf("%d", &opcao);
    switch(opcao){
        case 1:
            if(arvore1.topo == NULL){
                printf("Arvore vazia!\n");
            } else {
                imprimirArvoreOrdenada(&arvore1);
            }
            break;
        case 2:
            if(arvore2.topo == NULL){
                printf("Arvore vazia!\n");
            } else {
                imprimirArvoreOrdenada(&arvore2);
            }
            break;
        default:
            printf("Opcao Invalida\n\n");
    }
}

void menuImpressaoArvore(){
    int opcao;
    imprimirOpcoes(6);
    scanf("%d", &opcao);
    switch(opcao){
        case 1:
            if(arvore1.topo == NULL){
                printf("Arvore vazia!\n");
            } else {
                imprimirArvore(arvore1.topo);
            }
            break;
        case 2:
            if(arvore2.topo == NULL){
                printf("Arvore vazia!\n");
            } else {
                imprimirArvore(arvore2.topo);
            }
            break;
        default:
            printf("Opcao Invalida\n\n");
    }
}

void menuConcatenacao(){
    int opcao;
    imprimirOpcoes(4);
    scanf("%d", &opcao);
    switch(opcao){
        case 1:
            if(arvore1.topo && arvore2.topo){
                concatenaArvores(&arvore1, &arvore2);
                printf("\nConcatenacao realizada com sucesso.\n");
            } else printf("\nUma ou as duas arvores estao vazias.\n");

            break;
        case 2:
            if(arvore1.topo && arvore2.topo){
                concatenaArvores(&arvore2, &arvore1);
                printf("\nConcatenacao realizada com sucesso.\n");
            } else printf("\nUma ou as das arvores esta vazias.\n");
            break;
        default:
            printf("Opcao invalida.\n\n");
    }
}

void elementoEncontrado(int elemento, int caso){
    NoArvore *aux;
    if(caso == 1){
        aux = buscarNaArvore(elemento, arvore1);
        if(aux){
            printf("\nElemento encontrado!\n");
            printf("   Altura: %d\n", calculaAltura(aux));
            printf("   Fator balanceamento: %d\n\n", calculaFatorBalanceamento(aux));
        } else {
            printf("\nElemento nao encontrado!\n");
        }
    } else {
        aux = buscarNaArvore(elemento, arvore2);
        if(aux){
            printf("\nElemento encontrado!\n");
            printf("   Altura: %d\n", calculaAltura(aux));
            printf("   Fator balanceamento: %d\n\n", calculaFatorBalanceamento(aux));
        } else {
            printf("\nElemento nao encontrado!\n");
        }
    }
}

void menuBuscaArvore(){
    int opcao;
    int elemento;
    imprimirOpcoes(5);
    scanf("%d", &opcao);
    switch(opcao){
        case 1:
            printf("\nElemento para buscar: ");
            scanf("%d", &elemento);
            elementoEncontrado(elemento, 1);
            break;
        case 2:
            printf("\nElemento para buscar: ");
            scanf("%d", &elemento);
            elementoEncontrado(elemento, 1);
            break;
        default:
            printf("Opcao invalida.\n\n");
            break;
    }
}

void menuInicial(){

    int opcao = -1;
    while(opcao != 0){
        imprimirOpcoes(0);
        scanf("%d", &opcao);
        switch(opcao){
            case 1:
                menuInsercaoArvore();
                break;
            case 2:
                menuRemocaoArvore();
                break;
            case 3:
                menuImpressaoArvoreOrdenado();
                break;
            case 4:
                menuConcatenacao();
                break;
            case 5:
                menuBuscaArvore();
                break;
            case 6:
                menuImpressaoArvore();
                break;
            case 0:
                break;
        }
    }
}

int main(){

    inicializarArvore(&arvore1);
    inicializarArvore(&arvore2);

    menuInicial();

    return 0;
}
