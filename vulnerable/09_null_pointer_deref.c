/*
 * =============================================================================
 * Tên tệp       : null_deref_vuln.c
 * Kịch bản       : NULL Pointer Dereference (CWE-476) - #9 CWE Top 25 (2023)
 * Mô tả         : Phiên bản CÓ LỖ HỔNG - dereference con trỏ NULL gây crash
 *
 * So với bản gốc, thay đổi:
 *   - Hàm tim_nguoi_dung() có thể trả về NULL khi không tìm thấy
 *   - Hàm main() gọi in_nguoi_dung() mà không kiểm tra NULL
 *
 * Lỗ hổng:
 *   - CWE-476: NULL Pointer Dereference
 *   - CWE-754: Improper Check for Unusual or Exceptional Conditions
 *   - CVE liên quan: CVE-2021-30860 (iOS), CVE-2019-11932 (WhatsApp)
 *   - Rủi ro: DoS (crash ứng dụng), trong một số trường hợp có thể leo thang
 *
 * Dùng trong demo: SCGL phải phát hiện commit này là BUGGY.
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int  ma_nguoi_dung;
    char ten[64];
    int  tuoi;
} NguoiDung;

static NguoiDung danh_sach[5] = {
    {1, "Nguyen Van A", 25},
    {2, "Tran Thi B",   30},
    {3, "Le Van C",     22},
    {4, "Pham Thi D",   28},
    {5, "Hoang Van E",  35},
};

/* Hàm này trả về NULL nếu không tìm thấy */
NguoiDung* tim_nguoi_dung(int ma) {
    for (int i = 0; i < 5; i++) {
        if (danh_sach[i].ma_nguoi_dung == ma) {
            return &danh_sach[i];
        }
    }
    return NULL;  /* Không tìm thấy */
}

void in_nguoi_dung(const NguoiDung *nd) {
    /* LỖI: Không kiểm tra nd có NULL không! */
    printf("[User] ma=%d, ten=%s, tuoi=%d\n",
           nd->ma_nguoi_dung, nd->ten, nd->tuoi);  /* CRASH nếu nd == NULL */
}

int main(int argc, char **argv) {
    int ma_can_tim = 999;  /* Không tồn tại trong danh sách */

    NguoiDung *nd = tim_nguoi_dung(ma_can_tim);
    /* LỖI: nd có thể là NULL, nhưng không kiểm tra */
    printf("[CANH BAO] Loi hong NULL Pointer Dereference khi ma=%d\n", ma_can_tim);
    in_nguoi_dung(nd);  /* SẼ CRASH vì nd == NULL */

    return 0;
}
