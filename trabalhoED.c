/** FELIPE LEIVAS
    OTAVIO JACOBI - 261569
    Trabalho ED - 2015/2
    Sugestão de Palavras

    O programa recebe dois arquivos de texto, e gera um terceiro como resultado, o nome dos arquivos deve ser informado na abertura
  do programa pela linha de comando. Ele organiza todas as palavras do texto um em uma ABP simples, onde cada nó é uma estrutura,
  onde tem a palavra como id, sua frequência, suas ramificações e uma lista de estruturas onde o id são as palavras que seguem o
  id da arvore, também tem a medida estatística de associação entre palavras, e o próximo elemento da lista, esta lista está ordenada
  pela medida de associação entre as palavras, e a ABP por ordem alfabética.

    O programa primeiro bota todas as palavras dos textos para minúsculo e tira toda as formas de pontuação, depois ele monta uma ABP com
  essas palavras, e toda vez que uma palavra é inserida na arvore, ele adiciona a palavra seguinte a ela, no campo vizinhos, onde é montado
  uma lista de estrutura, caso a palavra vizinha já esteja ela não é repetida. Depois ele ordena a lista de palavras vizinhas, e consulta no
  segundo texto fornecido as palavras que ele deve procurar e mostrar as sugestões, depois de fazer isso ele imprime cada palavra e sugestão
  em um terceiro arquivo, que o nome é dado pelo terceiro parâmetro na chamada da função e o programa é encerrado.

    Escolhemos ABP, para representar as palavras do texto, por que como a inserção é feita várias vezes, e dependendo do tamanho do texto serão
  inseridas muitas palavras ela acabara sendo rápida para a inserção, e também será rápida para a consulta porque é uma ABP, mesmo que meio
  desbalanceada. E como provavelmente serão pesquisadas muito menos palavras que inseridas, a ABP se mostra mais eficiente nessa situação.

    Escolhemos uma Lista, para representar as palavras adjacentes, pois quando ordenamos elas pela medida de associação, quando formos consultar as
  palavras para servir de sugestão, é só ir seguindo a lista, que ela retornara as palavras que devem ser informadas.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <locale.h>
#include <time.h>
#include <windows.h>

typedef struct lista
{
    char id[50];
    long int freq_ab;
    double  moda;
    struct lista *prox;

} Vizinhos;

typedef struct tree
{
    char id[50];
    long int freq;
    Vizinhos *vizinhos;
    struct tree *esq;
    struct tree *dir;
} Words;


void stolower(char str[]);
void Desenha(Words *a , int nivel);
Words* abp(Words *a,char *x);
Vizinhos* insere_lista(Vizinhos *aux,char *x);
Words* busca (Words* a,char *x );
void calcula(Vizinhos* a,int f_a,  Words* aux);
void moda(Words* a,Words* aux);
void ordena(Words* a);
Vizinhos* ordena_lista(Vizinhos *a);
void deltree(Words *tree);


int main(int argc, char *argv[])
{
    clock_t start, end, elapsed;
    FILE *entrada, *entrada2, *copia, *consulta, *saida;
    Words *words=NULL, *aux=NULL;
    char linha[1024],linha2[1024];
    char delimiters[] = " .,;:!?'\n'-*";
    char *token=NULL, *cp, c, n,aspas=(char)39,tab=(char)9;
    int i=0, ok=0,okay=0;



    if (argc != 5)         //numero de parametros corretos
    {
        printf("Numero incorreto de parametros.\n Sintaxe: copia arq_origem arq_destino\n");
        return 1;
    }

    entrada = fopen(argv[1], "r");       //abre o primeiro arquivo (arquivo de entrada)
    copia = fopen("facil.txt", "w");     //arquivo auxiliar
    consulta = fopen(argv[2], "r");      //abre o arquivo de consulta
    saida = fopen (argv[3], "w");        //abre ou cria ou arquivo de saida

    if(!entrada)                         //caso arquivos não existam
    {
        printf("ERRO\n");
        return 1;
    }
    if(!consulta)
    {
        printf("ERRO\n");
        return 1;
    }

    while(!feof(entrada))                //cria arquivo auxiliar a ser usado sem pontos, e caracteres separadores especiais
    {
        c=fgetc(entrada);
        if((c==',')||(c==':')||(c==';')||(c=='.')||(c=='-')||(c==EOF)||(c=='!')||(c=='?')||(c=='"')||(c=='*')||(c==aspas)||(c=='(')||(c==')')||(c=='_') || c==tab)
            c=' ';

        fputc(tolower(c),copia);
    }

    fclose(entrada);
    fclose(copia);


    entrada = fopen("facil.txt", "r");
    entrada2 = fopen("facil.txt", "r");

    if(!entrada)
        printf("ERRO\n");

    ///AQUI COLOCA OS ELEMENTOS NA ABP
    else
    {
        while(fgets(linha,1024, entrada))
        {

            stolower(linha);   //função que transforma tudo para minusculo

            cp = strdup(linha);

            token = strtok (cp, delimiters);    //pega tokens (usa cp como auxiliar para caso o ponteiro fique iinvalido em algum momento)
            while(token!=NULL)
            {
                words = abp(words,token);          //insere na abp
                token = strtok (NULL, delimiters); //pega proximo token

            }
        }
    }
    words = abp(words, "(NULL)");  //insere a palavra "(NULL)" na abp, para ser apontada pela ultima palavra do texto


    aux = words;                   //ponteiro aux fixo no inicio da ABP
    fseek(entrada, 0, SEEK_SET);
    n=fscanf(entrada2,"%s",linha2);//inicia o arquivo auxiliar uma palavra na frente


    ///AQUI COLOCA OS ELEMTTOS NO CAMPO LISTA DO NÓ DA ABP
    while(n != EOF)
    {
        words = aux;

        n=fscanf(entrada2,"%s",linha2);
        c=fscanf(entrada,"%s",linha);

        if(n == EOF)
        {
            words=busca(words,linha);                               //caso seja a ultima palavra do texto, busca ela na árvore
            words->vizinhos=insere_lista(words->vizinhos,"(NULL)"); //insere ela apontando também para ("NULL")
            break;
        }

        words=busca(words,linha);                             //busca a palavra no texto

        words->vizinhos=insere_lista(words->vizinhos,linha2); //insere ela apontando para sua sucessora
    }

   // printf("iuti");
    fclose(entrada2);
    fclose(entrada);
    moda(aux,aux);  //calcula a medida estatística para cada elemento da lista
    ordena(aux);    //ordena a lista pelo valor de sua "moda"

    ///AQUI FAZ A BUSCA E GERA SAIDA
    start=clock();
    fseek(consulta,0,SEEK_SET);
    while(fgets(linha,1023,consulta))
    {

        words=aux;                     //volta para o inicio da árvore
        strcpy(linha, strtok(linha,delimiters)); //paga pegar a linha correta
        words=busca(words,linha);      //vai para o lugar da arvore daquela palavra

        if(words!=NULL)
        {
            fputs("Consulta: ", saida);
            fputs(words->id, saida);
            fputs("\n", saida);

            i=0;
            while(words->vizinhos!=NULL && i<atoi(argv[4]))
            {
                fprintf(saida,"Sugestao: %s",words->vizinhos->id);  //printa tudo bonitinho
                okay=strlen(words->vizinhos->id);
                for(ok=0; ok<30-okay; ok++)
                    fprintf(saida," ");
                fprintf(saida,"(%lf)\n",words->vizinhos->moda);
                words->vizinhos = words->vizinhos->prox;
                i++;
            }
            fputs("\n", saida);

        }
    }

    fputs("\n", saida);
    end=clock(); //lê o tempo final
    elapsed = 1000 * (end - start) / (CLOCKS_PER_SEC);
    fprintf(saida,"Gasto: %ld ms", elapsed); //mostra o tempo final em ms

    fclose(consulta);
    fclose(saida);

    //Desenha(aux,1);

    deltree(aux);

    return 0;
}
/*Retorna uma string toda em lower case*/
void stolower(char str[])
{
    int i;

    for(i=0; i<strlen(str); i++)
        str[i] = tolower(str[i]);
}

