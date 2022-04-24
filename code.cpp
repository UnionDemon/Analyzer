/*
void print(int a);

int myFunc() {
int* p;
int a = 10;
*p *= 2;
if (*p > 10) {
    print(*p);
    p = nullptr;
}
else
{
    *p *= 10;
    a -= *p;
}
return *p + 8;
}
*/


void print(int x);
int myFunc() {
    int a = 10;
    int b=5;
    for(a=1; a<=10; a++)
    {
        b++;
    }
    return 0;
}

/*
int nod(int first, int second)
{
	while (first != second)
		if (first < second)
			second -= first;
		else
			first -= second;
	return first;
}
*/

//int a = 10;
//int *pa = &a;