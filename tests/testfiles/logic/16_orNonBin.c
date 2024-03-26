int main() {
    int a = 0;
    int b = 0;
    int c = 1;
    int d = 2;
    int e = (a || b)*8 + (a || c) * 4 + (c || b) * 2 + (c || d);
    /*       0 ou 0       0 ou 1        1 ou 0          1 ou 1*/
    return e;
}