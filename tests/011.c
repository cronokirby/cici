/*LEX
int before ( ) { return 0 ; }
int main ( ) { return 0 ; }
int after ( ) { return 0 ; }
*/
/*AST
(top-level
(function before (params-def) (block (return (top-expr 0))))
(function main (params-def) (block (return (top-expr 0))))
(function after (params-def) (block (return (top-expr 0)))))
*/
//RET 0
int before() {
    return 0;
}

int main() {
    return 0;
}

int after() {
    return 0;
}
