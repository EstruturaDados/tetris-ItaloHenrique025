#include <stdio.h>
#include <stdlib.h> // Para rand() e srand()
#include <time.h>   // Para time()

// --- Definições Globais ---
#define MAX_FILA 5  // Capacidade da fila de peças futuras
#define MAX_PILHA 3 // Capacidade da pilha de reserva

/**
 * @brief Define a estrutura de uma peça do jogo.
 * Cada peça tem um 'nome' (tipo) e um 'id' único.
 */
typedef struct {
    char nome; // Tipo da peça (ex: 'I', 'O', 'T', 'L')
    int id;    // Identificador único da peça
} Peca;

/**
 * @brief Define a estrutura da Fila Circular.
 */
typedef struct {
    Peca pecas[MAX_FILA];
    int frente;   // Índice da frente da fila
    int tras;     // Índice do próximo local vago (traseira)
    int contagem; // Número de elementos atualmente na fila
} FilaPecas;

/**
 * @brief Define a estrutura da Pilha Linear.
 */
typedef struct {
    Peca pecas[MAX_PILHA];
    int topo; // Indica o número de elementos (0 = vazia)
} PilhaPecas;

// --- Protótipos das Funções ---

// Funções de Geração e Exibição
Peca gerarPeca();
void exibirEstado(FilaPecas *fila, PilhaPecas *pilha);
void exibirMenu();

// Funções da Fila Circular
void inicializarFila(FilaPecas *fila);
int filaCheia(FilaPecas *fila);
int filaVazia(FilaPecas *fila);
int adicionarFila(FilaPecas *fila, Peca p); // Enqueue
int removerFila(FilaPecas *fila, Peca *pecaRemovida); // Dequeue
void reabastecerFila(FilaPecas *fila); // Helper

// Funções da Pilha Linear
void inicializarPilha(PilhaPecas *pilha);
int pilhaCheia(PilhaPecas *pilha);
int pilhaVazia(PilhaPecas *pilha);
int push(PilhaPecas *pilha, Peca p);
int pop(PilhaPecas *pilha, Peca *pecaRemovida);

// --- Função Principal ---

