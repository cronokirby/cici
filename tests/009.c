/*LEX
int main ( ) { return 10 - 1 - 2 - 3 - 4 ; }
*/
/*AST
(int-main (return (top-expr (- (- (- (- 10 1) 2) 3) 4))))
*/
int main() {
    return 10 - 1 - 2 - 3 - 4;
}
