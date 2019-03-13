#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void menuUsuario();
void menuAdmin();

//Funções de Usuário
void iniciaProcesso(int id);
void finalizaProcesso();
void alocaMemoria();
void acessaCelula();


//Funções de administrador
void exibeProcesso();
void exibeMemoria();
void exibeTabelaPaginas();
void exibeQuadrosLivres();

//Funções auxiliares
int getQuadroLivre(); // Retorna a posicao de um quadro livre
int getQtdQuadrosLivres(); // Retorna a quatidade de quadros livres
int divide(float n1, float n2); // Divisao teto
int getQtdQuadrosProcesso(int id); // Retorna quantos quadros um processo tem
int tamUltimoQuadro(int id); //Retorna o tamanho ocupado no ultimo quadro
int getUltimoQuadro(int id); //Retorna o ultimo quadro que o processo ocupa

struct bloco{
	int idProcesso;
	int tam; //Fim na memoria
	int *tabela_paginas;
};
typedef struct bloco bloco;

bloco *bcp;

int tamMemoria, tamQuadro, contProcesso=0;
char *memoria;
int *mapa_bits;

int main(){

	int opcao;
	char modo;
	
	//Solicita as informacoes

	printf("Qual o tamanho da memória?\n");
	scanf("%d", &tamMemoria);

	
	printf("Qual o tamanho do quadro?\n");
	scanf("%d", &tamQuadro);

	
	//Aloca espaço dinamicamente
	memoria 	= (char *) malloc(sizeof(char)*tamMemoria);
	mapa_bits 	= (int *) malloc(sizeof(int)*(tamMemoria/tamQuadro));

	//Libera todos os quadros
	for(int i=0; i<(tamMemoria/tamQuadro); i++){
		mapa_bits[i] = 0;
	}
	
	//Lixo de memoria
	for(int i = 0; i<tamMemoria; i++){
		memoria[i] = '0';
	}
	
	do{
        printf("(U)suario ou (A)dministrador?\n");
        setbuf(stdin, NULL);
        scanf("%c", &modo);
        
        if(modo == 'U' || modo == 'u'){
			menuUsuario();
			scanf("%d", &opcao);
			switch(opcao){
				case 1:
					iniciaProcesso(contProcesso++);
					break;
				case 2:
					finalizaProcesso();
					break;
				case 3:
					alocaMemoria();
					break;
				case 4:
					acessaCelula();
					break;
				default:
					printf("Opção inválida");
			}
			
		}else if(modo == 'A' || modo == 'a'){
			menuAdmin();
			scanf("%d", &opcao);
			switch(opcao){
				case 1:
					exibeProcesso();
					break;
				case 2:
					exibeMemoria();
					break;
				case 3:
					exibeTabelaPaginas();
					break;
				case 4:
					exibeQuadrosLivres();
					break;
				default:
					printf("Opção inválida");
			}
		}else{
			break;
		}
	}while(1);
	
	return 0;
}

void menuUsuario(){
	printf("[1] Inicializar processo\n");
	printf("[2] Finalizar processo\n");
	printf("[3] Alocar memória para processo\n");
	printf("[4] Acessar conteudo\n");
	printf("Opção: ");
}

void menuAdmin(){
	printf("[1] Exibir conteudo de um processo\n");
	printf("[2] Exibir conteúdo da memória\n");
	printf("[3] Exibir tabela de páginas de um processo\n");
	printf("[4] Exibir lista de quadros livres\n");
	printf("Opção: ");
}

/* IMPLEMENTAÇÃO FUNÇÕES DE USUARIO */

//Aloca memoria e salva o processo na memoria
void iniciaProcesso(int id){
	int contChar = 0, qtd_quadros, quadro;
	char *string;
	
	//Aloca espaço para o processo
	string = (char*) malloc(sizeof(char)*tamMemoria);
	setbuf(stdin, NULL);
	printf("Conteudo: ");
	scanf("%[^\n]s",string);
	
	//Calcula a quantidade de quadros que o processo vai precisar
	qtd_quadros = divide((float) strlen(string), (float) tamQuadro);
	
	//Verifica se há espaço suficiente para salvar o processo
	if(getQtdQuadrosLivres() >= qtd_quadros){
	
		//Se for a primeira execucao, aloca memoria
		if(id == 0){
			bcp = (bloco *) malloc(sizeof(bloco));
		}else{
			bcp = (bloco *) realloc(bcp,sizeof(bloco) * (contProcesso+1));
		}
		
		bcp[id].tabela_paginas = (int *) malloc(sizeof(int) * qtd_quadros);
		
		for(int i = 0; i<qtd_quadros; i++){
			//Busca o quadro livre
			quadro = getQuadroLivre();
			
			if(quadro == -1){
				printf("Erro ao buscar quadro livre \n");
				break;
			}
			
			//Marca como ocupado
			mapa_bits[quadro] = 1;
			
			//Salva o quadro
			bcp[id].tabela_paginas[i] = quadro; 
			
			//Salvando os caracteres no bloco
			for(int j=0; j < tamQuadro; j++){
				if(contChar < strlen(string)){
					memoria[quadro*tamQuadro + j] = string[contChar];
					contChar++;
				}else{
					break;
				}
			}
		}
		//Salva as informações do processo
		bcp[id].idProcesso = id;
		bcp[id].tam = contChar;
	}
}

