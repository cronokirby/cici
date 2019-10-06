// A comment
/*/WOAH*/
/* * * * ***/
/*LEX
int main ( ) { return 2 ; }
*/
/*AST
(int-main (return (top-expr 2)))
*/
//RET 2
int main() { return 2; }