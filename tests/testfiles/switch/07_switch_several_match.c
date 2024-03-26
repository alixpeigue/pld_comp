int main() {
    int a = 2;
    switch (a) {
        case 0:
            a = 10;
            break;
        case 1:
        case 2:
        case 3:
            a = 20;
            break;
    }

    return a;
}
