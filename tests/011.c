/*LEX
int before ( ) { return 0 ; }
int main ( ) { return 0 ; }
int after ( ) { return 0 ; }
*/
/*AST
(top-level
(function before (block (return (top-expr 0))))
(function main (block (return (top-expr 0))))
(function after (block (return (top-expr 0)))))
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
