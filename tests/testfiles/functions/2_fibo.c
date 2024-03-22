int fibo(int a) {
    if (!a) {
        return 0;
    } else if (!(a - 1)) {
        return 1;
    }
    return fibo(a-1) + fibo(a -2);
}

int main() {
return fibo(5);
}
