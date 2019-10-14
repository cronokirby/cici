/*LEX
int main ( ) { if ( 10 == 10 ) { if ( 10 == 10 ) return 42 ; } return 0 ; }
*/
/*AST
(top-level
(function main (params) (block
    (if (== 10 10) (block (if (== 10 10) (return (top-expr 42)))))
    (return (top-expr 0)))))
*/
//RET 42
int main() {
    if (10 == 10) {
        if (10 == 10) return 42;
    }
    return 0;
}