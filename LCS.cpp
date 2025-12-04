#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
using namespace std;

// Tien ich tu viet (khong dung thu vien ho tro)
int max_i(int a, int b) { return (a > b) ? a : b; }

// Cap phat/giai phong mang 2 chieu int kich thuoc (rows x cols)
int** alloc2D(int rows, int cols) {
    int** A = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        A[i] = new int[cols];
        for (int j = 0; j < cols; ++j) A[i][j] = 0; // khoi tao 0
    }
    return A;
}

void free2D(int** A, int rows) {
    if (!A) return;
    for (int i = 0; i < rows; ++i) delete[] A[i];
    delete[] A;
}

// Hàm tính độ dài LCS và lưu bảng DP (khong dung vector)
int computeLCS(const string& X, const string& Y, int** L) {
    int m = X.length();
    int n = Y.length();

    // Co so da khoi tao 0 trong alloc2D
    // Xây dựng bảng L theo công thức quy hoạch động
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (X[i-1] == Y[j-1]) {
                // Trường hợp 1: Ký tự khớp
                L[i][j] = L[i-1][j-1] + 1;
            } else {
                // Trường hợp 2: Ký tự không khớp
                L[i][j] = max_i(L[i-1][j], L[i][j-1]);
            }
        }
    }
    
    return L[m][n];
}

// Hàm truy vết để tìm dãy con chung dài nhất
string traceLCS(const string& X, const string& Y, int** L) {
    string lcs = "";
    int i = X.length();
    int j = Y.length();
    
    // Truy vết ngược từ L[m][n]
    while (i > 0 && j > 0) {
        if (X[i-1] == Y[j-1]) {
            // Ký tự này thuộc LCS
            lcs = X[i-1] + lcs;
            i--;
            j--;
        } else if (L[i-1][j] > L[i][j-1]) {
            // Đi lên trên
            i--;
        } else {
            // Đi sang trái
            j--;
        }
    }
    
    return lcs;
}

// Hàm in bảng DP
void printTable(const string& X, const string& Y, int** L) {
    int m = X.length();
    int n = Y.length();
    
    cout << "\n=== BANG QUY HOACH DONG ===\n";
    cout << "      ";
    for (int j = 0; j < n; j++) {
        cout << setw(3) << Y[j];
    }
    cout << "\n";
    
    for (int i = 0; i <= m; i++) {
        if (i == 0)
            cout << "  ";
        else
            cout << X[i-1] << " ";
        
        for (int j = 0; j <= n; j++) {
            cout << setw(3) << L[i][j];
        }
        cout << "\n";
    }
}

// Hàm in công thức
void printFormula() {
    cout << "\n=== CONG THUC QUY HOACH DONG ===\n";
    cout << "Co so: L[i][0] = 0 va L[0][j] = 0\n";
    cout << "\nCong thuc truy hoi:\n";
    cout << "- Neu X[i] == Y[j]:\n";
    cout << "    L[i][j] = L[i-1][j-1] + 1\n";
    cout << "- Neu X[i] != Y[j]:\n";
    cout << "    L[i][j] = max(L[i-1][j], L[i][j-1])\n";
}

int main() {
    // Thiết lập console để hiển thị tiếng Việt (nếu cần)
    system("chcp 65001 > nul");
    
    cout << "========================================\n";
    cout << "  BAI TOAN LONGEST COMMON SUBSEQUENCE  \n";
    cout << "      (Day con chung dai nhat)         \n";
    cout << "========================================\n";
    
    string X, Y;

    // Chon cach nhap: tu ban phim hoac tu file
    cout << "\nChon cach nhap du lieu:" << '\n';
    cout << "1. Nhap tu ban phim" << '\n';
    cout << "2. Doc 2 chuoi tu file" << '\n';
    cout << "Lua chon (1/2): ";

    int choice = 1;
    if (!(cin >> choice)) {
        cerr << "Nhap lua chon khong hop le. Mac dinh dung ban phim." << '\n';
        cin.clear();
        cin.ignore(32767, '\n');
        choice = 1;
    }

    cin.ignore(32767, '\n'); // xoa bo dem newline

    if (choice == 2) {
        string path;
        cout << "Nhap duong dan file (vi du: Chuoi/test1.txt): ";
        getline(cin, path);

        ifstream fin(path);
        if (!fin) {
            cerr << "Khong mo duoc file: " << path << '\n';
            cerr << "Chuyen sang nhap tu ban phim." << '\n';
        } else {
            // Doc 2 dong dau tien lam 2 chuoi X, Y (bo qua khoang trang cuoi dong)
            if (!getline(fin, X)) X = "";
            if (!getline(fin, Y)) Y = "";
            fin.close();
        }
    }

    if (X.empty() && Y.empty()) {
        // Truong hop chua co du lieu (lua chon 1 hoac file khong hop le)
        cout << "\nNhap chuoi X: ";
        getline(cin, X);
        if (X.empty()) {
            // Neu nguoi dung nhap rong, cho phep nhap lai bang >> de bo qua khoang trang
            cout << "(X bi rong) Nhap lai X (khong khoang trang): ";
            cin >> X;
            cin.ignore(32767, '\n');
        }

        cout << "Nhap chuoi Y: ";
        getline(cin, Y);
        if (Y.empty()) {
            cout << "(Y bi rong) Nhap lai Y (khong khoang trang): ";
            cin >> Y;
            cin.ignore(32767, '\n');
        }
    }
    
    // In công thức
    printFormula();
    
    // Tính LCS
    int m = (int)X.length();
    int n = (int)Y.length();
    int** L = alloc2D(m + 1, n + 1);
    int lengthLCS = computeLCS(X, Y, L);
    
    // In bảng quy hoạch động
    printTable(X, Y, L);
    
    // Truy vết để tìm LCS
    string lcs = traceLCS(X, Y, L);
    
    // In kết quả
    cout << "\n=== KET QUA ===\n";
    cout << "Chuoi X: " << X << " (do dai: " << X.length() << ")\n";
    cout << "Chuoi Y: " << Y << " (do dai: " << Y.length() << ")\n";
    cout << "Do dai LCS: " << lengthLCS << "\n";
    cout << "Day con chung dai nhat: " << lcs << "\n";
    
    cout << "\n========================================\n";
    
    // Giai phong bo nho
    free2D(L, m + 1);

    return 0;
}
