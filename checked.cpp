/*
extern void read(int& v);

int f(int* t) {
    int a;
    int b;

    read(a);
    read(b);

    int sum = 0;
    for (int i = 0; i < b; i++) {
        if (t != nullptr) {
            sum += *t * a;
        } else {
            return -1;
        }
    }

    return sum - b;
}
*/

extern void print(int x);
void f(int a) {
    for (int i = 0; i < a; i++) 
        if (i % 2) {
            print(i);
        }
    
    print(-1);
}