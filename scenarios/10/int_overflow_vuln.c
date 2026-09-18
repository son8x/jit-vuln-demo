/*
 * =============================================================================
 * Tên tệp       : int_overflow_vuln.c
 * Kịch bản       : Integer Overflow (CWE-190)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - tràn số nguyên khi tính kích thước
 *
 * So với bản gốc, thay đổi:
 *   - Hàm cap_phat_mang() tính kich_thuoc = a * b mà không kiểm tra tràn
 *   - Nếu a * b > INT_MAX, kết quả tràn thành số âm hoặc rất nhỏ
 *   - malloc() cấp phát vùng nhớ nhỏ, nhưng vòng lặp ghi vượt biên
 *
 * Lỗ hổng:
 *   - CWE-190: Integer Overflow or Wraparound
 *   - CWE-122: Heap-based Buffer Overflow (hệ quả)
 *   - CVE liên quan: CVE-2021-30883 (iOS), CVE-2018-6789 (Exim)
 *   - Rủi ro: cấp phát vùng nhớ nhỏ hơn kích thước thật -> ghi đè bộ nhớ khác
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GIOI_HAN_TOI_DA   (1 << 30)  /* 1GB - ngưỡng an toàn */

/* HÀM NÀY CHỨA LỖ HỔNG INTEGER OVERFLOW */
int* cap_phat_va_di_en(int a, int b) {
    /* LỖI: Không kiểm tra tràn khi a * b */
    int kich_thuoc = a * b;  /* Nếu a=65536, b=65536 -> kich_thuoc = 0! */

    if (kich_thuoc <= 0) {
        printf("[CANH BAO] Integer overflow: a=%d, b=%d, kich_thuoc=%d\n",
               a, b, kich_thuoc);
    }

    /* malloc(0) hoặc malloc(số âm) - hành vi không xác định */
    int *mang = (int *)malloc(kich_thuoc * sizeof(int));
    if (mang == NULL) {
        printf("[LOI] malloc that bai\n");
        return NULL;
    }

    /* LỖI: Vòng lặp tin rằng đã cấp đủ bộ nhớ */
    for (int i = 0; i < kich_thuoc; i++) {
        mang[i] = i;  /* Ghi ngoài phạm vi nếu kich_thuoc thực < kich_thuoc kỳ vọng */
    }

    return mang;
}

int main(int argc, char **argv) {
    int a = 65536;
    int b = 65536;

    printf("[Test] Cap phat mang voi a=%d, b=%d\n", a, b);
    int *mang = cap_phat_va_di_en(a, b);
    if (mang != NULL) {
        printf("[Ket qua] mang[0]=%d\n", mang[0]);
        free(mang);
    }

    return 0;
}
