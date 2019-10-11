// A comment
/*/WOAH*/
/* * * * ***/
/*LEX
int main ( ) { return 2 ; }
*/
/*AST
(top-level (function main (params) (block (return (top-expr 2)))))
*/
//RET 2
int main() { return 2; }