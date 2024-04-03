int main () {
    int a = 1;
    int b = 2, c = 3;
    if (a == 1 && b == 3){
        c += 1000;
    } else {
        if (a == 1 && b == 2){
            if (c == 3 && 1 == 1){
                c += 1000;
            }
        }
    }
    if (a > b || a == 1){
        c += 1000;
    }
    return c;
}