void alocaMemoria(){
	int id;
	setbuf(stdin, NULL);
	printf("Id do processo: ");
	scanf("%d",&id);
	
	if(id < 0 || id >= contProcesso){
		printf("Processo inválido\n");
		return;
	}
	
	char *string;
	//int quadro;
	int contChar = 0, ultimo_quadro, espaco_livre, qtd_quadros, quadro;
	
	//Aloca espaço para o processo
	string = (char*) malloc(sizeof(char)*tamMemoria);
	setbuf(stdin, NULL);
	printf("Conteudo: ");
	scanf("%[^\n]s",string);
	
	//Busca o ultimo quadro que o processo ocupa
	ultimo_quadro = getUltimoQuadro(id);
	
	//Busca o espaco livre no ultimo bloco
	espaco_livre = tamQuadro - tamUltimoQuadro(id);
	
	//Aproveita o espaço livre do ultimo quadro
	if(espaco_livre > 0){
		for(int j=0; j < espaco_livre; j++){
			if(contChar < strlen(string)){
				memoria[ultimo_quadro*tamQuadro + tamUltimoQuadro(id) + j] = string[contChar];
				contChar++;
			}else{
				break;
			}
		}
		bcp[id].tam = bcp[id].tam + contChar;
	}
	
	//Calcula a quantidade de quadros que o processo vai precisar
	qtd_quadros = divide((float) strlen(string), (float) tamQuadro);
	
	//Verifica se há espaço suficiente para salvar o processo
	if(getQtdQuadrosLivres() >= qtd_quadros && qtd_quadros > 0){
		
		//Aloca mais memoria para a pagina
		bcp[id].tabela_paginas = (int *) realloc(bcp[id].tabela_paginas, sizeof(int) * qtd_quadros + getQtdQuadrosProcesso(id));
		
		for(int i = getQtdQuadrosProcesso(id); i<qtd_quadros + getQtdQuadrosProcesso(id); i++){
			printf("EXECUTOU \n");
			//Busca o quadro livre
			quadro = getQuadroLivre();
			
			if(quadro == -1){
				printf("Erro ao buscar quadro livre \n");
				break;
			}
			
			//Marca como ocupado
			mapa_bits[quadro] = 1;
			bcp[id].tabela_paginas[i] = quadro; 
			
			//Salvando os caracteres no bloco
			for(int j=0; j < tamQuadro; j++){
				if(contChar < strlen(string)){
					memoria[quadro*tamQuadro + j] = string[contChar];
					contChar++;
				}else{
					break;
				}
			}
		}
		
		bcp[id].tam = bcp[id].tam + contChar;
	}
}

//Finaliza um processo liberando espaço da memoria
void finalizaProcesso(){
	int id, finalizado;
	setbuf(stdin, NULL);
	printf("Id do processo: ");
	scanf("%d",&id);
	
	if(id < 0 || id >= contProcesso){
		printf("Processo inválido\n");
		return;
	}
	
	//Percorre o mapa de bits colocando como livre
	for(int i = 0; i < getQtdQuadrosProcesso(id); i++){
		mapa_bits[bcp[id].tabela_paginas[i]] = 0;
		finalizado = 1;
	}
	if(finalizado){
		printf("Processo %d finalizado com sucesso. \n", id);
	}
}

