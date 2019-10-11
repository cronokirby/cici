/*LEX
int two ( ) { return 2 ; }
int main ( ) { return two ( ) + two ( ) ; }
*/
/*AST
(top-level
(function two (params) (block (return (top-expr 2))))
(function main (params) (block
    (return (top-expr (+ (call two (params)) (call two (params))))))))
*/
//RET 4
int two() {
    return 2;
}

int main() {
    return two() + two();
}