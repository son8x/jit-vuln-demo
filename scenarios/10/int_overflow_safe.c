/*
 * =============================================================================
 * Tên tệp       : int_overflow_safe.c
 * Kịch bản       : Integer Overflow (CWE-190) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - kiểm tra tràn trước khi tính toán
 *
 * So với bản gốc, thay đổi:
 *   - Kiểm tra a và b có vượt ngưỡng cho phép không
 *   - Dùng phép chia để kiểm tra: nếu a > 0 và b > GIOI_HAN / a -> tràn
 *   - Trả về NULL thay vì cấp phát vùng nhớ sai kích thước
 *
 * Đặc điểm:
 *   - CWE-190 fix: kiểm tra overflow trước phép nhân
 *   - CWE-122 fix: ngăn chặn heap overflow hệ quả
 *   - Dùng size_t thay vì int để tăng phạm vi
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GIOI_HAN_TOI_DA   (1UL << 30)  /* 1GB */

/* HÀM AN TOÀN - kiểm tra tràn trước khi cấp phát */
int* cap_phat_va_di_en(size_t a, size_t b) {
    /* Bước 1: Validate input - từ chối nếu quá lớn */
    if (a == 0 || b == 0) {
        printf("[TU CHOI] Kich thuoc phai > 0\n");
        return NULL;
    }
    if (a > GIOI_HAN_TOI_DA || b > GIOI_HAN_TOI_DA) {
        printf("[TU CHOI] Kich thuoc qua lon (a=%zu, b=%zu)\n", a, b);
        return NULL;
    }
    /* Bước 2: Kiểm tra overflow trước phép nhân */
    if (a > GIOI_HAN_TOI_DA / b) {
        printf("[TU CHOI] Tran so nguyen: %zu * %zu > %lu\n",
               a, b, GIOI_HAN_TOI_DA);
        return NULL;
    }
    size_t kich_thuoc = a * b;

    int *mang = (int *)malloc(kich_thuoc * sizeof(int));
    if (mang == NULL) {
        printf("[LOI] malloc that bai\n");
        return NULL;
    }
    for (size_t i = 0; i < kich_thuoc; i++) {
        mang[i] = (int)i;
    }
    return mang;
}

int main(int argc, char **argv) {
    /* Trường hợp an toàn */
    int *mang1 = cap_phat_va_di_en(10, 10);
    if (mang1 != NULL) {
        printf("[Hop le] mang1[5] = %d\n", mang1[5]);
        free(mang1);
    }

    /* Trường hợp gây tràn - bị từ chối */
    int *mang2 = cap_phat_va_di_en(65536, 65536);
    if (mang2 == NULL) {
        printf("[An toan] Tran so nguyen da bi chan\n");
    }

    return 0;
}
