/*
void print(int a);

int myFunc() {
int* p;
int a = 10;
*p *= 2;
if (*p > 10) {
    print(*p);
    p = nullptr;
} else {
    *p *= 10;
    a -= *p;
}
return *p + 8;
}*/
/*
int myFunc() {
    int p, b, a;
    a=7;
    for (p=1; p < b; p =  p + b) 
        if (a<10)
        b++;
    
        
return b + 8;
}*/

/*
void print(int x);
int myFunc() {
    int a = 10;
    int b=5;
    for(a=1; a<=10; a++)
        b++;
    return 0;
}*/
/*
int test(int a, int b) {
if (b != 0)
a = a + 10 * a + a * 5;


return a;
}*/

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
/*
int main ()
{
  int firstvalue = 5, secondvalue = 15;
  int * p1, * p2;
  
  p1 = &firstvalue;
  p2 = &secondvalue;
  *p1 = 10;
  *p2 = *p1;
  p1 = p2;           
  *p1 = 20;        
  

  return 0;
}*/

int main() {
    int *p = new int[100];
    int a = 6;
    if (p == nullptr)
        return a+6;
    p[5]=8;
}