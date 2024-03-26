int main() {
    int a = 1;
    switch (a) {
        case 0:
            a = 10;
            break;
        case 1:
            return a;
            a = 20;
    }

    return a;
}
