extern void print(int x);
void f(int a) {
    for (int i = 0; i < a; i++) 
        if (i % 2) {
            print(i);
        }
    
    print(-1);
}