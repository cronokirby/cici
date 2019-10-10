/*LEX
int main ( ) {
    int x , y , z ;
    z = x = 2 , y = 3 ;
    1 , 2 , 3 ;
    return x + y + z ;
}
*/
/*AST
(top-level
(function main (params-def) (block
    (declaration (declare x) (declare y) (declare z))
    (expr-statement (top-expr (= z (= x 2)) (= y 3)))
    (expr-statement (top-expr 1 2 3))
    (return (top-expr (+ (+ x y) z))))))
*/
//RET 7
int main() {
    int x, y, z;
    z = x = 2, y = 3;
    1, 2, 3;
    return x + y + z;
}
