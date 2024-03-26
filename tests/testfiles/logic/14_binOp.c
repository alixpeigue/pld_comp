int main () {
    int a = 1345;
    int b = 8765;
    int c = a & b;
    int d = c | (a+b);
    int e = d ^ 1111 - 32 * 3;
    return e;
}