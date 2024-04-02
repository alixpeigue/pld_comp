int factorial(int n) {
  if (n <= 1) return 1;
  else return n * factorial(n - 1);
}

int main() {
  int n = 1, result;

  do {
    result = factorial(n);
    n++;
  } while (result > 1 && result < 120);

  switch (result) {
    case 1:
    case 120:
      n = 0;
      break;
    default:
      n--;
  }

  return n;
}