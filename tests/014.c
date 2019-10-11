/*LEX
int add ( int a , int b ) { return a + b ; }
int main ( ) { int x , y ; y = add ( 1 , x = 1 + 1 ) ; return y + x ; }
*/
/*AST
(top-level
(function add (params a b) (block (return (top-expr (+ a b)))))
(function main (params) (block
    (declaration (declare x) (declare y))
    (expr-statement (top-expr (= y (call add (params 1 (= x (+ 1 1)))))))
    (return (top-expr (+ y x))))))
*/
//RET 5
int add(int a, int b) {
    return a + b;
}

int main() {
    int x, y;
    y = add(1, x = 1 + 1);
    return y + x;
}