int main() {
    srand(time(NULL)); // Inicializa o gerador de números aleatórios
    
    FilaPecas fila;
    PilhaPecas pilha;
    int opcao;
    
    // Inicializa as estruturas
    inicializarFila(&fila);
    inicializarPilha(&pilha);
    
    // Requisito: Preenche a fila inicialmente
    printf("Inicializando o jogo e preenchendo a fila...\n");
    reabastecerFila(&fila);
    
    // Loop principal do menu
    do {
        exibirEstado(&fila, &pilha);
        exibirMenu();
        
        scanf("%d", &opcao);
        printf("\n");

        switch (opcao) {
            
            // 1: JOGAR PEÇA
            case 1: {
                Peca pecaJogada;
                if (removerFila(&fila, &pecaJogada)) {
                    printf(">> AÇÃO: Peça [%c %d] foi jogada!\n", 
                           pecaJogada.nome, pecaJogada.id);
                    reabastecerFila(&fila); // Reabastece a fila
                } else {
                    printf(">> AVISO: Fila vazia! Nenhuma peça para jogar.\n");
                }
                break;
            }
            
            // 2: RESERVAR PEÇA (FILA -> PILHA)
            case 2: {
                if (pilhaCheia(&pilha)) {
                    printf(">> AVISO: A pilha de reserva está cheia! Não é possível reservar.\a\n");
                } else {
                    Peca pecaReservada;
                    if (removerFila(&fila, &pecaReservada)) {
                        push(&pilha, pecaReservada);
                        printf(">> AÇÃO: Peça [%c %d] movida para a reserva.\n", 
                               pecaReservada.nome, pecaReservada.id);
                        reabastecerFila(&fila); // Reabastece a fila
                    } else {
                        printf(">> AVISO: Fila vazia! Nenhuma peça para reservar.\n");
                    }
                }
                break;
            }
            
            // 3: USAR PEÇA RESERVADA (PILHA)
            case 3: {
                Peca pecaUsada;
                if (pop(&pilha, &pecaUsada)) {
                    printf(">> AÇÃO: Peça reservada [%c %d] foi usada!\n", 
                           pecaUsada.nome, pecaUsada.id);
                } else {
                    printf(">> AVISO: A pilha de reserva está vazia!\n");
                }
                break;
            }

            // 4: TROCAR PEÇA ATUAL (SWAP FILA/PILHA)
            case 4: {
                if (filaVazia(&fila) || pilhaVazia(&pilha)) {
                    printf(">> AVISO: É preciso ter pelo menos uma peça na fila E na pilha para trocar.\n");
                } else {
                    // Pega os índices da frente da fila e topo da pilha
                    int indiceFrente = fila.frente;
                    int indiceTopo = pilha.topo - 1;
                    
                    // Realiza a troca (swap)
                    Peca tempPeca = fila.pecas[indiceFrente];
                    fila.pecas[indiceFrente] = pilha.pecas[indiceTopo];
                    pilha.pecas[indiceTopo] = tempPeca;
                    
                    printf(">> AÇÃO: Peça da frente da fila [%c %d] trocada com o topo da pilha [%c %d].\n",
                           fila.pecas[indiceFrente].nome, fila.pecas[indiceFrente].id,
                           pilha.pecas[indiceTopo].nome, pilha.pecas[indiceTopo].id);
                    // Não é necessário reabastecer, a contagem da fila é a mesma.
                }
                break;
            }

            // 5: TROCA MÚLTIPLA (SWAP 3 FILA / 3 PILHA)
            case 5: {
                if (fila.contagem < 3 || pilha.topo < 3) {
                    printf(">> AVISO: É preciso ter 3 peças na fila E 3 na pilha para a troca múltipla.\a\n");
                } else {
                    Peca tempPeca;
                    // Itera 3 vezes para trocar os 3 elementos
                    for (int i = 0; i < 3; i++) {
                        // Índice da fila (circular): da frente para frente+2
                        int indiceFila = (fila.frente + i) % MAX_FILA;
                        // Índice da pilha (linear): do topo para topo-2
                        int indicePilha = (pilha.topo - 1) - i; 
                        
                        // Realiza a troca (swap)
                        tempPeca = fila.pecas[indiceFila];
                        fila.pecas[indiceFila] = pilha.pecas[indicePilha];
                        pilha.pecas[indicePilha] = tempPeca;
                    }
                    printf(">> AÇÃO: Troca múltipla realizada entre os 3 primeiros da fila e os 3 da pilha.\n");
                    // Não é necessário reabastecer, a contagem da fila é a mesma.
                }
                break;
            }
            
            // 0: SAIR
            case 0:
                printf("Encerrando Tetris Stack... Até logo!\n");
                break;
            
            default:
                printf(">> ERRO: Opção inválida. Tente novamente.\n");
                break;
        }

    } while (opcao != 0);
    
    return 0;
}

// --- Implementação das Funções ---

/**
 * @brief Gera uma nova peça com um ID único e tipo aleatório.
 * Usa uma variável 'static' para garantir que o ID seja sempre único.
 * @return A Peca recém-criada.
 */
Peca gerarPeca() {
    static int proximoId = 0; // 'static' mantém o valor entre chamadas
    Peca novaPeca;
    char tiposDePeca[] = {'I', 'O', 'T', 'L', 'S', 'Z', 'J'};
    
    novaPeca.nome = tiposDePeca[rand() % 7]; // Seleciona um tipo
    novaPeca.id = proximoId++;             // Atribui ID e incrementa
    
    return novaPeca;
}

/**
 * @brief Exibe o estado atual da Fila (circular) e da Pilha (LIFO).
 */
