/*LEX
 int main ( ) { ; return 0 ; }
*/
/*AST
(top-level (function main (params-def) (block (expr-statement) (return (top-expr 0)))))
*/
//RET 0
int main() {
    ;
    return 0;
}