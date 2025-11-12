#include <stdlib.h>
#include <stdio.h>

static int shitSort(int* a, int n);

/* swap a[i] and a[j] */
#include <stdlib.h>
#include <stdio.h>

/* swap a[i] and a[j] */
static int swap(int* a, int i, int j) {
    int t = a[i]; a[i] = a[j]; a[j] = t;
    return 0;
}

/* copy src[0..n-1] -> dst[0..n-1] */
static int copy_array(int* dst, const int* src, int n) {
    for (int i = 0; i < n; ++i) dst[i] = src[i];
    return n;
}

static int is_sorted(const int* a, int n) {
    for (int i = 1; i < n; ++i) {
        if (a[i-1] > a[i]) return 0;
    }
    return 1;
}

/* “Hyper” sortedness check for a candidate permutation:
   For k = 1..n-1, sort prefix k and prefix (k+1) recursively using hyper_quantum_sort,
   then compare their last elements. If any violation, return 0.
   This only calls hyper_quantum_sort on sizes strictly < n, so it terminates. */
static int hyper_is_sorted(const int* a, int n) {
    if (n <= 1) return 1;

    for (int k = 1; k < n; ++k) {
        /* sort prefix [0..k-1] (length k) */
        int* left = (int*) malloc(sizeof(int) * k);
        int* right = (int*) malloc(sizeof(int) * (k+1));
        if (!left || !right) { free(left); free(right); return 0; }

        copy_array(left, a, k);
        copy_array(right, a, k+1);

        /* Recursively hyper-sort both prefixes */
        shitSort(left, k);
        shitSort(right, k+1);

        /* Compare last elements of the two sorted prefixes */
        int ok = !(left[k-1] > right[k]);
        free(left);
        free(right);
        if (!ok) return 0;
    }

    /* As a final sanity check, verify the whole permutation is nondecreasing. */
    return is_sorted(a, n);
}

/* DFS permutation generator that stops early when a hyper-sorted permutation is found.
   - arr: current working array (mutated in-place by swaps)
   - n: length
   - pos: current fixed-prefix position
   - out: where to copy the winning permutation when found
   - found: int* flag set to 1 when a winner is found
*/
static int dfs_permute_hyper(int* arr, int n, int pos, int* out, int* found) {
    //if (*found) return 1; /* early-out for debug purposes*/
    if (pos == n) {
        if (hyper_is_sorted(arr, n)) {
            copy_array(out, arr, n);
            *found = 1;
            return 1;
        }
        return 0;
    }
    for (int i = pos; i < n; ++i) {
        swap(arr, pos, i);
        dfs_permute_hyper(arr, n, pos + 1, out, found);
        swap(arr, pos, i); /* backtrack */
        if (*found) return 1;
    }
    return 0;
}

/* Public entry: sorts a[0..n-1] in-place to the first hyper-sorted permutation it can find. */
static int shitSort(int* a, int n) {
    if (n <= 1) return n;
    /* work buffers */
    int* work = (int*) malloc(sizeof(int) * n);
    int* best = (int*) malloc(sizeof(int) * n);
    if (!work || !best) { free(work); free(best); return 0; }

    copy_array(work, a, n);
    int found = 0;
    dfs_permute_hyper(work, n, 0, best, &found);

    if (found) copy_array(a, best, n);
    free(work);
    free(best);
    return found ? n : 0;
}


/* Inner DFS: enumerate all permutations of 'cur' and accept the first sorted one. */
static int dfs_inner_q(int* cur, int n, int pos, int* out, int* found) {
    if (*found) return 1;
    if (pos == n) {
        if (is_sorted(cur, n)) {
            copy_array(out, cur, n);
            *found = 1;
            return 1;
        }
        return 0;
    }
    for (int i = pos; i < n; ++i) {
        swap(cur, pos, i);
        dfs_inner_q(cur, n, pos + 1, out, found);
        swap(cur, pos, i);
        if (*found) return 1;
    }
    return 0;
}

/* Outer DFS: enumerate all permutations P of input, and for each P,
   run inner DFS over permutations Q of P. */
static int dfs_outer_p(int* cur, int n, int pos, int* out, int* found) {
    if (*found) return 1;
    if (pos == n) {
        int* tmp = (int*) malloc(sizeof(int) * n);
        if (!tmp) return 0;
        copy_array(tmp, cur, n);
        dfs_inner_q(tmp, n, 0, out, found);
        free(tmp);
        return *found ? 1 : 0;
    }
    for (int i = pos; i < n; ++i) {
        swap(cur, pos, i);
        dfs_outer_p(cur, n, pos + 1, out, found);
        swap(cur, pos, i);
        if (*found) return 1;
    }
    return 0;
}

/* Public entry: sorts a[0..n-1] to the first simple-sorted Q reachable via two-layer permutations. */
static int meta_permutation_sort(int* a, int n) {
    if (n <= 1) return n;
    int* work = (int*) malloc(sizeof(int) * n);
    int* best = (int*) malloc(sizeof(int) * n);
    if (!work || !best) { free(work); free(best); return 0; }
    copy_array(work, a, n);
    int found = 0;
    dfs_outer_p(work, n, 0, best, &found);
    if (found) copy_array(a, best, n);
    free(work);
    free(best);
    return found ? n : 0;
}


int main() {

    printf("Enter a sequence of maximum 20 numbers separated by a space:\n");

    int* arr = calloc(20, sizeof(int));

    int i;
    for (i = 0; i < 20; i++) {
        int N;
        scanf("%d", &N);
        arr[i] = N;
    }

    int N = i * sizeof(int);
    arr = realloc(arr, N);  // just because its easier

    shitSort(arr, N);

    for (int i = 0; i < N; i++) {
        printf("%d ", arr[i]);
    }

    free(arr);

    return 0;
}
