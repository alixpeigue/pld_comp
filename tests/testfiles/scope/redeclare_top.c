int main() {
    int a = 1;
    {
        int a;
        a = 2;
    }
    return a;
}
