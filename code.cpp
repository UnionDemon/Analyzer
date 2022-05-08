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
int myFunc() {
    int p;
if (p > 10) {
    p++;
} 
return p + 8;
}
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
int nod(int first, int second)
{
	while (first != second)
		if (first < second)
			second -= first;
		else
			first -= second;
	return first;
}*/

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