/*LEX
int main ( ) { return 1 + - 1 + 3 * ( 4 + 2 ) ; }
*/
/*AST
(top-level (function main (params) (block (return (top-expr (+ (+ 1 (- 1)) (* 3 (+ 4 2))))))))
*/
//RET 18
int main() { return 1 + -1 + 3 * (4 + 2); }
