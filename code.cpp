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
	while (first != second) {
        if (first < second)
        {
            second -= first;
        }
		else
			{
               first -= second;  
            }
           
    }
		
	return first;
}*/

/*разыменование нулевого указателя
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
/*
//разыменование нулевого указателя
int main() {
    int a = 6;
    int *p;
    p = nullptr;
    *p = a;
}*/
//разыменование нулевого указателя+неинициализированный указатель
/*int main() {
    int a = 6;
    int *p;
    *p = a;
}*/

int main() 
{
    long int a, b, c;
    if (a+b <=110000)
        return -1;
    return 0;
}


/*
int main() {
    int a = 6;
    int *p;
    p = &a;

    if (*p < 3)
    {
        return -1;
    }
    return 10;
}*/

//разыменование нулевого указателя
/*
int main() {
    int *p;
    int a = 6;
    if (p != nullptr) {
        return a;
    }
    *p = a;
     return a+6;
    p[5]=8;
}*/
/*
int f(int* p) {
    if (p != nullptr) {
        return -1;
    } else {
        return *p + 10;
    }
}*/


// ===============================================


/*
extern void print(int x);

int f(int a, int b) {
    print(a);
    print(b);

    int sum = 0;
    for (int i = 0; i < a; i++) {
        sum += i;
    }
    sum *= b;

    return sum;
}
*/

/*
extern void print(int x);

void f(int a) {
    int sum = 0;

    for (int i = 0; i < a; i++) {
        if (i % 2 == 0) {
            sum += i;
        }
        print(sum);
    }

    print(sum);
}
*/

/*
extern void print(int a);

void f(int a) {
    int sum = 0;
    for (int i = 0; i < a; i++)
        if (i % 2 == 0)
            sum += i;
        else
            sum -= 2 * i;
    
    print(sum);
}*/
/*
int do_fallocate(int offset, int len)
{
	if (offset < 0 || len <= 0)
		return -EINVAL;
	if ((offset + len > inode->i_sb->s_maxbytes) || (offset + len < 0))
        return -EFBIG;
}*/
