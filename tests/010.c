/*LEX
int main ( ) { return 30 / 10 % 2 ; }
*/
/*AST
(int-main (return (top-expr (% (/ 30 10) 2))))
*/
//RET 1
int main() { return 30 / 10 % 2; }
