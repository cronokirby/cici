/*LEX
int main ( ) { int x = 1 ; { int x = 2 ; } return x ; }
*/
/*AST
(top-level
(function main (params) (block
    (declaration (declare x 1))
    (block (declaration (declare x 2)))
    (return (top-expr x)))))
*/
//RET 1
int main() {
    int x = 1;
    {
        int x = 2;
    }
    return x;
}
