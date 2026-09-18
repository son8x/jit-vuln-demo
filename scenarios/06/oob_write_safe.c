/*
 * =============================================================================
 * Tên tệp       : oob_write_safe.c
 * Kịch bản       : Out-of-bounds Write (CWE-787) - PHIÊN BẢN ĐÃ SỬA
 * Mô tả         : Phiên bản an toàn - kiểm tra biên nghiêm ngặt trước khi ghi
 *
 * So với bản gốc, thay đổi:
 *   - Thêm kiểm tra biên: 0 <= vi_tri < KICH_THUOC_MANG
 *   - Trả về mã lỗi (-1) thay vì ghi đè
 *   - In cảnh báo khi vi_tri ngoài phạm vi
 *
 * Đặc điểm:
 *   - CWE-787 fix: bounds checking trước mọi thao tác ghi
 *   - CWE-119 fix: defensive programming
 *   - CWE-20 fix: input validation
 *
 * Dùng trong demo: SCGL phải cho phép commit (CLEAN).
 * =============================================================================
 */

#include <stdio.h>
#include <string.h>

#define KICH_THUOC_MANG    10

typedef struct {
    int kich_thuoc_thuc_te;
    int mang_du_lieu[KICH_THUOC_MANG];
} MangDong;

void khoi_tao_mang_dong(MangDong *md) {
    if (md == NULL) return;
    md->kich_thuoc_thuc_te = KICH_THUOC_MANG;
    for (int i = 0; i < KICH_THUOC_MANG; i++) {
        md->mang_du_lieu[i] = i * 10;
    }
}

void in_mang_dong(const MangDong *md) {
    if (md == NULL) return;
    printf("[Kich thuoc] %d\n", md->kich_thuoc_thuc_te);
    printf("[Mang] ");
    for (int i = 0; i < md->kich_thuoc_thuc_te && i < KICH_THUOC_MANG; i++) {
        printf("%d ", md->mang_du_lieu[i]);
    }
    printf("\n");
}

/* HÀM AN TOÀN - kiểm tra biên nghiêm ngặt */
int cap_nhat_phan_tu(MangDong *md, int vi_tri, int gia_tri_moi) {
    if (md == NULL) {
        printf("[LOI] Con tro NULL\n");
        return -1;
    }
    /* Kiểm tra biên: phải nằm trong [0, KICH_THUOC_MANG) */
    if (vi_tri < 0 || vi_tri >= KICH_THUOC_MANG) {
        printf("[TU CHOI] Vi tri %d ngoai pham vi [0, %d)\n",
               vi_tri, KICH_THUOC_MANG);
        return -1;
    }
    md->mang_du_lieu[vi_tri] = gia_tri_moi;
    return 0;
}

int main(int argc, char **argv) {
    MangDong md;
    khoi_tao_mang_dong(&md);
    in_mang_dong(&md);

    cap_nhat_phan_tu(&md, 2, 999);
    in_mang_dong(&md);

    cap_nhat_phan_tu(&md, -1, 99999);  /* Bị từ chối */
    in_mang_dong(&md);

    return 0;
}
