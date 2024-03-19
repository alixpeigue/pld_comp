int main() {
    int a = 5, b = -4;
    while(a) {
        a = a - 1;
        b = b + a;
        if (b) return a;
    }

    return a;
}
