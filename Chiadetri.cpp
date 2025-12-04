#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>


#define MIN_PARALLEL_SIZE 50000
#define MAX_DEPTH 3

// =============================
// Merge Sort Sequential
// =============================

void merge(int arr[], int left, int mid, int right, int temp[]) {
    int i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        temp[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (i = left; i <= right; i++)
        arr[i] = temp[i];
}

void mergeSortSeq(int arr[], int left, int right, int temp[]) {
    if (left >= right) return;

    int mid = (left + right) / 2;

    mergeSortSeq(arr, left, mid, temp);
    mergeSortSeq(arr, mid + 1, right, temp);

    merge(arr, left, mid, right, temp);
}

void mergeSortSequential(int arr[], int n) {
    int* temp = (int*)malloc(sizeof(int) * n);
    mergeSortSeq(arr, 0, n - 1, temp);
    free(temp);
}


// =============================
// Merge Sort Parallel (WinAPI)
// =============================

typedef struct {
    int* arr;
    int* temp;
    int left;
    int right;
    int depth;
} ThreadArgs;

DWORD WINAPI mergeSortThread(LPVOID arg);

DWORD WINAPI mergeSortThread(LPVOID arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int left = args->left;
    int right = args->right;

    if (left >= right) return 0;

    int mid = (left + right) / 2;
    int size = right - left + 1;

    if (args->depth < MAX_DEPTH && size >= MIN_PARALLEL_SIZE) {
        HANDLE t1, t2;

        ThreadArgs leftArgs = { args->arr, args->temp, left, mid, args->depth + 1 };
        ThreadArgs rightArgs = { args->arr, args->temp, mid + 1, right, args->depth + 1 };

        t1 = CreateThread(NULL, 0, mergeSortThread, &leftArgs, 0, NULL);
        t2 = CreateThread(NULL, 0, mergeSortThread, &rightArgs, 0, NULL);

        WaitForSingleObject(t1, INFINITE);
        WaitForSingleObject(t2, INFINITE);

        CloseHandle(t1);
        CloseHandle(t2);
    }
    else {
        mergeSortSeq(args->arr, left, right, args->temp);
        return 0;
    }

    merge(args->arr, left, mid, right, args->temp);
    return 0;
}

void mergeSortParallel(int arr[], int n) {
    int* temp = (int*)malloc(sizeof(int) * n);
    ThreadArgs args = { arr, temp, 0, n - 1, 0 };
    mergeSortThread(&args);
    free(temp);
}


// =============================
// Unique Random Array (Fisher–Yates shuffle)
// =============================

void generateUniqueRandom(int arr[], int n) {
    for (int i = 0; i < n; i++)
        arr[i] = i;

    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
}

void printArray(int arr[], int n, int maxPrint) {
    int limit = (n < maxPrint) ? n : maxPrint;
    for (int i = 0; i < limit; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int readArrayFromFile(const char* filename, int arr[], int* n) {
    FILE* f = NULL;

    // fopen_s nhận 3 tham số: &FILE*, tên file, mode
    errno_t err = fopen_s(&f, filename, "r");
    if (err != 0 || f == NULL) {
        printf("Không mở được file %s\n", filename);
        return 0;
    }

    // Thử đọc số lượng phần tử trước
    if (fscanf_s(f, "%d", n) == 1) {
        for (int i = 0; i < *n; i++) {
            if (fscanf_s(f, "%d", &arr[i]) != 1) {
                printf("Dữ liệu trong file không hợp lệ!\n");
                fclose(f);
                return 0;
            }
        }
    }
    else {
        // Không có số đầu → tự đọc hết file
        rewind(f);
        *n = 0;
        while (fscanf_s(f, "%d", &arr[*n]) == 1) {
            (*n)++;
        }
    }

    fclose(f);
    return 1;  // Thành công
}

void writeArrayToFile(const char* filename, int arr[], int n) {
    FILE* f = NULL;
    errno_t err = fopen_s(&f, filename, "w");
    if (err != 0 || f == NULL) {
        printf("Không mở được file để ghi: %s\n", filename);
        return;
    }

    fprintf(f, "%d\n", n);
    for (int i = 0; i < n; i++) {
        fprintf(f, "%d ", arr[i]);
    }

    fclose(f);
    printf("Đã ghi mảng sau khi sắp xếp vào file %s\n", filename);
}
// =============================
// MAIN MENU
// =============================

int main() {
    srand((unsigned)time(NULL));

    while (1) {

        int chooseInput;
        int* arr = NULL;
        int n = 0;

        // =============================
        // B1: CHON NGUON DU LIEU
        // =============================
        printf("\n===== NHAP DU LIEU =====\n");
        printf("1. Doc du lieu tu file input.txt\n");
        printf("2. Tu nhap tu ban phim\n");
        printf("3. Sinh mang ngau nhien (khong trung)\n");
        printf("0. Thoat\n");
        printf("Chon: ");
        scanf_s("%d", &chooseInput);

        if (chooseInput == 0) {
            printf("Thoat chuong trinh.\n");
            return 0;
        }

        // ---- 1. DOC FILE ----
        if (chooseInput == 1) {
            arr = (int*)malloc(sizeof(int) * 1000000);

            if (!readArrayFromFile("Chuoi/input_mg.txt", arr, &n)) {
                printf("Khong the doc file input.txt\n");
                free(arr);
                continue;
            }

            printf("Da doc %d phan tu tu file.\n", n);
        }

        // ---- 2. TU NHAP ----
        else if (chooseInput == 2) {

            printf("Nhap so luong phan tu: ");
            scanf_s("%d", &n);

            arr = (int*)malloc(sizeof(int) * n);

            printf("Nhap %d phan tu:\n", n);
            for (int i = 0; i < n; i++) {
                scanf_s("%d", &arr[i]);
            }
        }

        // ---- 3. SINH NGAU NHIEN ----
        else if (chooseInput == 3) {

            printf("Nhap so luong phan tu muon sinh (>= 2): ");
            scanf_s("%d", &n);

            arr = (int*)malloc(sizeof(int) * n);

            generateUniqueRandom(arr, n);

            printf("Da sinh mang ngau nhien khong trung voi %d phan tu.\n", n);
        }


        // =============================
        // IN 10 PHAN TU DAU CUA MANG GOC
        // =============================
        printf("\nMang ban dau (10 phan tu dau): ");
        printArray(arr, n, 10);


        // Tao 2 mang de chay 2 che do
        int* arrSeq = (int*)malloc(sizeof(int) * n);
        int* arrPar = (int*)malloc(sizeof(int) * n);

        for (int i = 0; i < n; i++) {
            arrSeq[i] = arr[i];
            arrPar[i] = arr[i];
        }


        // =============================
        // B2: CHON KIEU CHAY MERGE SORT
        // =============================
        int choiceSort;
        printf("\n===== MENU MERGE SORT =====\n");
        printf("1. Chay ca tuan tu va song song\n");
        printf("2. Chi chay tuan tu\n");
        printf("3. Chi chay song song\n");
        printf("Chon: ");
        scanf_s("%d", &choiceSort);

        double timeSeq = 0, timePar = 0;


        // ---- SAP XEP TUAN TU ----
        if (choiceSort == 1 || choiceSort == 2) {
            clock_t start = clock();
            mergeSortSequential(arrSeq, n);
            clock_t end = clock();
            timeSeq = (double)(end - start) / CLOCKS_PER_SEC;

            printf("Thoi gian sap xep tuan tu: %.6f giay\n", timeSeq);
        }

        // ---- SAP XEP SONG SONG ----
        if (choiceSort == 1 || choiceSort == 3) {
            clock_t start = clock();
            mergeSortParallel(arrPar, n);
            clock_t end = clock();
            timePar = (double)(end - start) / CLOCKS_PER_SEC;

            printf("Thoi gian sap xep song song: %.6f giay\n", timePar);
        }

        // ---- SPEEDUP ----
        // ---- SPEEDUP ----
        if (choiceSort == 1) {

            if (timePar == 0 && timeSeq == 0) {
                printf("Khong the tinh speedup: ca hai thoi gian bang 0!\n");
            }
            else if (timePar == 0) {
                printf("Khong the tinh speedup: thoi gian song song = 0 (chia cho 0)!\n");
            }
            else if (timeSeq == 0) {
                printf("Khong the tinh speedup: thoi gian tuan tu = 0!\n");
            }
            else {
                double speedup = timeSeq / timePar;
                printf("Speedup (Seq / Par): %.2f\n", speedup);
            }
        }



        // =============================
        // IN MANG SAU KHI SAP XEP
        // =============================
        printf("\nMang sau khi sap xep (10 phan tu dau): ");

        if (choiceSort == 2)
            printArray(arrSeq, n, 10);
        else
            printArray(arrPar, n, 10);


        // =============================
        // B3: HOI CO MUON XUAT FILE KHONG
        // =============================
        int exportChoice;
        printf("\nBan co muon ghi ket qua ra output.txt khong?\n");
        printf("1. Co\n");
        printf("2. Khong\n");
        printf("Chon: ");
        scanf_s("%d", &exportChoice);

        if (exportChoice == 1) {
            if (choiceSort == 2)
                writeArrayToFile("output.txt", arrSeq, n);
            else
                writeArrayToFile("output.txt", arrPar, n);

            printf("Da ghi file output.txt.\n");
        }

        // GIAI PHONG BO NHO
        free(arr);
        free(arrSeq);
        free(arrPar);

        printf("\n==============================\n");
        printf("Nhan Enter de quay lai menu...");
        getchar(); getchar();
    }

    return 0;
}


