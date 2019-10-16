/*LEX
int main ( ) { int x = 0 ; while ( x != 10 ) x = x + 1 ; return x ; }
*/
/*AST
(top-level
(function main (params) (block
    (declaration (declare x 0))
    (while (!= x 10) (expr-statement (top-expr (= x (+ x 1)))))
    (return (top-expr x)))))
*/
//RET 10
int main() {
    int x = 0;
    while (x != 10) x = x + 1;
    return x;
}