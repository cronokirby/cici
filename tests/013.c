/*LEX
int main ( ) { return ( 10 ^ 10 ) & 10 | 10 ; }
*/
/*AST
(top-level
(function main (params) (block (return (top-expr (| (& (^ 10 10) 10) 10))))))
*/
//RET 10
int main() {
    return (10 ^ 10) & 10 | 10;
}
