/*LEX
int main ( ) { return 1 + - 1 + 3 * ( 4 + 2 ) ; }
*/
/*AST
(int-main (return (top-expr (+ (+ 1 (- 1)) (* 3 (+ 4 2))))))
*/
int main() { return 1 + -1 + 3 * (4 + 2); }