void exibirEstado(FilaPecas *fila, PilhaPecas *pilha) {
    printf("\n--- Estado Atual ---\n");
    
    // 1. Exibir Fila (Circular)
    printf("Fila de peças: ");
    if (filaVazia(fila)) {
        printf("[Vazia]\n");
    } else {
        for (int i = 0; i < fila->contagem; i++) {
            int indice = (fila->frente + i) % MAX_FILA;
            printf("[%c %d] ", fila->pecas[indice].nome, fila->pecas[indice].id);
        }
        printf("\n");
    }
    
    // 2. Exibir Pilha (LIFO - Topo primeiro)
    printf("Pilha de reserva (Topo -> Base): ");
    if (pilhaVazia(pilha)) {
        printf("[Vazia]\n");
    } else {
        for (int i = pilha->topo - 1; i >= 0; i--) {
            printf("[%c %d] ", pilha->pecas[i].nome, pilha->pecas[i].id);
        }
        printf("\n");
    }
    printf("--------------------\n");
}

/**
 * @brief Exibe o menu de opções para o jogador.
 */
void exibirMenu() {
    printf("\nOpções disponíveis:\n");
    printf("Código | Ação\n");
    printf("----------------------------------------------------------\n");
    printf("  1    | Jogar peça da frente da fila\n");
    printf("  2    | Enviar peça da fila para a pilha de reserva\n");
    printf("  3    | Usar peça da pilha de reserva\n");
    printf("  4    | Trocar peça da frente da fila com o topo da pilha\n");
    printf("  5    | Trocar os 3 primeiros da fila com as 3 peças da pilha\n");
    printf("  0    | Sair\n");
    printf("----------------------------------------------------------\n");
    printf("Escolha sua ação: ");
}

// --- Funções da Fila Circular ---

void inicializarFila(FilaPecas *fila) {
    fila->frente = 0;
    fila->tras = 0;
    fila->contagem = 0;
}

int filaCheia(FilaPecas *fila) { return (fila->contagem == MAX_FILA); }
int filaVazia(FilaPecas *fila) { return (fila->contagem == 0); }

/**
 * @brief Adiciona uma peça ao final da fila (enqueue).
 */
int adicionarFila(FilaPecas *fila, Peca p) {
    if (filaCheia(fila)) return 0; // Falha
    fila->pecas[fila->tras] = p;
    fila->tras = (fila->tras + 1) % MAX_FILA; // Avanço circular
    fila->contagem++;
    return 1; // Sucesso
}

/**
 * @brief Remove uma peça da frente da fila (dequeue).
 */
int removerFila(FilaPecas *fila, Peca *pecaRemovida) {
    if (filaVazia(fila)) return 0; // Falha
    *pecaRemovida = fila->pecas[fila->frente];
    fila->frente = (fila->frente + 1) % MAX_FILA; // Avanço circular
    fila->contagem--;
    return 1; // Sucesso
}

/**
 * @brief Helper para encher a fila até o máximo.
 */
void reabastecerFila(FilaPecas *fila) {
    while (!filaCheia(fila)) {
        adicionarFila(fila, gerarPeca());
    }
}

// --- Funções da Pilha Linear ---

void inicializarPilha(PilhaPecas *pilha) {
    pilha->topo = 0; // 'topo' age como contador
}

int pilhaCheia(PilhaPecas *pilha) { return (pilha->topo == MAX_PILHA); }
int pilhaVazia(PilhaPecas *pilha) { return (pilha->topo == 0); }

/**
 * @brief Adiciona uma peça ao topo da pilha (push).
 */
int push(PilhaPecas *pilha, Peca p) {
    if (pilhaCheia(pilha)) return 0; // Falha
    pilha->pecas[pilha->topo] = p; // Adiciona no topo
    pilha->topo++;                   // Incrementa o topo
    return 1; // Sucesso
}

/**
 * @brief Remove uma peça do topo da pilha (pop).
 */
int pop(PilhaPecas *pilha, Peca *pecaRemovida) {
    if (pilhaVazia(pilha)) return 0; // Falha
    pilha->topo--;                     // Decrementa o topo
    *pecaRemovida = pilha->pecas[pilha->topo]; // Pega o item
    return 1; // Sucesso
}