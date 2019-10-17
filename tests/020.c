/*LEX
int main ( ) {
    int x1 = 0 , x2 = 0 , x3 = 0 ;
    while ( 1 ) {
        if ( x1 == 10 ) break ;
        x1 = x1 + 1 ;
    }
    while ( x3 != 10 ) {
        x3 = x3 + 1 ;
        continue ;
        x2 = x2 + 1 ;
    }
    return x1 + x2 + x3 ;
}
*/
/*AST
(top-level
(function main (params) (block
    (declaration (declare x1 0) (declare x2 0) (declare x3 0))
    (while 1 (block
        (if (== x1 10) (break))
        (expr-statement (top-expr (= x1 (+ x1 1))))))
    (while (!= x3 10) (block
        (expr-statement (top-expr (= x3 (+ x3 1))))
        (continue)
        (expr-statement (top-expr (= x2 (+ x2 1))))))
    (return (top-expr (+ (+ x1 x2) x3))))))
*/
//RET 20
int main() {
    int x1 = 0, x2 = 0, x3 = 0;
    while (1) {
        if (x1 == 10) break;
        x1 = x1 + 1;
    }
    while (x3 != 10) {
        x3 = x3 + 1;
        continue;
        x2 = x2 + 1;
    }
    return x1 + x2 + x3;
}
