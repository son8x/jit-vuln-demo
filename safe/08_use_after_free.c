/*
 * =============================================================================
 * Tên tệp       : uaf_safe.c
 * Kịch bản       : Use After Free (CWE-416) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - đặt con trỏ về NULL sau khi free
 *
 * So với bản gốc, thay đổi:
 *   - Sau khi free(), đặt con trỏ về NULL
 *   - Tất cả hàm đều kiểm tra NULL trước khi truy cập
 *   - Không bao giờ sử dụng con trỏ sau khi free
 *
 * Đặc điểm:
 *   - CWE-416 fix: NULL con trỏ sau free, kiểm tra NULL trước mọi truy cập
 *   - CWE-415 fix: không double-free (đã set NULL nên free(NULL) an toàn)
 *   - CWE-690 fix: kiểm tra return value của malloc
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
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

NguoiDung* tao_nguoi_dung(int ma, const char *ten, int tuoi) {
    NguoiDung *nd = (NguoiDung *)malloc(sizeof(NguoiDung));
    if (nd == NULL) return NULL;
    nd->ma_nguoi_dung = ma;
    strncpy(nd->ten, ten, sizeof(nd->ten) - 1);
    nd->ten[sizeof(nd->ten) - 1] = '\0';
    nd->tuoi = tuoi;
    return nd;
}

void in_nguoi_dung(const NguoiDung *nd) {
    if (nd == NULL) {
        printf("[LOI] Con tro NULL\n");
        return;
    }
    printf("[User] ma=%d, ten=%s, tuoi=%d\n",
           nd->ma_nguoi_dung, nd->ten, nd->tuoi);
}

/* HÀM AN TOÀN - free và set về NULL, kiểm tra NULL trước khi dùng */
void xu_ly_dung(NguoiDung **nd_ptr) {
    if (nd_ptr == NULL || *nd_ptr == NULL) return;
    in_nguoi_dung(*nd_ptr);
    free(*nd_ptr);
    *nd_ptr = NULL;  /* Đặt về NULL để tránh UAF */
}

int main(int argc, char **argv) {
    NguoiDung *nd = tao_nguoi_dung(1, "Nguyen Van A", 25);
    if (nd == NULL) return 1;

    xu_ly_dung(&nd);

    /* An toàn: nd đã là NULL, hàm in_nguoi_dung sẽ báo lỗi */
    in_nguoi_dung(nd);

    /* An toàn: free(NULL) không làm gì cả */
    free(nd);

    return 0;
}
