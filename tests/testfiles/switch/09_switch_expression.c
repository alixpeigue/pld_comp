int main() {
	int a = 2;
	switch(a*3) {
		case 8:
			return 8;
		case 2:
			return 2;
		case 7:
			return 7;
		case 6:
			return 20;
		default:
			return a+1;
	}
}
