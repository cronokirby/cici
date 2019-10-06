/*LEX
 int main ( ) { ; return 0 ; }
*/
/*AST
(int-main (expr-statement) (return (top-expr 0)))
*/
//RET 0
int main() {
    ;
    return 0;
}