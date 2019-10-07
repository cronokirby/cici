/*LEX
int main ( ) { return 10 - 1 - 2 - 3 - 4 ; }
*/
/*AST
(top-level (function main (block (return (top-expr (- (- (- (- 10 1) 2) 3) 4))))))
*/
//RET 0
int main() {
    return 10 - 1 - 2 - 3 - 4;
}