/*Função recursiva para inserir em uma ABP*/
Words* abp(Words *a,char *x)
{
    if(a == NULL)       //malloca espaço, e cria o nodo
    {
        a = (Words*) malloc(sizeof(Words));
        strcpy(a->id,x);
        a->vizinhos=NULL;
        a->esq = NULL;
        a->dir = NULL;
        a->freq=1;
    }

    else
    {
        if(strcmp(x,a->id)>0)         //para recursão a esquerda
            a->esq=abp(a->esq,x);
        else if(strcmp(x,a->id)<0)    //para recursão a direita
            a->dir=abp(a->dir,x);
        else if(strcmp(x,a->id)==0)   //para palavras iguais, adiciona 1 no seu contador
            a->freq = a->freq + 1;

    }
    return a;
}

/*FUNÇÃO AUXILIAR, utilizada basicamente para debugar o programa ao decorrer do desenvolvimento
Extremamente util caso queira se adicionar algo novo e testar, por isso mantida aqui, NÃO é utilizada (normalmente) no main do programa com outras finalidades
printa toda a árvore, com todas as palavras e sua moda*/
void Desenha(Words *a , int nivel)
{
    Vizinhos *aux=a->vizinhos;

    if(a != NULL)
    {

        if (a->dir != NULL) Desenha(a->dir, (nivel+1));
        if(a->vizinhos!=NULL)
            printf("%s %ull ", a->id, a->freq);
        while(aux!=NULL)
        {
            printf("%s %lf ",aux, aux->moda);

            aux= aux->prox;
        }
        printf("\n");
        printf("\n");
        if (a->esq != NULL) Desenha(a->esq, (nivel+1));
    }
}

