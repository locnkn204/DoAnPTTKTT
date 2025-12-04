#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif


// Cấu trúc lưu thông tin vật phẩm
typedef struct
{
    int id;        // Mã vật phẩm
    double weight; // Trọng lượng
    double value;  // Giá trị
    double ratio;  // Tỷ lệ giá trị/trọng lượng
} Item;

// Hàm so sánh để sắp xếp theo tỷ lệ giảm dần
int compareItems(const void *a, const void *b)
{
    Item *item1 = (Item *)a;
    Item *item2 = (Item *)b;

    // So sánh tỷ lệ value/weight (giảm dần)
    if (item2->ratio > item1->ratio)
        return 1;
    if (item2->ratio < item1->ratio)
        return -1;
    return 0;
}

// Hàm trả về giá trị lớn hơn
int maxInt(int a, int b)
{
    return (a > b) ? a : b;
}

// Thuật toán tham lam cho bài toán balo phân số
double fractionalKnapsack(Item items[], int n, double capacity)
{
    // Sắp xếp các vật phẩm theo tỷ lệ giá trị/trọng lượng giảm dần
    qsort(items, n, sizeof(Item), compareItems);

    double totalValue = 0.0;    // Tổng giá trị đạt được
    double currentWeight = 0.0; // Trọng lượng hiện tại trong balo

    printf("\n+========================================================+\n");
    printf("|          QUA TRINH CHON VAT PHAM (THAM LAM)           |\n");
    printf("+========================================================+\n");
    printf("| Vat pham | Trong luong | Gia tri | Ty le  | Lay (%%)   |\n");
    printf("+----------+-------------+---------+--------+-----------+\n");

    for (int i = 0; i < n; i++)
    {
        if (currentWeight + items[i].weight <= capacity)
        {
            // Lấy toàn bộ vật phẩm
            currentWeight += items[i].weight;
            totalValue += items[i].value;

            printf("|   %-7d|    %-9.2lf|  %-7.2lf| %-6.2f | 100.00%%   |\n",
                   items[i].id, items[i].weight, items[i].value, items[i].ratio);
        }
        else
        {
            // Chỉ lấy một phần vật phẩm (balo phân số)
            double remainingCapacity = capacity - currentWeight;
            double fraction = remainingCapacity / items[i].weight;

            totalValue += items[i].value * fraction;
            currentWeight += remainingCapacity;

            printf("|   %-7d|    %-9.2lf|  %-7.2lf| %-6.2f |  %-7.2f%% |\n",
                   items[i].id, items[i].weight, items[i].value,
                   items[i].ratio, fraction * 100);

            break; // Balo đã đầy
        }
    }

    printf("+========================================================+\n");

    return totalValue;
}

// Thuật toán tham lam cho bài toán balo 0-1 (không tối ưu)
double zeroOneKnapsackGreedy(Item items[], int n, double capacity, int selected[])
{
    // Sắp xếp các vật phẩm theo tỷ lệ giá trị/trọng lượng giảm dần
    qsort(items, n, sizeof(Item), compareItems);

    double totalValue = 0.0;
    double currentWeight = 0.0;

    // Khởi tạo mảng đánh dấu
    for (int i = 0; i < n; i++)
    {
        selected[i] = 0;
    }

    printf("\n+========================================================+\n");
    printf("|       QUA TRINH CHON VAT PHAM (BALO 0-1 THAM LAM)     |\n");
    printf("+========================================================+\n");
    printf("| Vat pham | Trong luong | Gia tri | Ty le  | Chon?     |\n");
    printf("+----------+-------------+---------+--------+-----------+\n");

    for (int i = 0; i < n; i++)
    {
        if (currentWeight + items[i].weight <= capacity)
        {
            // Lấy toàn bộ vật phẩm
            currentWeight += items[i].weight;
            totalValue += items[i].value;
            selected[i] = 1;

            printf("|   %-7d|    %-9.2lf|  %-7.2lf| %-6.2f |   CO      |\n",
                   items[i].id, items[i].weight, items[i].value, items[i].ratio);
        }
        else
        {
            printf("|   %-7d|    %-9.2lf|  %-7.2lf| %-6.2f |  KHONG    |\n",
                   items[i].id, items[i].weight, items[i].value, items[i].ratio);
        }
    }

    printf("+========================================================+\n");

    return totalValue;
}

