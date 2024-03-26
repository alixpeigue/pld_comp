int main() {
    int a = 2, b = 0;
    while (a) {
        a = a-1;
        continue;
        b = b + 2;
    }

    return a + b;
}
