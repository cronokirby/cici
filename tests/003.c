/*LEX
int main ( ) { return ! ~ - 2 ; }
*/
/*AST
(int-main (block (return (top-expr (! (~ (- 2)))))))
*/
//RET 0
int main() { return !~-2; }