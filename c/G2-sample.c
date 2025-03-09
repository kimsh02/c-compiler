/*
 * A sample G2 program
 */
int f(int x)
{
}

int a()
{
	int x;
	if (x > 0) {
		while (x < 10) {
			x = 0;
			println();
			/* f = 10000; */
			f(x);
		}

	} else {
		x = 1;
	}

	return x;
}
