int main() {
    int a = 2, b = 0;
    do {
        a = a-1;
        continue;
        b = b + 2;
    } while (a);

    return a + b;
}