/*Função que insere uma nova palavra no final de uma lista*/
Vizinhos *insere_lista(Vizinhos *inicio,char *x)
{

    Vizinhos *nova=NULL, *aux;

    aux=inicio;
    if(aux == NULL)    //malloca espaço para o nodo caso a lista seja vazia
    {
        nova= (Vizinhos*) malloc(sizeof(Vizinhos));
        nova->freq_ab = 1;
        nova->prox = NULL;
        strcpy(nova->id, x);
        inicio = nova;
    }

    else
    {
        while((strcmp(aux->id,x)!=0) && aux->prox!=NULL)  //vai até o final da lista para inserir o novo elemento
            aux = aux->prox;

        if(strcmp(aux->id,x)!=0) //malloca espaço para o novo nodo caso já exista algum elemento na lista
        {
            nova= (Vizinhos*) malloc(sizeof (Vizinhos));
            nova->freq_ab = 1;
            nova->prox = NULL;
            strcpy(nova->id, x);
            aux->prox = nova;
        }

        else
            aux->freq_ab = aux->freq_ab +1;
    }

    return inicio;

}

/*Função que procura determinada palavra (chave) em uma ABP*/
Words* busca (Words* a,char *x )
{
    Words *aux;
    aux = a;
    if(a != NULL)
    {
        if(strcmp(x,a->id)>0)         //recursão a esquerda
            return busca (a->esq,x);
        else if(strcmp(x,a->id)<0)    //recursão a direita
            return busca (a->dir,x);
        else if(strcmp(x,a->id)==0)   //caso ache o elemento
            return a;
    }
    return aux;
}

/*FUNÇÃO AUXILIAR: usada para calcular a relação estatística*/
void calcula(Vizinhos* a,int f_a,  Words* aux)
{
    int f_b, f_ab;
    float moda=0;

    if(a!=NULL)
    {
        calcula(a->prox,f_a,aux);

        f_ab = a->freq_ab;
        f_b = busca(aux,a->id)->freq;
        moda= f_ab/(sqrt(f_a*f_b));
        a->moda = moda;

    }

}
/*Calcula a relação estatística para cada um dos elementos da lista*/
void moda(Words* a,Words* aux)
{
    if(a!= NULL)
    {
        calcula(a->vizinhos,a->freq, aux);
        moda(a->esq, aux);
        moda(a->dir,aux);
    }
}

/*FUNÇÃO AUXILIAR: BUBBLE SORT em uma LSE*/
Vizinhos* ordena_lista(Vizinhos *a)
{

    if(a == NULL || a->prox == NULL)
        return a;


    Vizinhos *atual, *maior,*ant_maior,*anterior,*tmp;

    atual = a;
    maior = a;
    anterior = a;
    ant_maior = a;

    while(atual != NULL)
    {
        if(atual->moda > maior->moda)
        {
            ant_maior = anterior;
            maior = atual;
        }
        anterior = atual;
        atual = atual->prox;
    }

    if(maior != a)
    {
        ant_maior->prox = a;
        tmp = a->prox;
        a->prox = maior->prox;
        maior->prox = tmp;
    }

    maior->prox = ordena_lista(maior->prox);


    return maior;
}

/*Paassa por todos os elementos da árvore para calcular a moda*/
void ordena(Words* a)
{
    if(a!=NULL)
    {
        a->vizinhos = ordena_lista(a->vizinhos);
        ordena(a->esq);
        ordena(a->dir);
    }
}

/*Recursão para liberar toda a memória do final*/
void deltree(Words *tree)
{
    if (tree)
    {
        deltree(tree->esq);
        deltree(tree->dir);
        free(tree);
    }
}
