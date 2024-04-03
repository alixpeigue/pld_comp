int factorial(int n) {
  if (n <= 1) return 1;
  else return n * factorial(n - 1);
}

int main() {
  int n = 4, result = factorial(n);

  while(result > 2 && result < 120) {
    result = factorial(++n);
  }

  switch (result) {
    case 2:
    case 120:
      result = n;
      break;
    default:
      result = n % 2;
  }

  return result;
}
