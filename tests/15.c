/*LEX
int main ( ) { return ( 2 != 2 ) + ( 2 == 2 ) ; }
*/
/*AST
(top-level
(function main (params) (block (return (top-expr (+ (!= 2 2) (== 2 2)))))))
*/
//RET 1
int main() {
    return (2 != 2) + (2 == 2);
}
