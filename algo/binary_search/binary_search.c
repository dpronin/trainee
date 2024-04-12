#include <stdio.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int binary_search(int const a[], int v, int n)
{
    int l = 0, r = n - 1;

    while (l <= r) {
        int const m = (l + r) / 2;
        if (v < a[m])
            r = m - 1;
        else if (v > a[m])
            l = m + 1;
        else
            return m;
    }

    return -1;
}

int main(int argc, char const *argv[])
{
    int const a[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};

    printf("array: ");
    for (int i = 0; i < ARRAY_SIZE(a); ++i)
        printf("%i ", a[i]);
    printf("\n");

    for (int i = 0; i < ARRAY_SIZE(a); ++i) {
        printf("search %d: index %i\n", a[i], binary_search(a, a[i], ARRAY_SIZE(a)));
        printf("search %d: index %i\n", 2 * i, binary_search(a, 2 * i, ARRAY_SIZE(a)));
    }

    return 0;
}
