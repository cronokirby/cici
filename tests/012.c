/*LEX
int two ( ) { return 2 ; }
int main ( ) { return two ( ) ; }
*/
/*AST
(top-level
(function two (block (return (top-expr 2))))
(function main (block (return (top-expr (call two))))))
*/
//RET 2
int two() {
    return 2;
}

int main() {
    return two();
}