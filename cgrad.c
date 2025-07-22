    #include "cgrad.h"

    int get_in_n(OperationType op_type) {
        switch (op_type)
        {
        case ADD_EXPR:
            return ADD_IN_N;
            break;
        case SUB_EXPR:
            return SUB_IN_N;
            break;
        case MUL_EXPR:
            return MUL_IN_N;
            break;
        case DIV_EXPR:
            return DIV_IN_N;
            break;
        default:
            return DEFAULT_IN_N;
            break;
        }
    }

    void add_back(Node* self);

    Node* init_node() {
        Node* node = malloc(sizeof(Node));
        node->inputs = NULL;
        node->variable_f = false;
        node->operation_f = false;
        node->forward_f = false;
        node->backward_f = false;
        node->backward = NULL;
        node->grad = EMPTY_GRAD_VALUE;
        node->value = EMPTY_NODE_VALUE;
        return node;
    }

    Node* init_variable(double val) {
        Node* node = init_node();
        node->value = val;
        node->variable_f = true;
        node->backward = NULL;
        return node;
    }

    Node* init_empty_operation(OperationType op_type) {
        Node* node = init_node();
        node->value = EMPTY_OPERATION_VALUE;
        Operation* operation = malloc(sizeof(Operation));
        operation->opType = op_type;
        operation->in_n = get_in_n(op_type);
        node->operation_f = true;
        node->inputs = NULL;
        node->operation = operation;
        return node;
    }

    void add_back(Node* self) {
        self->inputs[0]->grad += self->grad;
        self->inputs[1]->grad += self->grad;
    }

    Node* add(Node* l, Node* r) {
        Node* out = init_empty_operation(ADD_EXPR);
        out->value = l->value + r->value;
        out->operation_f = true;
        out->inputs = malloc(sizeof(Node*) * out->operation->in_n);

        out->inputs[0] = l; out->inputs[1] = r;

        out->backward = add_back;
        return out;
    }

    void sin_back(Node* self) {
        self->inputs[0]->grad += cos(self->inputs[0]->value) * self->grad;
    }

    Node* SIN(Node* in) {
        Node* out = init_empty_operation(SIN_EXPR);
        out->value = sin(in->value);
        out->operation_f= true;
        out->inputs = malloc(sizeof(Node*));

        out->inputs[0] = in;

        out->backward = sin_back;
        return out;
    }

    void cos_back(Node* self) {
        Node* input = self->inputs[0];
        input->grad += -sin(input->value) * self->grad;
    }

    Node* COS(Node* in) {
        Node* out = init_empty_operation(COS_EXPR);
        out->value = cos(in->value);
        out->operation_f= true;
        out->inputs = malloc(sizeof(Node*));

        out->inputs[0] = in;

        out->backward = cos_back;
        return out;
    }

    void mul_back(Node* self) {
        double x = self->inputs[0]->value;
        double y = self->inputs[1]->value;
        self->inputs[0]->grad += y * self->grad;
        self->inputs[1]->grad += x * self->grad;
    }

    Node* mul(Node* l, Node* r) {
        Node* out = init_empty_operation(MUL_EXPR);
        out->value = l->value * r->value;
        out->operation_f= true;
        out->inputs = malloc(sizeof(Node*) * out->operation->in_n);

        out->inputs[0] = l; out->inputs[1] = r;

        out->backward = mul_back;
        return out;
    }

    void backprop(Node* root) {
        if(!root->backward || root->backward_f) return;
        root->backward(root); root->backward_f = true;
        int i = 0;
        while(root->inputs[i]) {
            backprop(root->inputs[i]);
            i++;
        }
    }

    int main(void) {

        Node* x1 = init_variable(2);
        Node* x2 = init_variable(3);

        Node* m = mul(x1, x2);
        Node* a = add(m, x1);
        Node* y1 = mul(a, x2);


        print_computation_graph(y1);

        y1->grad = 1.0;
        backprop(y1);

        printf("GRADIENT : [%lf, %lf]\n", x1->grad, x2->grad);


        Node* x3 = init_variable(2);
        Node* x4 = init_variable((double) 0.5236);

        Node* s = SIN(x4);
        Node* m2 = mul(x3, s);
        Node* y2 = add(m2, s);

        print_computation_graph(y2);
        
        y2->grad = 1.0;
        backprop(y2);

        printf("GRADIENT [X3, X4]: [%lf, %lf]\n", x3->grad, x4->grad);

        return 0;
    }

    // f(x1, x2) =          ((x1 * x2) + x1) * ((x1 * x2) + x1)
    // grad(f(x1, x2)) =    [(x2 + 1) * ((x1 * x2) + x1) + ((x1 * x2) + x1) * (x2 + 1) , x1 * ((x1 * x2) + x1) + ((x1 * x2) + x1) * x1 ] 

    // for x1 = 2 and x2 = 3    grad(f(x1, x2)) = [64, 32]

    // ((x1 * x2) + x1) * X2
    // (x2 + 1) * x2,    (x1 * x2 + x1) + x1 * x2  = 12, 14

    // f(x3, x4) = (sin(x4) * x3) + sin(x4)
    // for x3 = 2, X4 = sqrt(2) / 2 

    // GRAD = [  sin(x4)  ,  X3 * cos(X4) + cos(X4)   ] = [  0.5  ,    1.5 ]