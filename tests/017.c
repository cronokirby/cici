/*LEX
int main ( ) { int x = 2 ; if ( 10 == 2 ) { x = 1 ; } else if ( 10 == 10 ) { x = 0 ; } return x ; }
*/
/*AST
(top-level
(function main (params) (block
    (declaration (declare x 2))
    (if (== 10 2)
        (block (expr-statement (top-expr (= x 1))))
        (if (== 10 10) (block (expr-statement (top-expr (= x 0))))))
    (return (top-expr x)))))
*/
//RET 0
int main() {
    int x = 2;
    if (10 == 2) {
        x = 1;
    } else if (10 == 10) {
        x = 0;
    }
    return x;
}