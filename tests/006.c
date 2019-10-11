/*LEX
int main ( ) { int x1 = 2 + 2 , x2 = 2 ; return x1 + x2 ; }
*/
/*AST
(top-level
(function main (params) (block
    (declaration (declare x1 (+ 2 2)) (declare x2 2))
    (return (top-expr (+ x1 x2))))))
*/
//RET 6
int main() {
    int x1 = 2 + 2, x2 = 2;
    return x1 + x2;
}
