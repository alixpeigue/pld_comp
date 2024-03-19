int main () {
    int a = 9876;
    int b = 1234;
    int c = b &= a;
    int d = c ^= b;
    int e = d |= c;
    return e;
}