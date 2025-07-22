#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define EMPTY_VARIABLE_VALUE 0
#define EMPTY_NODE_VALUE 0.0
#define EMPTY_GRAD_VALUE 0.0
#define EMPTY_DERIVATIVE_VALUE 0.0
#define NULL_DERIVATIVE NULL


#define MAX_IN_NODES    10
#define MAX_OUT_NODES   10

#define EMPTY_OPERATION_VALUE 0.0

#define DEFAULT_IN_N MAX_IN_NODES
#define DEFAULT_OUT_N MAX_OUT_NODES
#define ADD_IN_N    2
#define SUB_IN_N    2
#define MUL_IN_N    2
#define DIV_IN_N    2
#define SIN_IN_N    1
#define COS_IN_N    1


// --- Forward declarations ---
typedef struct Node Node;
typedef struct Operation Operation;

typedef enum {
    VAR_EXPR,
    CONST_EXPR,
    ADD_EXPR,
    SUB_EXPR,
    MUL_EXPR,
    DIV_EXPR,
    SIN_EXPR,
    COS_EXPR,
} OperationType;

struct Operation{
    OperationType opType;
    int in_n;
};


struct Node{
    Node** inputs;

    double value;
    double grad;    

    void (*backward)(struct Node* self);  // funzione per calcolo gradiente

    // 00 00 00 01 operation_mask
    // 00 00 01 00 backward_mask
    // 00 00 00 00
    bool variable_f;
    bool operation_f;
    bool forward_f; // both can be optimized with bit operations in a single variable
    bool backward_f;

    union {
        Operation* operation;
    };
};






























void print_expression_type_inline(OperationType type, double value) {
    switch (type) {
        case VAR_EXPR:   printf("VAR[%.2f]", value); break;
        case CONST_EXPR: printf("CONST[%.2f]", value); break;
        case ADD_EXPR:   printf("ADD[%.2f]", value); break;
        case SUB_EXPR:   printf("SUB[%.2f]", value); break;
        case MUL_EXPR:   printf("MUL[%.2f]", value); break;
        case DIV_EXPR:   printf("DIV[%.2f]", value); break;
        case SIN_EXPR:   printf("SIN[%.2f]", value); break;
        case COS_EXPR:   printf("COS[%.2f]", value); break;

        default:         printf("UNK[%.2f]", value); break;
    }
}

void print_computation_tree(Node* node, int indent, bool is_left) {
    if (node == NULL) return;

    for (int i = 0; i < indent; i++)
            printf(" ");
    if (indent != 0)
            printf("└── ");
    else
        printf("ROOT ");
    print_expression_type_inline(node->operation_f ? node->operation->opType : VAR_EXPR, node->value); printf("\n");

    Node** inputs = node->inputs;
    int i = 0;
    while(inputs && inputs[i]) {
        if (inputs[i] != NULL)
            print_computation_tree(inputs[i], indent + 4, false);
        i++;
    }
}

void print_computation_graph(Node* node) {
    printf("Computation Graph (as tree):\n");
    print_computation_tree(node, 0, false);
}