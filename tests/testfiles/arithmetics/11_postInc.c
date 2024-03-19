int main () {
    int a = 2;
    int b = a++ * a++;
    b = a-- * ++b + 5 << 3;
    return b;
}