//Exibe um caracter com base no endereço lógico
void acessaCelula(){
	int id, end_logico, deslocamento;
	setbuf(stdin, NULL);
	printf("Id do processo: ");
	scanf("%d",&id);
	printf("Endereço lógico: ");
	scanf("%d",&end_logico);
	
	if(id < 0 || id >= contProcesso){
		printf("Processo inválido\n");
		return;
	}
	
	/* A logica ultilizada é que se o deslocamento for menor que o tamanho quadro, entao o endereço esta no quadro atual. */
	
	deslocamento = end_logico;
	
	//Valida se o endereço é valido
	if(end_logico < getQtdQuadrosProcesso(id) * tamQuadro){
		//Acessando os quadros
		for(int i=0; i <getQtdQuadrosProcesso(id); i++){
			if(mapa_bits[bcp[id].tabela_paginas[i]] == 1){
				if(deslocamento < tamQuadro){
					if(deslocamento >= tamUltimoQuadro(id)){
						printf("Alerta: Acesso ilegal de memória!\n");
					}else{
						printf("%c \n", memoria[i*tamQuadro + deslocamento]);
						break;
					}
				}
				//Decrementa o deslocamento e passa para o proximo quadro
				deslocamento -= tamQuadro;
			}	
		}
	}else{
		printf("Alerta: Acesso ilegal de memória!\n");
	}
}

/* IMPREMENTAÇÃO DAS FUNÇÕES DO ADMINISTRADOR	*/

//Exibe toda a memoria
void exibeMemoria(){
	for(int i=1; i<=tamMemoria; i++){
		printf("%c", memoria[i-1]);
		if(i % tamQuadro == 0){
			printf("\n");
		}
	}
}

//Exibe o conteudo de um processo
void exibeProcesso(){
	int id, tamanhoProcesso;
	setbuf(stdin, NULL);
	printf("Id do processo: ");
	scanf("%d",&id);
	
	if(id < 0 || id >= contProcesso){
		printf("Processo inválido\n");
		return;
	}
	
	tamanhoProcesso = 0;
	//Procura o quadro
	for(int i = 0; i < getQtdQuadrosProcesso(id); i++){
		if(mapa_bits[bcp[id].tabela_paginas[i]] == 1){
			for(int j=0; j<tamQuadro; j++){
				if(tamanhoProcesso >= bcp[id].tam){
					break;
				}
				printf("%c", memoria[bcp[id].tabela_paginas[i]*tamQuadro + j]);
				tamanhoProcesso++;
			}
		}
	}
	printf("\n");
}

//Exibe a tabela de paginas de um processe e seus respectivo quadro
void exibeTabelaPaginas(){
	int id;
	setbuf(stdin, NULL);
	printf("Id do processo: ");
	scanf("%d",&id);
	
	if(id < 0 || id >= contProcesso){
		printf("Processo inválido\n");
		return;
	}
	
	for(int i = 0; i < getQtdQuadrosProcesso(id); i++){
		//Verifica se o quadro esta sendo usado
		if(mapa_bits[bcp[id].tabela_paginas[i]] == 1){
			printf("Pagina %d Quadro %d \n", i, bcp[id].tabela_paginas[i]);
		}
	}
	printf("\n");
	
}

//Exibe todos os quadros livres
void exibeQuadrosLivres(){
	printf("Lista de quadros livres \n");
	for(int i = 0; i < (tamMemoria/tamQuadro); i++){
		if(mapa_bits[i] == 0){
			printf("Quadro %d\n", i);
		}
	}
	printf("\n");
}


/* IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES*/


// Retorna a posicao de um quadro livre
int getQuadroLivre(){
	for(int i=0; i<(tamMemoria/tamQuadro); i++){
		if(mapa_bits[i] == 0){
			return i;
		}
	}
	return -1;
}

// Retorna a quatidade de quadros livres
int getQtdQuadrosLivres(){
	int cont = 0;
	for(int i=0; i<(tamMemoria/tamQuadro); i++){
		if(mapa_bits[i] == 0){
			cont++;
		}
	}
	return cont;
}

//Retorna o tamanho ocupado no ultimo quadro
int tamUltimoQuadro(int id){
	int qtd_quadros;
	
	qtd_quadros = getQtdQuadrosProcesso(id);
	
	return (tamQuadro-((qtd_quadros*tamQuadro)-bcp[id].tam));
}

//Retorna o ultimo quadro que o processo ocupa
int getUltimoQuadro(int id){
	int quadro = -1;
	for(int i = 0; i < getQtdQuadrosProcesso(id); i++){
		quadro = bcp[id].tabela_paginas[i];
	}
	return quadro;
}

// Retorna quantos quadros um processo tem
int getQtdQuadrosProcesso(int id){
	return divide(bcp[id].tam, tamQuadro);
}

//Divide dois numeros e retorna o teto
int divide(float n1, float n2){
	return ceil(n1/n2);
}
