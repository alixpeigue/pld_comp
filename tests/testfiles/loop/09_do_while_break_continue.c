int main() {
    int a = 3;
    do {
        a = a-1;
        if(a) {
            break;
        } else {
            continue;
        }
    } while (1);
    return a;
}