// Thuật toán Quy hoạch động cho bài toán balo 0-1
int zeroOneKnapsackDP(Item items[], int n, int capacity, int selected[])
{
    // Tạo bảng DP
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++)
    {
        dp[i] = (int *)calloc(capacity + 1, sizeof(int));
    }

    // Điền bảng DP
    for (int i = 1; i <= n; i++)
    {
        for (int w = 0; w <= capacity; w++)
        {
            int itemWeight = (int)items[i - 1].weight;
            int itemValue = (int)items[i - 1].value;

            if (itemWeight <= w)
            {
                dp[i][w] = maxInt(dp[i - 1][w], dp[i - 1][w - itemWeight] + itemValue);
            }
            else
            {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    int maxValue = dp[n][capacity];

    // Truy vết để tìm các vật phẩm được chọn
    for (int i = 0; i < n; i++)
    {
        selected[i] = 0;
    }

    int w = capacity;
    for (int i = n; i > 0 && maxValue > 0; i--)
    {
        if (maxValue != dp[i - 1][w])
        {
            selected[i - 1] = 1;
            maxValue -= (int)items[i - 1].value;
            w -= (int)items[i - 1].weight;
        }
    }

    // Hiển thị bảng DP (chỉ hiển thị một phần nếu quá lớn)
    printf("\n+============================================================+\n");
    printf("|           BANG QUY HOACH DONG (Mot phan)                   |\n");
    printf("+============================================================+\n");

    int maxDisplay = (capacity > 15) ? 15 : capacity;
    printf("| i\\w  |");
    for (int w = 0; w <= maxDisplay; w++)
    {
        printf(" %3d", w);
    }
    if (capacity > 15)
        printf(" ...");
    printf("\n+------+");
    for (int w = 0; w <= maxDisplay; w++)
    {
        printf("----");
    }
    if (capacity > 15)
        printf("----");
    printf("\n");

    for (int i = 0; i <= n && i <= 10; i++)
    {
        printf("|  %-3d |", i);
        for (int w = 0; w <= maxDisplay; w++)
        {
            printf(" %3d", dp[i][w]);
        }
        if (capacity > 15)
            printf(" ...");
        printf("\n");
    }
    if (n > 10)
        printf("|  ... | ...\n");
    printf("+============================================================+\n");

    printf("\n+========================================================+\n");
    printf("|       CAC VAT PHAM DUOC CHON (QUY HOACH DONG)         |\n");
    printf("+========================================================+\n");
    printf("| Vat pham | Trong luong | Gia tri | Ty le  | Chon?     |\n");
    printf("+----------+-------------+---------+--------+-----------+\n");

    for (int i = 0; i < n; i++)
    {
        printf("|   %-7d|    %-9.2lf|  %-7.2lf| %-6.2f |  %-7s    |\n",
               items[i].id, items[i].weight, items[i].value,
               items[i].ratio, selected[i] ? "CO" : "KHONG");
    }
    printf("+========================================================+\n");

    int result = dp[n][capacity];

    // Giải phóng bộ nhớ
    for (int i = 0; i <= n; i++)
    {
        free(dp[i]);
    }
    free(dp);

    return result;
}

int main()
{
    // Thiết lập UTF-8 cho console Windows để hiển thị tiếng Việt và ký tự khung
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "");

    int n;
    double capacity;
    Item *items = NULL;
    int inputChoice;

    printf("+========================================================+\n");
    printf("|        BAI TOAN BALO - THUAT TOAN THAM LAM             |\n");
    printf("+========================================================+\n\n");

    // -----------------------------------------------------------
    // PHẦN 1: TÙY CHỌN NHẬP DỮ LIỆU (FILE HOẶC NHẬP TAY)
    // -----------------------------------------------------------
    printf("Chon phuong thuc nhap du lieu:\n");
    printf("  1. Nhap tu file 'input.txt'\n");
    printf("  2. Nhap tu ban phim (thu cong)\n");
    printf("Lua chon cua ban (1/2): ");
    scanf("%d", &inputChoice);

    if (inputChoice == 1)
    {
        FILE *fin = fopen("Chuoi/input_qhd.txt", "r");
        if (fin == NULL)
        {
            printf("\n[Loi] Khong tim thay file 'input.txt'! Chuyen sang nhap thu cong.\n\n");
            inputChoice = 2; // Fallback to manual
        }
        else
        {
            fscanf(fin, "%d", &n);
            fscanf(fin, "%lf", &capacity);
            items = (Item *)malloc(n * sizeof(Item));

            printf("\n>> Dang doc du lieu tu file...\n");
            for (int i = 0; i < n; i++)
            {
                items[i].id = i + 1;
                fscanf(fin, "%lf %lf", &items[i].weight, &items[i].value);
                // Tính tỷ lệ ngay khi đọc file để khớp logic
                items[i].ratio = items[i].value / items[i].weight;
            }
            fclose(fin);
            printf(">> Da doc thanh cong %d vat pham. Suc chua balo: %.2f\n", n, capacity);
        }
    }

    // Nếu chọn nhập tay hoặc đọc file thất bại
    if (inputChoice != 1)
    {
        printf("Nhap so luong vat pham: ");
        scanf("%d", &n);

        printf("Nhap trong luong toi da cua balo: ");
        scanf("%lf", &capacity);

        items = (Item *)malloc(n * sizeof(Item));
        printf("\n");
        for (int i = 0; i < n; i++)
        {
            items[i].id = i + 1;
            printf("Vat pham %d:\n", i + 1);
            printf("  Trong luong: ");
            scanf("%lf", &items[i].weight);
            printf("  Gia tri: ");
            scanf("%lf", &items[i].value);

            // Tinh ty le gia tri/trong luong
            items[i].ratio = items[i].value / items[i].weight;
            printf("\n");
        }
    }

    Item *itemsCopy = (Item *)malloc(n * sizeof(Item));
    int *selectedGreedy = (int *)malloc(n * sizeof(int));
    int *selectedDP = (int *)malloc(n * sizeof(int));

    // Sao chép dữ liệu gốc
    memcpy(itemsCopy, items, n * sizeof(Item));

    // Hiển thị thông tin ban đầu
    printf("\n+======================================================+\n");
    printf("|              THONG TIN CAC VAT PHAM BAN DAU          |\n");
    printf("+======================================================+\n");
    printf("| Vat pham | Trong luong | Gia tri | Ty le (v/w)       |\n");
    printf("+----------+-------------+---------+-------------------+\n");

    for (int i = 0; i < n; i++)
    {
        printf("|   %-7d|    %-9.2lf|  %-7.2lf|      %-13.2f|\n",
               items[i].id, items[i].weight, items[i].value, items[i].ratio);
    }
    printf("+======================================================+\n");

    // Giải bài toán balo phân số
    printf("\n=======================================================\n");
    printf("         GIAI PHAP: BAI TOAN BALO PHAN SO\n");
    printf("=======================================================\n");

    clock_t start = clock();
    double fractionalResult = fractionalKnapsack(items, n, capacity);
    clock_t end = clock();
    double timeFractional = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("\n+-------------------------------------------------------+\n");
    printf("| TONG GIA TRI TOI DA (Balo phan so): %.2f            |\n", fractionalResult);
    printf("| Thoi gian thuc thi: %.4f ms                        |\n", timeFractional);
    printf("+-------------------------------------------------------+\n");

    // Khôi phục dữ liệu gốc cho tham lam 0-1
    memcpy(items, itemsCopy, n * sizeof(Item));

    // Giải bài toán balo 0-1 bằng tham lam (không tối ưu)
    printf("\n=======================================================\n");
    printf("       GIAI PHAP: BAI TOAN BALO 0-1 (Tham lam)\n");
    printf("       *** LUU Y: KET QUA CO THE KHONG TOI UU!\n");
    printf("=======================================================\n");

    start = clock();
    double zeroOneResultGreedy = zeroOneKnapsackGreedy(items, n, capacity, selectedGreedy);
    end = clock();
    double timeGreedy = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("\n+-----------------------------------------------------+\n");
    printf("| TONG GIA TRI (Balo 0-1 tham lam): %-18.2f|\n", zeroOneResultGreedy);
    printf("| Thoi gian thuc thi: %.4f ms                      |\n", timeGreedy);
    printf("+-----------------------------------------------------+\n");

    // Khôi phục dữ liệu gốc cho DP
    memcpy(items, itemsCopy, n * sizeof(Item));

    // Giải bài toán balo 0-1 bằng Quy hoạch động
    printf("\n=======================================================\n");
    printf("      GIAI PHAP: BAI TOAN BALO 0-1 (Quy hoach dong)\n");
    printf("          [OK] KET QUA TOI UU DUOC DAM BAO!\n");
    printf("=======================================================\n");

    start = clock();
    int zeroOneResultDP = zeroOneKnapsackDP(items, n, (int)capacity, selectedDP);
    end = clock();
    double timeDP = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;

    printf("\n+-------------------------------------------------------+\n");
    printf("| TONG GIA TRI TOI UU (Balo 0-1 DP): %-18d|\n", zeroOneResultDP);
    printf("| Thoi gian thuc thi: %.4f ms                        |\n", timeDP);
    printf("+-------------------------------------------------------+\n");

    // Bảng so sánh kết quả
    printf("\n+==========================================================+\n");
    printf("|             BANG SO SANH CAC THUAT TOAN                  |\n");
    printf("+=========================================================+\n");
    printf("| Thuat toan        | Gia tri    | Thoi gian   | Do chinh  |\n");
    printf("|                   |            |    (ms)     |    xac    |\n");
    printf("+-------------------+------------+-------------+-----------+\n");
    printf("| Balo phan so      |  %8.2f  |   %8.4f  |  Toi uu   |\n",
           fractionalResult, timeFractional);
    printf("| Tham lam (0-1)    |  %8.2f  |   %8.4f  |  Xap xi   |\n",
           zeroOneResultGreedy, timeGreedy);
    printf("| Quy hoach dong    |  %8d  |   %8.4f  |  Toi uu   |\n",
           zeroOneResultDP, timeDP);
    printf("+-------------------+------------+-------------+-----------+\n");

    double diffPercent = 0;
    if (zeroOneResultDP > 0)
    {
        diffPercent = ((double)(zeroOneResultDP - (int)zeroOneResultGreedy) / zeroOneResultDP) * 100;
    }

    printf("| Chenh lech Tham lam vs DP:  %.2f%%                         |\n", diffPercent);
    printf("+=========================================================+\n");

   // Kết luận
    printf("\n+==========================================================+\n");
    printf("|                       KET LUAN                           |\n");
    printf("+=========================================================+\n");
    printf("| * Balo phan so: Tham lam cho ket qua TOI UU              |\n");
    printf("|   Do phuc tap: O(n log n)                                |\n");
    printf("|                                                          |\n");
    printf("| * Balo 0-1 (Tham lam): Ket qua XAP XI, nhanh             |\n");
    printf("|   Do phuc tap: O(n log n)                                |\n");
    printf("|                                                          |\n");
    printf("| * Balo 0-1 (Quy hoach dong): Ket qua TOI UU              |\n");
    printf("|   Do phuc tap: O(n x W)                                  |\n");
    printf("|                                                          |\n");
    if (diffPercent > 0.01) {
        printf("|   Trong bai toan nay, Tham lam sai %.2f%% so voi DP!  |\n", diffPercent);
    } else {
        printf("|  Trong bai toan nay, Tham lam trung voi DP!             |\n");
    }
    printf("+=========================================================+\n");

    // -----------------------------------------------------------
    // PHẦN 2: GHI KẾT QUẢ RA FILE
    // -----------------------------------------------------------
    char saveChoice;
    printf("\nBan co muon xuat ket qua tong hop ra file 'output.txt' khong? (y/n): ");
    // Thêm khoảng trắng trước %c để xóa bộ đệm nếu còn sót ký tự enter
    scanf(" %c", &saveChoice); 

    if (saveChoice == 'y' || saveChoice == 'Y')
    {
        FILE *fout = fopen("output.txt", "w");
        if (fout == NULL) {
            printf("[Loi] Khong the tao file 'output.txt'!\n");
        } else {
            fprintf(fout, "BAO CAO KET QUA BAI TOAN BALO\n");
            fprintf(fout, "=============================\n\n");
            fprintf(fout, "1. Thong so dau vao:\n");
            fprintf(fout, " - So vat pham: %d\n", n);
            fprintf(fout, " - Suc chua: %.2f\n\n", capacity);
            
            fprintf(fout, "2. Ket qua tinh toan:\n");
            fprintf(fout, " - Balo Phan so (Tham lam): %.2f (Thoi gian: %.4f ms)\n", fractionalResult, timeFractional);
            fprintf(fout, " - Balo 0-1 (Tham lam):     %.2f (Thoi gian: %.4f ms)\n", zeroOneResultGreedy, timeGreedy);
            fprintf(fout, " - Balo 0-1 (Quy hoach dong): %d   (Thoi gian: %.4f ms)\n\n", zeroOneResultDP, timeDP);
            
            fprintf(fout, "3. Ket luan:\n");
            if (diffPercent > 0.01) {
                fprintf(fout, " -> Tham lam cho ket qua kem hon Quy hoach dong %.2f%%.\n", diffPercent);
            } else {
                fprintf(fout, " -> Tham lam va Quy hoach dong cho ket qua nhu nhau.\n");
            }
            
            fclose(fout);
            printf(">> Da xuat ket qua ra file 'output.txt' thanh cong!\n");
        }
    }

    // Giải phóng bộ nhớ
    free(items);
    free(itemsCopy);
    free(selectedGreedy);
    free(selectedDP);
    
    return 0;
}
